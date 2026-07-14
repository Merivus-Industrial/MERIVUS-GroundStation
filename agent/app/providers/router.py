from __future__ import annotations

from app.config import AgentSettings, DEFAULT_MODEL, DEFAULT_OLLAMA_MODEL
from app.providers.base import AgentProvider, ProviderError, ProviderHealth, ProviderInfo
from app.providers.mock import MockProvider
from app.providers.ollama import OllamaProvider
from app.schemas import AgentRequest, AgentResponseData


class UnknownProviderError(ProviderError):
    def __init__(self, provider_name: str) -> None:
        super().__init__("provider_not_found", f"Unknown provider: {provider_name}")


class ProviderRouter:
    def __init__(self, settings: AgentSettings) -> None:
        self._settings = settings
        self._mock_provider = MockProvider()
        self._provider = self._create_provider(settings.provider)

    @property
    def provider_name(self) -> str:
        return self._provider.name

    @property
    def model(self) -> str:
        return self._provider.model

    def generate(self, request: AgentRequest) -> AgentResponseData:
        try:
            return self._provider.generate(request)
        except ProviderError:
            if self._settings.allow_mock_fallback and self._provider.name != self._mock_provider.name:
                return self._mock_provider.generate(request)
            raise

    def health(self) -> ProviderHealth:
        return self._provider.health()

    def info(self) -> ProviderInfo:
        return self._provider.info()

    def _create_provider(self, provider_name: str) -> AgentProvider:
        if provider_name == "mock":
            return self._mock_provider
        if provider_name == "ollama":
            return OllamaProvider(self._settings)
        raise UnknownProviderError(provider_name)


def provider_model(provider_name: str, settings: AgentSettings | None = None) -> str:
    if provider_name == "mock":
        return DEFAULT_MODEL
    if provider_name == "ollama":
        return settings.ollama_model if settings is not None else DEFAULT_OLLAMA_MODEL
    return "unavailable"
