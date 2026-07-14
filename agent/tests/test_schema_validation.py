from fastapi.testclient import TestClient

from app.config import AgentSettings
from app.main import create_app


def _payload(**overrides):
    payload = {
        "request_id": "req-1",
        "session_id": "session-1",
        "message": "普通问题",
        "context": {},
        "allowed_capabilities": [],
    }
    payload.update(overrides)
    return payload


def test_empty_message_returns_validation_error():
    client = TestClient(create_app(AgentSettings()))

    response = client.post("/merivus/agent", json=_payload(message="   "))

    assert response.status_code == 422
    assert response.json()["error_code"] == "validation_error"


def test_too_long_message_returns_validation_error():
    client = TestClient(create_app(AgentSettings(max_message_length=4)))

    response = client.post("/merivus/agent", json=_payload(message="12345"))

    assert response.status_code == 422
    assert response.json() == {
        "error_code": "message_too_long",
        "message": "消息长度超过本机Agent配置上限。",
        "request_id": "req-1",
    }


def test_missing_request_id_returns_validation_error():
    client = TestClient(create_app(AgentSettings()))
    payload = _payload()
    payload.pop("request_id")

    response = client.post("/merivus/agent", json=payload)

    assert response.status_code == 422
    assert response.json()["error_code"] == "validation_error"


def test_missing_session_id_returns_validation_error():
    client = TestClient(create_app(AgentSettings()))
    payload = _payload()
    payload.pop("session_id")

    response = client.post("/merivus/agent", json=payload)

    assert response.status_code == 422
    assert response.json()["error_code"] == "validation_error"
    assert response.json()["request_id"] == "req-1"


def test_extra_top_level_field_is_rejected():
    client = TestClient(create_app(AgentSettings()))

    response = client.post("/merivus/agent", json=_payload(unexpected=True))

    assert response.status_code == 422
    assert response.json()["error_code"] == "validation_error"


def test_invalid_json_is_rejected():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        content="{",
        headers={"Content-Type": "application/json"},
    )

    assert response.status_code == 400
    assert response.json()["error_code"] == "invalid_json"
