from __future__ import annotations

import re
from dataclasses import dataclass
from typing import Any

from app.schemas import AgentResponseData, Proposal


STANDARD_COMMANDS = {
    "vehicle.query_status",
    "vehicle.query_battery",
    "vehicle.query_position",
    "vehicle.query_rtk",
    "log.explain_error",
    "ui.select_vehicle",
    "ui.open_page",
    "map.focus_coordinate",
    "mission.create_draft",
    "mission.analyze",
    "vehicle.arm",
    "vehicle.takeoff",
    "vehicle.land",
    "vehicle.rtl",
    "vehicle.pause",
    "vehicle.goto",
    "mission.upload",
    "mission.start",
    "param.write",
    "mavlink.send_raw",
}

COMMAND_ALIASES = {
    "status": "vehicle.query_status",
    "query_status": "vehicle.query_status",
    "vehicle.status": "vehicle.query_status",
    "查看状态": "vehicle.query_status",
    "查询状态": "vehicle.query_status",
    "无人机状态": "vehicle.query_status",
    "电量": "vehicle.query_battery",
    "battery": "vehicle.query_battery",
    "query_battery": "vehicle.query_battery",
    "gps": "vehicle.query_position",
    "position": "vehicle.query_position",
    "定位": "vehicle.query_position",
    "rtk": "vehicle.query_rtk",
    "log": "log.explain_error",
    "error_log": "log.explain_error",
    "解释日志": "log.explain_error",
    "select_vehicle": "ui.select_vehicle",
    "选择无人机": "ui.select_vehicle",
    "open_page": "ui.open_page",
    "打开页面": "ui.open_page",
    "focus_coordinate": "map.focus_coordinate",
    "地图定位": "map.focus_coordinate",
    "mission_draft": "mission.create_draft",
    "任务草稿": "mission.create_draft",
    "mission_analysis": "mission.analyze",
    "mission.analyse": "mission.analyze",
    "分析航线": "mission.analyze",
    "arm": "vehicle.arm",
    "解锁": "vehicle.arm",
    "force_arm": "vehicle.arm",
    "强制解锁": "vehicle.arm",
    "takeoff": "vehicle.takeoff",
    "take off": "vehicle.takeoff",
    "起飞": "vehicle.takeoff",
    "起飞到10米": "vehicle.takeoff",
    "land": "vehicle.land",
    "降落": "vehicle.land",
    "rtl": "vehicle.rtl",
    "return_to_launch": "vehicle.rtl",
    "return to launch": "vehicle.rtl",
    "返航": "vehicle.rtl",
    "回家": "vehicle.rtl",
    "pause": "vehicle.pause",
    "hold": "vehicle.pause",
    "暂停": "vehicle.pause",
    "悬停": "vehicle.pause",
    "goto": "vehicle.goto",
    "go_to": "vehicle.goto",
    "飞到坐标": "vehicle.goto",
    "upload_mission": "mission.upload",
    "上传任务": "mission.upload",
    "start_mission": "mission.start",
    "启动任务": "mission.start",
    "write_param": "param.write",
    "参数写入": "param.write",
    "send_mavlink": "mavlink.send_raw",
    "mavlink": "mavlink.send_raw",
}

ARGUMENT_ALIASES = {
    "drone_id": "vehicle_id",
    "drone": "vehicle_id",
    "uav_id": "vehicle_id",
    "uav": "vehicle_id",
    "vehicle": "vehicle_id",
    "target_vehicle": "vehicle_id",
    "aircraft_id": "vehicle_id",
    "vehicleId": "vehicle_id",
    "height": "altitude_m",
    "altitude": "altitude_m",
    "alt": "altitude_m",
    "takeoff_height": "altitude_m",
    "target_altitude": "altitude_m",
    "targetAltitude": "altitude_m",
    "lat": "latitude",
    "latitude": "latitude",
    "lng": "longitude",
    "lon": "longitude",
    "longitude": "longitude",
    "page": "page",
    "view": "page",
    "panel": "page",
}

