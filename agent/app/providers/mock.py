from __future__ import annotations

import re

from app.config import DEFAULT_MODEL
from app.providers.base import AgentProvider, ProviderHealth
from app.schemas import AgentRequest, AgentResponseData, Proposal


STATUS_KEYWORDS = ("状态", "无人机状态", "连接状态", "电量", "gps", "rtk", "battery", "status", "connected")
LOG_KEYWORDS = ("日志", "报错", "错误", "故障", "log", "error", "fault")
MISSION_KEYWORDS = ("航线", "航点", "地形", "建筑", "禁飞区", "mission", "waypoint", "terrain", "building", "no-fly")
HIGH_RISK_COMMANDS = (
    ("vehicle.takeoff", ("起飞", "takeoff", "take off")),
    ("vehicle.arm", ("解锁", "arm")),
    ("vehicle.land", ("降落", "land")),
    ("vehicle.rtl", ("返航", "rtl", "return to launch")),
    ("vehicle.pause", ("暂停", "pause")),
    ("vehicle.hold", ("悬停", "hold", "hover")),
    ("vehicle.change_mode", ("改变模式", "切换模式", "change mode")),
    ("mission.upload", ("上传任务", "upload mission")),
    ("mission.start", ("启动任务", "start mission")),
)

COMMAND_LABELS = {
    "vehicle.takeoff": "起飞",
    "vehicle.arm": "解锁",
    "vehicle.land": "降落",
    "vehicle.rtl": "返航",
    "vehicle.pause": "暂停",
    "vehicle.hold": "悬停",
    "vehicle.change_mode": "改变模式",
    "mission.upload": "上传任务",
    "mission.start": "启动任务",
}


class MockProvider(AgentProvider):
    name = "mock"
    model = DEFAULT_MODEL

    def health(self) -> ProviderHealth:
        return ProviderHealth(ready=True, available_models=[self.model])

    def generate(self, request: AgentRequest) -> AgentResponseData:
        message = request.message.lower()
        high_risk_command = self._detect_high_risk_command(message)

        if high_risk_command:
            return self._flight_proposal_response(request, high_risk_command)

        if self._contains_any(message, STATUS_KEYWORDS):
            return AgentResponseData(reply=self._status_reply(request))

        if self._contains_any(message, LOG_KEYWORDS):
            return AgentResponseData(reply="当前Mock Agent可以验证日志分析接口，但尚未接入实际日志工具。")

        if self._contains_any(message, MISSION_KEYWORDS):
            return AgentResponseData(reply="当前Mock Agent可以生成分析流程，但尚未连接GIS Safety Service，不能把结果作为真实安全结论。")

        return AgentResponseData(reply="当前为本机Mock Agent演示回答。服务未连接外部网络、真实模型、MAVLink或飞行执行入口。")

    def _flight_proposal_response(self, request: AgentRequest, command: str) -> AgentResponseData:
        if command not in request.allowed_capabilities:
            return AgentResponseData(
                reply=f"已识别到 {command} 请求，但该能力未被QGC授权。本机Agent不会生成可执行参数。",
                proposal=None,
            )

        vehicle_id = self._extract_explicit_vehicle_id(request.message)
        action_label = COMMAND_LABELS.get(command, command)
        reply = "已识别为飞行动作请求，但本机Agent无飞行执行权限。该内容仅为结构化建议。"
        if vehicle_id is None:
            reply += " 当前请求未明确目标无人机，需要由QGC或用户明确目标后再评估。"

        return AgentResponseData(
            reply=reply,
            proposal=Proposal(
                command=command,
                arguments={"vehicle_id": vehicle_id},
                summary=self._proposal_summary(action_label, vehicle_id),
            ),
        )

    @staticmethod
    def _contains_any(message: str, keywords: tuple[str, ...]) -> bool:
        return any(keyword in message for keyword in keywords)

    @staticmethod
    def _detect_high_risk_command(message: str) -> str | None:
        for command, keywords in HIGH_RISK_COMMANDS:
            if any(keyword in message for keyword in keywords):
                return command
        return None

    @staticmethod
    def _extract_explicit_vehicle_id(message: str) -> int | None:
        patterns = (
            r"(\d+)\s*号",
            r"vehicle\s*#?\s*(\d+)",
            r"vehicle_id\s*[:=]\s*(\d+)",
            r"id\s*[:=]\s*(\d+)",
        )
        for pattern in patterns:
            match = re.search(pattern, message, flags=re.IGNORECASE)
            if match:
                return int(match.group(1))
        chinese_digits = {
            "一": 1,
            "二": 2,
            "三": 3,
            "四": 4,
            "五": 5,
            "六": 6,
            "七": 7,
            "八": 8,
            "九": 9,
        }
        match = re.search(r"([一二三四五六七八九])号", message)
        if match:
            return chinese_digits[match.group(1)]
        return None

    @staticmethod
    def _proposal_summary(action_label: str, vehicle_id: int | None) -> str:
        if vehicle_id is None:
            return f"建议目标无人机明确后再评估{action_label}操作"
        return f"建议{vehicle_id}号无人机执行{action_label}操作"

    @staticmethod
    def _status_reply(request: AgentRequest) -> str:
        context = request.context
        parts = [
            "当前为本机Mock Agent。尚未从QGC获得可验证的实时飞行器执行能力，回答仅基于请求中的状态快照。"
        ]
        if context.connected is not None:
            parts.append(f"connected={context.connected}")
        if context.armed is not None:
            parts.append(f"armed={context.armed}")
        if context.battery_percent is not None:
            parts.append(f"battery_percent={context.battery_percent}")
        if context.flight_mode is not None:
            parts.append(f"flight_mode={context.flight_mode}")
        return " ".join(parts)
