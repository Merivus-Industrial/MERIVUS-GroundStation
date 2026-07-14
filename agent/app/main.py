from __future__ import annotations

import logging
import time
from collections.abc import Callable

from fastapi import FastAPI, Header, Request, status
from fastapi.exceptions import RequestValidationError
from fastapi.responses import JSONResponse

from app.config import AGENT_VERSION, DEFAULT_CAPABILITIES, SERVICE_NAME, AgentSettings, configure_logging
from app.schemas import AgentRequest, AgentResponse, ErrorResponse
from app.services.agent_service import AgentService, ProviderError, UnknownProviderError, provider_model

logger = logging.getLogger("merivus.agent")


def create_app(settings: AgentSettings | None = None) -> FastAPI:
    settings = settings or AgentSettings.from_env()
    configure_logging(settings)

    app = FastAPI(title="MERIVUS Local Agent", version=AGENT_VERSION)
    app.state.settings = settings
    try:
        app.state.agent_service = AgentService(settings)
    except UnknownProviderError as exc:
        app.state.agent_service = None
        logger.error("Configured provider is unavailable: %s", exc.message)

    @app.middleware("http")
    async def request_logging_middleware(request: Request, call_next: Callable):
        started = time.perf_counter()
        response = await call_next(request)
        elapsed_ms = (time.perf_counter() - started) * 1000
        request_id = getattr(request.state, "request_id", None)
        logger.info(
            "request completed method=%s path=%s status=%s request_id=%s elapsed_ms=%.2f",
            request.method,
            request.url.path,
            response.status_code,
            request_id,
            elapsed_ms,
        )
        return response

    @app.exception_handler(RequestValidationError)
    async def validation_exception_handler(request: Request, exc: RequestValidationError):
        request_id = _request_id_from_body(getattr(exc, "body", None))
        error_code = "invalid_json" if _has_json_error(exc) else "validation_error"
        http_status = status.HTTP_400_BAD_REQUEST if error_code == "invalid_json" else status.HTTP_422_UNPROCESSABLE_ENTITY
        return _error_response(
            error_code=error_code,
            message="请求JSON格式错误。" if error_code == "invalid_json" else "请求字段未通过验证。",
            request_id=request_id,
            http_status=http_status,
        )

    @app.get("/health")
    async def health():
        return {
            "status": "ok",
            "service": SERVICE_NAME,
            "provider": settings.provider,
            "version": AGENT_VERSION,
        }

    @app.get("/merivus/info")
    async def info():
        service = app.state.agent_service
        provider_info = service.info() if service is not None else None
        return {
            "service": SERVICE_NAME,
            "version": AGENT_VERSION,
            "provider": settings.provider,
            "model": provider_info.model if provider_info is not None else provider_model(settings.provider, settings),
            "provider_ready": provider_info.provider_ready if provider_info is not None else False,
            "provider_error": provider_info.provider_error if provider_info is not None else f"Unknown provider: {settings.provider}",
            "available_models": provider_info.available_models if provider_info is not None else [],
            "external_network_enabled": provider_info.external_network_enabled if provider_info is not None else False,
            "flight_execution_enabled": provider_info.flight_execution_enabled if provider_info is not None else False,
            "supported_capabilities": DEFAULT_CAPABILITIES,
            "max_message_length": settings.max_message_length,
        }

    @app.post("/merivus/agent", response_model=AgentResponse)
    async def merivus_agent(
        request: Request,
        agent_request: AgentRequest,
        x_merivus_token: str | None = Header(default=None),
    ):
        request.state.request_id = agent_request.request_id
        if settings.local_token and x_merivus_token != settings.local_token:
            return _error_response("unauthorized", "本机请求Token无效。", agent_request.request_id, status.HTTP_401_UNAUTHORIZED)

        if len(agent_request.message) > settings.max_message_length:
            return _error_response(
                "message_too_long",
                "消息长度超过本机Agent配置上限。",
                agent_request.request_id,
                status.HTTP_422_UNPROCESSABLE_ENTITY,
            )

        service = request.app.state.agent_service
        if service is None:
            return _error_response(
                "provider_not_found",
                "当前Provider不可用。",
                agent_request.request_id,
                status.HTTP_500_INTERNAL_SERVER_ERROR,
            )

        try:
            response_data = service.generate(agent_request)
        except ProviderError as exc:
            logger.warning("Provider error request_id=%s provider=%s code=%s", agent_request.request_id, settings.provider, exc.code)
            return _error_response(
                exc.code,
                exc.message,
                agent_request.request_id,
                status.HTTP_503_SERVICE_UNAVAILABLE,
            )
        except Exception:
            logger.exception("Provider error request_id=%s provider=%s", agent_request.request_id, settings.provider)
            return _error_response(
                "provider_error",
                "Provider处理失败，已安全拒绝本次请求。",
                agent_request.request_id,
                status.HTTP_500_INTERNAL_SERVER_ERROR,
            )

        return AgentResponse(
            request_id=agent_request.request_id,
            reply=response_data.reply,
            proposal=response_data.proposal,
            provider=service.provider_name,
            model=service.model,
            status="ok",
        )

    logger.info("starting service=%s host=%s port=%s provider=%s", SERVICE_NAME, settings.host, settings.port, settings.provider)
    return app


def _has_json_error(exc: RequestValidationError) -> bool:
    return any(error.get("type") == "json_invalid" for error in exc.errors())


def _request_id_from_body(body: object) -> str | None:
    if isinstance(body, dict):
        value = body.get("request_id")
        return value if isinstance(value, str) else None
    return None


def _error_response(error_code: str, message: str, request_id: str | None, http_status: int) -> JSONResponse:
    return JSONResponse(
        status_code=http_status,
        content=ErrorResponse(error_code=error_code, message=message, request_id=request_id).model_dump(),
    )


app = create_app()
