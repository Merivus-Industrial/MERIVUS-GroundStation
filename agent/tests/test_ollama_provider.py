import json

import httpx
import pytest
from fastapi.testclient import TestClient

from app.config import AgentSettings
from app.main import create_app
from app.providers.base import ProviderError
from app.providers.mock import MockProvider
from app.providers.ollama import OllamaProvider
from app.providers.router import ProviderRouter
from app.schemas import AgentContext, AgentRequest


def _agent_request(message="hello", allowed_capabilities=None):
    return AgentRequest(
        request_id="req-1",
        session_id="session-1",
        message=message,
        context=AgentContext(connected=True, armed=False),
        allowed_capabilities=allowed_capabilities or [],
    )


def _chat_response(content):
    return {"message": {"role": "assistant", "content": content}, "done": True}


def _provider(models=None, chat_content=None, tags_status=200, chat_status=200, raise_on=None):
    calls = []
    models = ["qwen3:8b"] if models is None else models
    chat_content = '{"reply":"ok","proposal":null}' if chat_content is None else chat_content

    def handler(request):
        calls.append(request)
        if raise_on == request.url.path:
            raise httpx.ConnectError("boom", request=request)
        if request.url.path == "/api/tags":
            return httpx.Response(
                tags_status,
                json={"models": [{"name": name} for name in models]},
                request=request,
            )
        if request.url.path == "/api/chat":
            return httpx.Response(
                chat_status,
                json=_chat_response(chat_content),
                request=request,
            )
        return httpx.Response(404, request=request)

    client = httpx.Client(base_url="http://ollama.test", transport=httpx.MockTransport(handler))
    settings = AgentSettings(provider="ollama", ollama_base_url="http://ollama.test", ollama_model="qwen3:8b")
    return OllamaProvider(settings, client), calls


def test_ollama_service_available_and_model_exists():
    provider, _calls = _provider(models=["qwen3:8b", "nomic-embed-text"])

    health = provider.health()

    assert health.ready is True
    assert health.error is None
    assert "qwen3:8b" in health.available_models


def test_ollama_service_unavailable():
    provider, _calls = _provider(raise_on="/api/tags")

    health = provider.health()

    assert health.ready is False
    assert health.error == "Ollama service is not available"


def test_ollama_model_missing():
    provider, _calls = _provider(models=["llama3.1:8b"])

    health = provider.health()

    assert health.ready is False
    assert health.error == "Model qwen3:8b is not installed"
    assert health.available_models == ["llama3.1:8b"]


def test_ollama_generate_normal_reply_sets_chat_contract():
    provider, calls = _provider(chat_content='{"reply":"GPS status is unknown from context.","proposal":null}')

    response = provider.generate(_agent_request())

    assert response.reply == "GPS status is unknown from context."
    assert response.proposal is None
    chat_call = [call for call in calls if call.url.path == "/api/chat"][0]
    payload = json.loads(chat_call.content.decode("utf-8"))
    assert payload["stream"] is False
    assert payload["think"] is False
    assert payload["model"] == "qwen3:8b"
    assert payload["format"]["required"] == ["reply", "proposal"]


def test_ollama_generate_accepts_proposal_null():
    provider, _calls = _provider(chat_content='{"reply":"No action proposed.","proposal":null}')

    response = provider.generate(_agent_request())

    assert response.reply == "No action proposed."
    assert response.proposal is None


def test_ollama_generate_accepts_high_risk_proposal_as_suggestion_only():
    provider, _calls = _provider(
        chat_content=json.dumps(
            {
                "reply": "This is only a structured suggestion and has not been executed.",
                "proposal": {
                    "command": "vehicle.takeoff",
                    "arguments": {"vehicle_id": 1, "altitude_m": 10},
                    "summary": "Suggest reviewing takeoff for UAV-1.",
                },
            }
        )
    )

    response = provider.generate(_agent_request("take off", ["vehicle.takeoff"]))

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.takeoff"
    assert response.proposal.arguments == {"vehicle_id": 1, "altitude_m": 10}


def test_ollama_rejects_invalid_model_json():
    provider, _calls = _provider(chat_content="not json")

    with pytest.raises(ProviderError) as exc_info:
        provider.generate(_agent_request())

    assert exc_info.value.code == "model_output_invalid_json"


def test_ollama_rejects_missing_reply():
    provider, _calls = _provider(chat_content='{"proposal":null}')

    with pytest.raises(ProviderError) as exc_info:
        provider.generate(_agent_request())

    assert exc_info.value.code == "model_output_invalid_schema"


def test_ollama_normalizes_arguments_with_wrong_type_to_null_proposal():
    provider, _calls = _provider(
        chat_content=json.dumps(
            {
                "reply": "bad arguments",
                "proposal": {"command": "mission.analyze", "arguments": [], "summary": "bad"},
            }
        )
    )

    response = provider.generate(_agent_request())

    assert response.proposal is None
    assert "无法形成结构化建议" in response.reply


def test_ollama_strips_executed_field():
    provider, _calls = _provider(chat_content='{"reply":"done","proposal":null,"executed":true}')

    response = provider.generate(_agent_request())

    assert response.reply == "done"
    assert response.proposal is None


def test_ollama_timeout_is_handled():
    def handler(request):
        raise httpx.TimeoutException("slow", request=request)

    client = httpx.Client(base_url="http://ollama.test", transport=httpx.MockTransport(handler))
    provider = OllamaProvider(AgentSettings(provider="ollama"), client)

    health = provider.health()

    assert health.ready is False
    assert health.error == "Ollama service request timed out"


def test_ollama_http_500_is_handled():
    provider, _calls = _provider(tags_status=500)

    health = provider.health()

    assert health.ready is False
    assert health.error == "Ollama returned HTTP 500"


def test_unknown_provider_returns_clear_info_error():
    client = TestClient(create_app(AgentSettings(provider="missing")))

    info_response = client.get("/merivus/info")
    chat_response = client.post(
        "/merivus/agent",
        json={"request_id": "req-1", "session_id": "s", "message": "hello", "context": {}, "allowed_capabilities": []},
    )

    assert info_response.status_code == 200
    assert info_response.json()["provider_ready"] is False
    assert info_response.json()["provider_error"] == "Unknown provider: missing"
    assert chat_response.status_code == 500


class _FailingProvider(MockProvider):
    name = "ollama"
    model = "qwen3:8b"

    def generate(self, request):
        raise ProviderError("provider_not_ready", "forced failure")


def test_router_does_not_silently_fallback_to_mock():
    router = ProviderRouter(AgentSettings(provider="mock", allow_mock_fallback=False))
    router._provider = _FailingProvider()

    with pytest.raises(ProviderError):
        router.generate(_agent_request())


def test_router_explicitly_fallbacks_to_mock():
    router = ProviderRouter(AgentSettings(provider="mock", allow_mock_fallback=True))
    router._provider = _FailingProvider()

    response = router.generate(_agent_request("status", ["vehicle.query_status"]))

    assert response.reply
    assert response.proposal is None
