from __future__ import annotations

from abc import ABC, abstractmethod
from dataclasses import dataclass, field

from app.schemas import AgentRequest, AgentResponseData


@dataclass(frozen=True)
class ProviderHealth:
    ready: bool
    error: str | None = None
    available_models: list[str] = field(default_factory=list)


@dataclass(frozen=True)
class ProviderInfo:
    provider: str
    model: str
    provider_ready: bool
    provider_error: str | None = None
    available_models: list[str] = field(default_factory=list)
    external_network_enabled: bool = False
    flight_execution_enabled: bool = False


class ProviderError(RuntimeError):
    def __init__(self, code: str, message: str) -> None:
        super().__init__(message)
        self.code = code
        self.message = message


class AgentProvider(ABC):
    name: str
    model: str

    @abstractmethod
    def generate(self, request: AgentRequest) -> AgentResponseData:
        """Return a structured response without executing flight actions."""

    @abstractmethod
    def health(self) -> ProviderHealth:
        """Return provider readiness without executing flight actions."""

    def info(self) -> ProviderInfo:
        health = self.health()
        return ProviderInfo(
            provider=self.name,
            model=self.model,
            provider_ready=health.ready,
            provider_error=health.error,
            available_models=health.available_models,
            external_network_enabled=False,
            flight_execution_enabled=False,
        )
