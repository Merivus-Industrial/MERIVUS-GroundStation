from __future__ import annotations

import logging
import os
from dataclasses import dataclass


SERVICE_NAME = "merivus-agent"
AGENT_VERSION = "0.1.0"
DEFAULT_MODEL = "mock-v1"
DEFAULT_OLLAMA_BASE_URL = "http://127.0.0.1:11434"
DEFAULT_OLLAMA_MODEL = "qwen3:8b"
DEFAULT_OLLAMA_TIMEOUT_SECONDS = 60
DEFAULT_CAPABILITIES = [
    "vehicle.query_status",
    "vehicle.query_battery",
    "vehicle.query_gps",
    "vehicle.query_rtk",
    "log.explain_error",
    "mission.analyze",
    "mission.create_draft",
    "vehicle.takeoff",
    "vehicle.arm",
    "vehicle.land",
    "vehicle.rtl",
    "vehicle.pause",
    "vehicle.hold",
    "vehicle.change_mode",
    "mission.upload",
    "mission.start",
]


@dataclass(frozen=True)
class AgentSettings:
    host: str = "127.0.0.1"
    port: int = 8765
    provider: str = "mock"
    ollama_base_url: str = DEFAULT_OLLAMA_BASE_URL
    ollama_model: str = DEFAULT_OLLAMA_MODEL
    ollama_timeout_seconds: int = DEFAULT_OLLAMA_TIMEOUT_SECONDS
    allow_mock_fallback: bool = False
    log_level: str = "INFO"
    max_message_length: int = 8000
    local_token: str | None = None

    @classmethod
    def from_env(cls) -> "AgentSettings":
        host = os.getenv("MERIVUS_AGENT_HOST", "127.0.0.1").strip() or "127.0.0.1"
        if host == "0.0.0.0":
            host = "127.0.0.1"

        return cls(
            host=host,
            port=_parse_int(os.getenv("MERIVUS_AGENT_PORT"), 8765),
            provider=os.getenv("MERIVUS_AGENT_PROVIDER", "mock").strip().lower() or "mock",
            ollama_base_url=_normalize_base_url(os.getenv("MERIVUS_OLLAMA_BASE_URL"), DEFAULT_OLLAMA_BASE_URL),
            ollama_model=os.getenv("MERIVUS_OLLAMA_MODEL", DEFAULT_OLLAMA_MODEL).strip() or DEFAULT_OLLAMA_MODEL,
            ollama_timeout_seconds=_parse_int(os.getenv("MERIVUS_OLLAMA_TIMEOUT_SECONDS"), DEFAULT_OLLAMA_TIMEOUT_SECONDS),
            allow_mock_fallback=_parse_bool(os.getenv("MERIVUS_AGENT_ALLOW_MOCK_FALLBACK"), False),
            log_level=os.getenv("MERIVUS_AGENT_LOG_LEVEL", "INFO").strip().upper() or "INFO",
            max_message_length=_parse_int(os.getenv("MERIVUS_AGENT_MAX_MESSAGE_LENGTH"), 8000),
            local_token=os.getenv("MERIVUS_LOCAL_TOKEN") or None,
        )


def _parse_int(raw: str | None, default: int) -> int:
    if raw is None:
        return default
    try:
        return int(raw)
    except ValueError:
        return default


def _parse_bool(raw: str | None, default: bool) -> bool:
    if raw is None:
        return default
    value = raw.strip().lower()
    if value in {"1", "true", "yes", "on"}:
        return True
    if value in {"0", "false", "no", "off"}:
        return False
    return default


def _normalize_base_url(raw: str | None, default: str) -> str:
    value = (raw or default).strip().rstrip("/")
    return value or default


def configure_logging(settings: AgentSettings) -> None:
    level = getattr(logging, settings.log_level, logging.INFO)
    logging.basicConfig(
        level=level,
        format="%(asctime)s %(levelname)s %(name)s %(message)s",
    )