ALLOWED_ARGUMENTS = {
    "vehicle.query_status": {"vehicle_id"},
    "vehicle.query_battery": {"vehicle_id"},
    "vehicle.query_position": {"vehicle_id"},
    "vehicle.query_rtk": {"vehicle_id"},
    "log.explain_error": {"vehicle_id"},
    "ui.select_vehicle": {"vehicle_id"},
    "ui.open_page": {"page"},
    "map.focus_coordinate": {"latitude", "longitude"},
    "mission.create_draft": {"vehicle_id"},
    "mission.analyze": {"vehicle_id"},
    "vehicle.arm": {"vehicle_id"},
    "vehicle.takeoff": {"vehicle_id", "altitude_m"},
    "vehicle.land": {"vehicle_id"},
    "vehicle.rtl": {"vehicle_id"},
    "vehicle.pause": {"vehicle_id"},
    "vehicle.goto": {"vehicle_id", "latitude", "longitude", "altitude_m"},
    "mission.upload": {"vehicle_id"},
    "mission.start": {"vehicle_id"},
    "param.write": {"name", "value"},
    "mavlink.send_raw": set(),
}

DANGEROUS_FIELDS = {
    "executed",
    "executable",
    "risk",
    "localRisk",
    "local_risk",
    "policyDecision",
    "policy_decision",
    "requiresConfirmation",
    "requires_confirmation",
    "mavlink",
    "shell",
    "script",
    "px4_parameters",
}

FLIGHT_TARGET_COMMANDS = {
    "vehicle.arm",
    "vehicle.takeoff",
    "vehicle.land",
    "vehicle.rtl",
    "vehicle.pause",
    "vehicle.goto",
    "mission.upload",
    "mission.start",
}

QA_PRIORITY_KEYWORDS = (
    "是什么意思",
    "什么原因",
    "为什么",
    "如何解释",
    "解释一下",
    "报警",
    "报错",
    "故障原因",
    "原因",
    "区别",
    "作用",
    "怎么办",
    "ekf2",
    "gps未定位",
    "未获得有效位置估计",
    "preflight fail",
    "no gps",
)

EXPLICIT_QUERY_KEYWORDS = ("查询", "查看", "读取", "获取", "显示", "list", "status", "query")
EXPLICIT_UI_KEYWORDS = ("选择", "打开")
EXPLICIT_FLIGHT_KEYWORDS = (
    "让",
    "请",
    "立即",
    "执行",
    "起飞",
    "降落",
    "返航",
    "悬停",
    "暂停",
    "前往",
    "飞到",
    "上传任务",
    "启动任务",
    "takeoff",
    "take off",
    "land",
    "rtl",
    "return to launch",
)
FORBIDDEN_COMMAND_KEYWORDS = ("写px4参数", "发送mavlink", "shell", "绕过", "忽略白名单")


@dataclass(frozen=True)
class NormalizedProposalResult:
    reply: str
    proposal: Proposal | None
    normalized: bool
    reason: str | None = None


def normalize_model_response(data: dict[str, Any], user_message: str | None = None) -> AgentResponseData:
    reply = data.get("reply")
    if not isinstance(reply, str) or not reply.strip():
        raise ValueError("Model output must include a non-empty reply string")

    raw_proposal = data.get("proposal")
    result = normalize_proposal(raw_proposal, reply.strip())
    if user_message and raw_proposal is None and result.proposal is None:
        recovered = recover_proposal_from_user_message(user_message)
        if recovered is not None:
            result = NormalizedProposalResult(
                reply=_append_recovery_note(reply.strip()),
                proposal=recovered,
                normalized=True,
                reason="normalizer_recovery",
            )
    if user_message and result.proposal is not None:
        intent_type = classify_user_intent(user_message)
        if _proposal_conflicts_with_intent(intent_type, result.proposal.command, user_message):
            result = _no_structured_proposal(
                _ensure_context_boundary(reply.strip()),
                f"用户意图为 {intent_type}，不是明确指令",
            )
    return AgentResponseData(reply=result.reply, proposal=result.proposal)


