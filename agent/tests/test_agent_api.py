from fastapi.testclient import TestClient

from app.config import AgentSettings
from app.main import create_app


def _payload(**overrides):
    payload = {
        "request_id": "req-1",
        "session_id": "session-1",
        "message": "请查询无人机状态",
        "context": {
            "connected": True,
            "armed": False,
            "battery_percent": 78,
            "flight_mode": "Position",
        },
        "allowed_capabilities": ["vehicle.query_status"],
    }
    payload.update(overrides)
    return payload


def test_normal_message_returns_reply():
    client = TestClient(create_app(AgentSettings()))

    response = client.post("/merivus/agent", json=_payload())
    body = response.json()

    assert response.status_code == 200
    assert body["request_id"] == "req-1"
    assert body["reply"]
    assert body["proposal"] is None
    assert body["provider"] == "mock"
    assert body["model"] == "mock-v1"
    assert body["status"] == "ok"


def test_high_risk_request_only_returns_proposal():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        json=_payload(
            message="请让2号无人机起飞",
            allowed_capabilities=["vehicle.takeoff"],
        ),
    )
    body = response.json()

    assert response.status_code == 200
    assert "无飞行执行权限" in body["reply"]
    assert body["proposal"] == {
        "command": "vehicle.takeoff",
        "arguments": {"vehicle_id": 2},
        "summary": "建议2号无人机执行起飞操作",
    }


def test_high_risk_request_does_not_return_executed_field():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        json=_payload(message="请让1号起飞", allowed_capabilities=["vehicle.takeoff"]),
    )
    body = response.json()

    assert response.status_code == 200
    assert "executed" not in body
    assert "executed" not in body["proposal"]


def test_unauthorized_capability_does_not_generate_proposal():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        json=_payload(message="请让1号无人机起飞", allowed_capabilities=[]),
    )
    body = response.json()

    assert response.status_code == 200
    assert body["proposal"] is None
    assert "未被QGC授权" in body["reply"]


def test_missing_target_does_not_assume_vehicle_id():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        json=_payload(message="请起飞", allowed_capabilities=["vehicle.takeoff"]),
    )
    body = response.json()

    assert response.status_code == 200
    assert body["proposal"]["arguments"]["vehicle_id"] is None
    assert "需要由QGC或用户明确目标" in body["reply"]


def test_context_missing_fields_are_not_fabricated():
    client = TestClient(create_app(AgentSettings()))

    response = client.post(
        "/merivus/agent",
        json=_payload(context={}, message="查询状态"),
    )
    body = response.json()

    assert response.status_code == 200
    assert "connected=" not in body["reply"]
    assert "armed=" not in body["reply"]
    assert "battery_percent=" not in body["reply"]
    assert "flight_mode=" not in body["reply"]


def test_provider_exception_returns_safe_error():
    app = create_app(AgentSettings())

    def raise_error(_request):
        raise RuntimeError("sensitive internal details")

    app.state.agent_service.provider.generate = raise_error
    client = TestClient(app)

    response = client.post("/merivus/agent", json=_payload())
    body = response.json()

    assert response.status_code == 500
    assert body == {
        "error_code": "provider_error",
        "message": "Provider处理失败，已安全拒绝本次请求。",
        "request_id": "req-1",
    }
    assert "sensitive internal details" not in body["message"]


def test_local_token_required_when_configured():
    client = TestClient(create_app(AgentSettings(local_token="dev-secret")))

    response = client.post("/merivus/agent", json=_payload())
    body = response.json()

    assert response.status_code == 401
    assert body == {
        "error_code": "unauthorized",
        "message": "本机请求Token无效。",
        "request_id": "req-1",
    }


def test_local_token_rejects_wrong_value():
    client = TestClient(create_app(AgentSettings(local_token="dev-secret")))

    response = client.post(
        "/merivus/agent",
        json=_payload(),
        headers={"X-Merivus-Token": "wrong"},
    )

    assert response.status_code == 401


def test_local_token_accepts_correct_value():
    client = TestClient(create_app(AgentSettings(local_token="dev-secret")))

    response = client.post(
        "/merivus/agent",
        json=_payload(),
        headers={"X-Merivus-Token": "dev-secret"},
    )
    body = response.json()

    assert response.status_code == 200
    assert body["request_id"] == "req-1"
    assert body["status"] == "ok"
