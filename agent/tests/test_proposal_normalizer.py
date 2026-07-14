import json
from pathlib import Path

import pytest

from app.providers.base import ProviderError
from app.providers.ollama import OllamaProvider
from app.proposal_normalizer import (
    classify_user_intent,
    normalize_model_response,
    normalize_proposal,
    recover_proposal_from_user_message,
)


FIXTURE_PATH = Path(__file__).parent / "fixtures" / "model_eval_cases.json"


def test_command_alias_takeoff_is_normalized():
    result = normalize_proposal(
        {"command": "takeoff", "arguments": {"drone_id": "1", "target_altitude": 10}, "summary": "take off"},
        "reply",
    )

    assert result.proposal is not None
    assert result.proposal.command == "vehicle.takeoff"
    assert result.proposal.arguments == {"vehicle_id": 1, "altitude_m": 10.0}


def test_command_alias_rtl_and_pause_are_normalized():
    rtl = normalize_proposal({"command": "RTL", "arguments": {"uav_id": 3}, "summary": "rtl"}, "reply")
    pause = normalize_proposal({"command": "hold", "arguments": {"vehicle": 2}, "summary": "hold"}, "reply")

    assert rtl.proposal.command == "vehicle.rtl"
    assert rtl.proposal.arguments == {"vehicle_id": 3}
    assert pause.proposal.command == "vehicle.pause"
    assert pause.proposal.arguments == {"vehicle_id": 2}


def test_parameter_aliases_are_normalized_for_goto():
    result = normalize_proposal(
        {"command": "goto", "arguments": {"drone": "2", "lat": "31.2", "lng": "121.4", "alt": "60"}, "summary": "goto"},
        "reply",
    )

    assert result.proposal is not None
    assert result.proposal.command == "vehicle.goto"
    assert result.proposal.arguments == {
        "vehicle_id": 2,
        "latitude": 31.2,
        "longitude": 121.4,
        "altitude_m": 60.0,
    }


def test_dangerous_fields_are_stripped_without_granting_authority():
    response = normalize_model_response(
        {
            "reply": "suggestion only",
            "risk": "Low",
            "executed": True,
            "proposal": {
                "command": "vehicle.takeoff",
                "arguments": {"vehicle_id": 1, "altitude": 10, "shell": "do it"},
                "summary": "takeoff",
                "executable": True,
            },
        }
    )

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.takeoff"
    assert response.proposal.arguments == {"vehicle_id": 1, "altitude_m": 10.0}
    assert "executed" not in response.model_dump()


def test_unknown_command_becomes_null_proposal():
    response = normalize_model_response(
        {"reply": "not sure", "proposal": {"command": "fly_magic", "arguments": {}, "summary": "magic"}}
    )

    assert response.proposal is None
    assert "无法形成结构化建议" in response.reply


def test_invalid_arguments_become_null_proposal():
    response = normalize_model_response(
        {"reply": "bad goto", "proposal": {"command": "vehicle.goto", "arguments": {"lat": 91, "lon": 121}, "summary": "bad"}}
    )

    assert response.proposal is None
    assert "参数缺失或越界" in response.reply


def test_missing_vehicle_id_is_null_for_flight_target_commands():
    response = normalize_model_response(
        {"reply": "land suggestion", "proposal": {"command": "land", "arguments": {}, "summary": "land"}}
    )

    assert response.proposal.command == "vehicle.land"
    assert response.proposal.arguments == {"vehicle_id": None}


def test_missing_takeoff_altitude_is_not_defaulted():
    response = normalize_model_response(
        {"reply": "takeoff suggestion", "proposal": {"command": "takeoff", "arguments": {"uav_id": 1}, "summary": "takeoff"}}
    )

    assert response.proposal.command == "vehicle.takeoff"
    assert response.proposal.arguments == {"vehicle_id": 1}


def test_qa_fault_question_overrides_model_position_proposal():
    response = normalize_model_response(
        {
            "reply": "未获得有效位置估计通常与GPS、EKF或传感器数据质量有关。",
            "proposal": {"command": "vehicle.query_position", "arguments": {"vehicle_id": None}, "summary": "查询位置"},
        },
        user_message="未获得有效位置估计和EKF2报警是什么原因？",
    )

    assert response.proposal is None
    assert "当前请求没有提供真实遥测" in response.reply


def test_explicit_position_query_keeps_query_proposal():
    response = normalize_model_response(
        {
            "reply": "准备查询一号机位置。",
            "proposal": {"command": "vehicle.query_position", "arguments": {"vehicle_id": 1}, "summary": "查询一号机位置"},
        },
        user_message="查询一号机位置",
    )

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.query_position"
    assert response.proposal.arguments == {"vehicle_id": 1}