def classify_user_intent(message: str) -> str:
    normalized = message.strip().lower()
    if not normalized:
        return "answer_only"

    if _contains_any(normalized, FORBIDDEN_COMMAND_KEYWORDS):
        return "forbidden_command"

    if _is_explicit_log_tool_request(normalized, "log.explain_error"):
        return "log_tool"

    if _is_explicit_mission_request(normalized):
        return "mission_analysis"

    if _is_qa_or_log_explanation(normalized) and not _is_explicit_query(normalized):
        return "log_explanation" if _looks_like_log_or_fault(normalized) else "answer_only"

    if _is_explicit_ui_action(normalized):
        return "ui_action"

    if _is_explicit_flight_proposal(normalized):
        return "flight_proposal"

    if _is_explicit_query(normalized):
        return "status_query"

    return "answer_only"


def normalize_proposal(raw_proposal: Any, reply: str) -> NormalizedProposalResult:
    if raw_proposal is None:
        return NormalizedProposalResult(reply=reply, proposal=None, normalized=True)

    if not isinstance(raw_proposal, dict):
        return _no_structured_proposal(reply, "proposal 不是对象或 null")

    cleaned = _strip_dangerous_fields(raw_proposal)
    command = _normalize_command(cleaned.get("command"))
    if command is None:
        return _no_structured_proposal(reply, "无法将模型命令映射到标准 command")

    arguments_raw = cleaned.get("arguments")
    if arguments_raw is None:
        arguments_raw = {}
    if not isinstance(arguments_raw, dict):
        return _no_structured_proposal(reply, "proposal.arguments 不是对象")

    arguments = _normalize_arguments(command, arguments_raw)
    if arguments is None:
        return _no_structured_proposal(reply, "参数缺失或越界，无法形成结构化建议")

    summary = cleaned.get("summary")
    if not isinstance(summary, str) or not summary.strip():
        return _no_structured_proposal(reply, "proposal.summary 缺失")

    try:
        proposal = Proposal(command=command, arguments=arguments, summary=summary.strip())
    except Exception:
        return _no_structured_proposal(reply, "规范化后的 proposal 未通过 Agent schema")

    return NormalizedProposalResult(reply=reply, proposal=proposal, normalized=True)


def recover_proposal_from_user_message(message: str) -> Proposal | None:
    normalized = message.strip().lower()
    if not normalized:
        return None

    intent_type = classify_user_intent(normalized)
    if intent_type in {"answer_only", "log_explanation", "forbidden_command"}:
        return None

    recovered = _recover_log_tool_request(normalized)
    if recovered is not None:
        return recovered

    recovered = _recover_mission_request(normalized)
    if recovered is not None:
        return recovered

    recovered = _recover_status_query(normalized)
    if recovered is not None:
        return recovered

    recovered = _recover_ui_action(normalized)
    if recovered is not None:
        return recovered

    return _recover_flight_proposal(normalized)


def _recover_status_query(message: str) -> Proposal | None:
    if not _is_explicit_query(message):
        return None

    vehicle_id = _extract_vehicle_id(message)
    command: str | None = None
    summary: str | None = None
    arguments: dict[str, Any] = {}

    if "rtk" in message:
        command = "vehicle.query_rtk"
        summary = "查询 RTK 状态"
        if vehicle_id is not None:
            arguments["vehicle_id"] = vehicle_id
    elif "电量" in message:
        command = "vehicle.query_battery"
        summary = f"查询{_vehicle_label(vehicle_id)}电量"
    elif "位置" in message or "gps" in message or "定位" in message:
        command = "vehicle.query_position"
        summary = f"查询{_vehicle_label(vehicle_id)}位置"
    elif "状态" in message:
        command = "vehicle.query_status"
        summary = f"查询{_vehicle_label(vehicle_id)}状态"

    if command is None:
        return None
    if command != "vehicle.query_rtk" and vehicle_id is None and not _is_global_status_query(message, command):
        return None
    if vehicle_id is not None and "vehicle_id" in ALLOWED_ARGUMENTS[command]:
        arguments["vehicle_id"] = vehicle_id

    return Proposal(command=command, arguments=arguments, summary=summary or command)


