from __future__ import annotations

from typing import Any, Literal

from pydantic import BaseModel, ConfigDict, Field, field_validator


class AgentContext(BaseModel):
    model_config = ConfigDict(extra="forbid")

    vehicle_count: int | None = None
    active_vehicle_id: int | None = None
    connected: bool | None = None
    armed: bool | None = None
    flight_mode: str | None = None
    battery_percent: float | None = None
    gps_valid: bool | None = None
    rtk_fix_type: str | None = None
    link_state: str | None = None


class AgentRequest(BaseModel):
    model_config = ConfigDict(extra="forbid")

    request_id: str
    session_id: str
    message: str
    context: AgentContext = Field(default_factory=AgentContext)
    allowed_capabilities: list[str] = Field(default_factory=list)

    @field_validator("request_id", "session_id")
    @classmethod
    def non_empty_id(cls, value: str) -> str:
        if not value or not value.strip():
            raise ValueError("must not be empty")
        return value.strip()

    @field_validator("message")
    @classmethod
    def non_empty_message(cls, value: str) -> str:
        stripped = value.strip() if value else ""
        if not stripped:
            raise ValueError("message must not be empty")
        return stripped


class Proposal(BaseModel):
    model_config = ConfigDict(extra="forbid")

    command: str
    arguments: dict[str, Any] = Field(default_factory=dict)
    summary: str


class AgentResponse(BaseModel):
    model_config = ConfigDict(extra="forbid")

    request_id: str
    reply: str
    proposal: Proposal | None = None
    provider: str
    model: str
    status: Literal["ok"]


class ErrorResponse(BaseModel):
    model_config = ConfigDict(extra="forbid")

    error_code: str
    message: str
    request_id: str | None = None


class AgentResponseData(BaseModel):
    reply: str
    proposal: Proposal | None = None
