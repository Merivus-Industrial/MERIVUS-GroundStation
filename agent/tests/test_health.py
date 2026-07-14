from fastapi.testclient import TestClient

from app.config import AgentSettings
from app.main import create_app


def test_health_returns_ok():
    client = TestClient(create_app(AgentSettings()))

    response = client.get("/health")

    assert response.status_code == 200
    assert response.json() == {
        "status": "ok",
        "service": "merivus-agent",
        "provider": "mock",
        "version": "0.1.0",
    }


def test_info_returns_mock_and_disables_execution():
    client = TestClient(create_app(AgentSettings()))

    response = client.get("/merivus/info")
    body = response.json()

    assert response.status_code == 200
    assert body["provider"] == "mock"
    assert body["model"] == "mock-v1"
    assert body["external_network_enabled"] is False
    assert body["flight_execution_enabled"] is False
    assert body["max_message_length"] == 8000


def test_health_does_not_call_provider():
    app = create_app(AgentSettings())

    def fail_generate(_request):
        raise AssertionError("health must not call provider")

    app.state.agent_service.provider.generate = fail_generate
    client = TestClient(app)

    response = client.get("/health")

    assert response.status_code == 200