def _recover_log_tool_request(message: str) -> Proposal | None:
    if not _is_explicit_log_tool_request(message, "log.explain_error"):
        return None
    vehicle_id = _extract_vehicle_id(message)
    arguments = {"vehicle_id": vehicle_id} if vehicle_id is not None else {}
    return Proposal(command="log.explain_error", arguments=arguments, summary="解释日志或错误信息")


def _recover_mission_request(message: str) -> Proposal | None:
    if not _is_explicit_mission_request(message):
        return None
    vehicle_id = _extract_vehicle_id(message)
    arguments = {"vehicle_id": vehicle_id} if vehicle_id is not None else {}
    if "草稿" in message:
        return Proposal(command="mission.create_draft", arguments=arguments, summary="生成任务草稿")
    return Proposal(command="mission.analyze", arguments=arguments, summary="分析任务或航线")


def _recover_ui_action(message: str) -> Proposal | None:
    if not _is_explicit_ui_action(message):
        return None
    if "选择" in message:
        vehicle_id = _extract_vehicle_id(message)
        if vehicle_id is None:
            return None
        return Proposal(
            command="ui.select_vehicle",
            arguments={"vehicle_id": vehicle_id},
            summary=f"选择{_vehicle_label(vehicle_id)}",
        )
    if "打开" in message:
        page = _extract_page_name(message)
        if page is None:
            return None
        return Proposal(command="ui.open_page", arguments={"page": page}, summary=f"打开{page}页面")
    return None


def _recover_flight_proposal(message: str) -> Proposal | None:
    if not _is_explicit_flight_proposal(message):
        return None

    vehicle_id = _extract_vehicle_id(message)
    if vehicle_id is None:
        return None

    if "起飞" in message or "takeoff" in message or "take off" in message:
        altitude_m = _extract_altitude_m(message)
        if altitude_m is None:
            return None
        return Proposal(
            command="vehicle.takeoff",
            arguments={"vehicle_id": vehicle_id, "altitude_m": altitude_m},
            summary=f"建议{_vehicle_label(vehicle_id)}起飞到{_format_number(altitude_m)}米",
        )
    if "返航" in message or "rtl" in message or "return to launch" in message:
        return Proposal(command="vehicle.rtl", arguments={"vehicle_id": vehicle_id}, summary=f"建议{_vehicle_label(vehicle_id)}返航")
    if "降落" in message or "land" in message:
        return Proposal(command="vehicle.land", arguments={"vehicle_id": vehicle_id}, summary=f"建议{_vehicle_label(vehicle_id)}降落")
    if "暂停" in message or "悬停" in message or "pause" in message or "hold" in message:
        return Proposal(command="vehicle.pause", arguments={"vehicle_id": vehicle_id}, summary=f"建议{_vehicle_label(vehicle_id)}暂停任务")
    return None


def _normalize_command(command: Any) -> str | None:
    if not isinstance(command, str):
        return None
    value = command.strip()
    if not value:
        return None
    if value in STANDARD_COMMANDS:
        return value
    key = value.lower().replace("_", ".").strip()
    if key in STANDARD_COMMANDS:
        return key
    return COMMAND_ALIASES.get(value) or COMMAND_ALIASES.get(value.lower())


def _normalize_arguments(command: str, raw_arguments: dict[str, Any]) -> dict[str, Any] | None:
    allowed = ALLOWED_ARGUMENTS.get(command)
    if allowed is None:
        return None

    normalized: dict[str, Any] = {}
    for key, value in raw_arguments.items():
        if key in DANGEROUS_FIELDS:
            continue
        canonical_key = ARGUMENT_ALIASES.get(key, key)
        if canonical_key not in allowed:
            continue
        coerced = _coerce_argument(canonical_key, value)
        if coerced is _INVALID:
            return None
        normalized[canonical_key] = coerced

    if command in FLIGHT_TARGET_COMMANDS and "vehicle_id" not in normalized:
        normalized["vehicle_id"] = None

    if command == "ui.select_vehicle" and not isinstance(normalized.get("vehicle_id"), int):
        return None

    if command == "ui.open_page" and not normalized.get("page"):
        return None

    if command in {"map.focus_coordinate", "vehicle.goto"}:
        if not _valid_coordinate(normalized.get("latitude"), normalized.get("longitude")):
            return None
        if command == "vehicle.goto" and not _valid_altitude(normalized.get("altitude_m"), allow_zero=True):
            return None

    if command == "vehicle.takeoff" and "altitude_m" in normalized:
        if not _valid_altitude(normalized.get("altitude_m"), allow_zero=False):
            return None

    if command == "mavlink.send_raw":
        return {}

    return normalized


