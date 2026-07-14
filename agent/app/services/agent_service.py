from __future__ import annotations

from app.config import AgentSettings
from app.providers.base import ProviderError, ProviderInfo
from app.providers.router import ProviderRouter, UnknownProviderError, provider_model
from app.schemas import AgentRequest, AgentResponseData


class AgentService:
    def __init__(self, settings: AgentSettings) -> None:
        self.router = ProviderRouter(settings)

    @property
    def provider(self):
        return self.router._provider

    @property
    def provider_name(self) -> str:
        return self.router.provider_name

    @property
    def model(self) -> str:
        return self.router.model

    def generate(self, request: AgentRequest) -> AgentResponseData:
        return self.router.generate(request)

    def info(self) -> ProviderInfo:
        return self.router.info()


__all__ = ["AgentService", "ProviderError", "UnknownProviderError", "provider_model"]