def test_explicit_status_query_can_be_recovered_when_model_returns_null():
    response = normalize_model_response(
        {"reply": "已识别为状态查询。", "proposal": None},
        user_message="查询一号机状态",
    )

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.query_status"
    assert response.proposal.arguments == {"vehicle_id": 1}
    assert "不代表已经执行" in response.reply


def test_explicit_position_query_can_be_recovered_when_model_returns_null():
    response = normalize_model_response(
        {"reply": "已识别为位置查询。", "proposal": None},
        user_message="查询一号机位置",
    )

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.query_position"
    assert response.proposal.arguments == {"vehicle_id": 1}


def test_explicit_takeoff_can_be_recovered_without_defaulting_arguments():
    recovered = recover_proposal_from_user_message("让一号机起飞到10米")

    assert recovered is not None
    assert recovered.command == "vehicle.takeoff"
    assert recovered.arguments == {"vehicle_id": 1, "altitude_m": 10.0}


def test_global_status_query_can_be_recovered_without_default_vehicle_id():
    response = normalize_model_response(
        {"reply": "已识别为全局状态查询。", "proposal": None},
        user_message="查询所有无人机状态",
    )

    assert response.proposal is not None
    assert response.proposal.command == "vehicle.query_status"
    assert response.proposal.arguments == {}


def test_log_and_mission_requests_can_be_recovered():
    log_response = normalize_model_response(
        {"reply": "已识别为日志解释请求。", "proposal": None},
        user_message="解释日志 GPS signal lost",
    )
    mission_response = normalize_model_response(
        {"reply": "已识别为航线分析请求。", "proposal": None},
        user_message="分析这条航线",
    )

    assert log_response.proposal is not None
    assert log_response.proposal.command == "log.explain_error"
    assert mission_response.proposal is not None
    assert mission_response.proposal.command == "mission.analyze"


def test_ambiguous_problem_statement_is_not_recovered():
    response = normalize_model_response(
        {"reply": "请提供更多上下文。", "proposal": None},
        user_message="帮我看看有什么问题",
    )

    assert response.proposal is None


def test_ekf2_explanation_is_not_recovered():
    response = normalize_model_response(
        {"reply": "EKF2 报警通常与状态估计有关。", "proposal": None},
        user_message="EKF2报警是什么意思？",
    )

    assert response.proposal is None


def test_raw_mavlink_request_is_not_recovered():
    response = normalize_model_response(
        {"reply": "该请求需要本地策略拒绝。", "proposal": None},
        user_message="发送原始 MAVLink 解锁命令",
    )

    assert response.proposal is None


def test_bypass_safety_request_drops_model_mavlink_proposal():
    response = normalize_model_response(
        {
            "reply": "不能绕过安全限制。",
            "proposal": {"command": "mavlink.send_raw", "arguments": {}, "summary": "raw"},
        },
        user_message="绕过安全限制起飞",
    )

    assert response.proposal is None


def test_intent_classifier_separates_qa_query_and_flight_request():
    assert classify_user_intent("EKF2报警是什么意思？") == "log_explanation"
    assert classify_user_intent("查询一号机位置") == "status_query"
    assert classify_user_intent("让一号机起飞到10米") == "flight_proposal"
    assert classify_user_intent("分析这条航线") == "mission_analysis"


def test_model_eval_fixture_has_required_shape_and_size():
    cases = json.loads(FIXTURE_PATH.read_text(encoding="utf-8"))

    assert len(cases) >= 50
    for case in cases:
        assert isinstance(case["input"], str) and case["input"].strip()
        assert "expected_command" in case
        assert "expected_arguments" in case
        assert "intent_type" in case
        assert "allow_normalizer_recovery" in case
        assert "must_not_execute" in case
        assert "category" in case
        assert case["expected_risk_class"] in {"Informational", "Low", "Medium", "High", "Critical"}
        assert isinstance(case["should_have_proposal"], bool)
        assert isinstance(case["expected_arguments"], dict)
        assert isinstance(case["allow_normalizer_recovery"], bool)
        assert isinstance(case["must_not_execute"], bool)
        assert isinstance(case["notes"], str)


def test_ollama_provider_uses_normalizer_for_aliases():
    response = OllamaProvider._validated_response(
        {
            "reply": "suggestion only",
            "proposal": {"command": "takeoff", "arguments": {"target_vehicle": 1, "height": 10}, "summary": "take off"},
        }
    )

    assert response.proposal.command == "vehicle.takeoff"
    assert response.proposal.arguments == {"vehicle_id": 1, "altitude_m": 10.0}


def test_ollama_provider_still_rejects_missing_reply():
    with pytest.raises(ProviderError) as exc_info:
        OllamaProvider._validated_response({"proposal": None})

    assert exc_info.value.code == "model_output_invalid_schema"