_INVALID = object()


def _coerce_argument(key: str, value: Any) -> Any:
    if key == "vehicle_id":
        if value is None:
            return None
        if isinstance(value, bool):
            return _INVALID
        if isinstance(value, int):
            return value if value > 0 else _INVALID
        if isinstance(value, str):
            stripped = value.strip()
            if stripped.isdigit():
                parsed = int(stripped)
                return parsed if parsed > 0 else _INVALID
        return _INVALID

    if key in {"altitude_m", "latitude", "longitude"}:
        if isinstance(value, bool):
            return _INVALID
        if isinstance(value, (int, float)):
            return float(value)
        if isinstance(value, str):
            try:
                return float(value.strip())
            except ValueError:
                return _INVALID
        return _INVALID

    if key in {"page", "name"}:
        if not isinstance(value, str):
            return _INVALID
        stripped = value.strip()
        return stripped if stripped else _INVALID

    return value


def _valid_coordinate(latitude: Any, longitude: Any) -> bool:
    return (
        isinstance(latitude, (int, float))
        and isinstance(longitude, (int, float))
        and -90.0 <= float(latitude) <= 90.0
        and -180.0 <= float(longitude) <= 180.0
    )


def _valid_altitude(value: Any, allow_zero: bool) -> bool:
    if not isinstance(value, (int, float)):
        return False
    minimum = 0.0 if allow_zero else 0.0
    if allow_zero:
        return minimum <= float(value) <= 120.0
    return minimum < float(value) <= 120.0


def _strip_dangerous_fields(value: Any) -> Any:
    if isinstance(value, dict):
        return {
            key: _strip_dangerous_fields(child)
            for key, child in value.items()
            if key not in DANGEROUS_FIELDS
        }
    if isinstance(value, list):
        return [_strip_dangerous_fields(child) for child in value]
    return value


def _no_structured_proposal(reply: str, reason: str) -> NormalizedProposalResult:
    suffix = f"\n\n无法形成结构化建议：{reason}。"
    return NormalizedProposalResult(reply=reply + suffix, proposal=None, normalized=False, reason=reason)


def _proposal_conflicts_with_intent(intent_type: str, command: str, user_message: str) -> bool:
    if intent_type in {"answer_only", "log_explanation"}:
        return not _is_explicit_log_tool_request(user_message, command)
    if intent_type == "status_query":
        return command not in {
            "vehicle.query_status",
            "vehicle.query_battery",
            "vehicle.query_position",
            "vehicle.query_rtk",
        }
    if intent_type == "ui_action":
        return command not in {"ui.select_vehicle", "ui.open_page", "map.focus_coordinate"}
    if intent_type == "flight_proposal":
        return command not in FLIGHT_TARGET_COMMANDS
    if intent_type in {"log_tool", "mission_analysis"}:
        return command not in {"log.explain_error", "mission.create_draft", "mission.analyze"}
    if intent_type == "forbidden_command":
        return not _is_explicit_forbidden_tool_request(user_message, command)
    return False


def _is_qa_or_log_explanation(message: str) -> bool:
    return _contains_any(message, QA_PRIORITY_KEYWORDS)


def _looks_like_log_or_fault(message: str) -> bool:
    return _contains_any(
        message,
        ("报警", "报错", "故障", "ekf2", "gps", "rtk", "preflight fail", "no gps", "日志", "log", "error", "fail"),
    )


def _is_explicit_query(message: str) -> bool:
    if "rtk" in message and "状态" in message:
        return True
    if "是否" in message and "连接" in message:
        return True
    if not _contains_any(message, EXPLICIT_QUERY_KEYWORDS):
        return False
    return _contains_any(
        message,
        ("状态", "电量", "位置", "gps", "rtk", "定位", "一号机", "二号机", "三号机", "无人机", "uav", "vehicle"),
    )


def _is_explicit_ui_action(message: str) -> bool:
    return _contains_any(message, EXPLICIT_UI_KEYWORDS) and _contains_any(message, ("页面", "地图", "参数", "一号机", "二号机", "三号机"))


def _is_explicit_flight_proposal(message: str) -> bool:
    if _is_qa_or_log_explanation(message) and not message.startswith(("让", "请", "立即", "执行")):
        return False
    return _contains_any(message, EXPLICIT_FLIGHT_KEYWORDS)


def _is_explicit_log_tool_request(message: str, command: str) -> bool:
    return command == "log.explain_error" and _contains_any(message.lower(), ("解释日志", "分析日志", "log"))


def _is_explicit_mission_request(message: str) -> bool:
    return (
        ("任务草稿" in message and _contains_any(message, ("生成", "创建")))
        or ("航线" in message and _contains_any(message, ("分析", "判断")))
        or ("航点" in message and _contains_any(message, ("分析", "判断")))
    )


def _is_global_status_query(message: str, command: str) -> bool:
    return command == "vehicle.query_status" and _contains_any(message, ("所有", "全部", "当前是否连接", "是否连接"))


def _is_explicit_forbidden_tool_request(message: str, command: str) -> bool:
    normalized = message.lower()
    if command == "mavlink.send_raw":
        return "mavlink" in normalized and not _contains_any(normalized, ("绕过", "忽略白名单", "shell"))
    if command == "param.write":
        return "参数" in normalized or "param" in normalized
    return False


def _ensure_context_boundary(reply: str) -> str:
    if any(marker in reply for marker in ("没有真实遥测", "缺少真实遥测", "请求上下文", "只能给出常见原因")):
        return reply
    return reply + "\n\n当前请求没有提供真实遥测、日志或传感器数据，因此只能给出常见原因和排查方向。"


def _append_recovery_note(reply: str) -> str:
    if "已按明确指令补全结构化建议" in reply:
        return reply
    return reply + "\n\n已按明确指令补全结构化建议；该建议仍需由 QGC 本地策略校验，不代表已经执行。"


def _extract_vehicle_id(message: str) -> int | None:
    english_match = re.search(r"(?:uav|vehicle)[\s#-]*([1-9]\d*)", message)
    if english_match:
        return int(english_match.group(1))

    digit_match = re.search(r"(?<!\d)([1-9]\d*)\s*(?:号机|号无人机|号|#|uav|vehicle)", message)
    if digit_match:
        return int(digit_match.group(1))

    chinese_numbers = {
        "一": 1,
        "二": 2,
        "两": 2,
        "三": 3,
        "四": 4,
        "五": 5,
        "六": 6,
        "七": 7,
        "八": 8,
        "九": 9,
        "十": 10,
    }
    for token, value in chinese_numbers.items():
        if any(pattern in message for pattern in (f"{token}号机", f"{token}号无人机", f"{token}号")):
            return value
    return None


def _extract_altitude_m(message: str) -> float | None:
    match = re.search(r"(?:起飞到|高度到|到|至|爬升到)\s*([1-9]\d*(?:\.\d+)?)\s*(?:米|m)", message)
    if not match:
        match = re.search(r"([1-9]\d*(?:\.\d+)?)\s*(?:米|m)", message)
    if not match:
        return None
    value = float(match.group(1))
    return value if _valid_altitude(value, allow_zero=False) else None


def _extract_page_name(message: str) -> str | None:
    page_aliases = {
        "地图": "map",
        "参数": "parameters",
        "ai": "ai",
        "日志": "logs",
    }
    for token, page in page_aliases.items():
        if token in message:
            return page
    return None


def _vehicle_label(vehicle_id: int | None) -> str:
    return f"{vehicle_id}号无人机" if vehicle_id is not None else "无人机"


def _format_number(value: float) -> str:
    return str(int(value)) if float(value).is_integer() else str(value)


def _contains_any(message: str, keywords: tuple[str, ...]) -> bool:
    return any(keyword in message for keyword in keywords)
