import json
from app.providers.system_prompt import MERIVUS_SYSTEM_PROMPT


def _fewshot_section() -> str:
    start = MERIVUS_SYSTEM_PROMPT.index("Few-shot 示例")
    end = MERIVUS_SYSTEM_PROMPT.index("参数名必须只使用标准字段")
    return MERIVUS_SYSTEM_PROMPT[start:end]


def test_fewshot_examples_do_not_grant_local_policy_fields():
    section = _fewshot_section()

    assert "executable" not in section
    assert "risk" not in section
    assert "policyDecision" not in section
    assert "requiresConfirmation" not in section


def test_fewshot_examples_are_plain_json_objects():
    section = _fewshot_section()
    outputs = [line for line in section.splitlines() if line.startswith("{") and line.endswith("}")]

    assert len(outputs) == 5
    for output in outputs:
        parsed = json.loads(output)
        assert set(parsed) == {"reply", "proposal"}
        if parsed["proposal"] is not None:
            assert set(parsed["proposal"]) == {"command", "arguments", "summary"}


def test_fewshot_examples_cover_required_boundaries():
    section = _fewshot_section()

    assert '"proposal":null' in section
    assert '"command":"vehicle.query_status"' in section
    assert '"command":"vehicle.query_position"' in section
    assert '"command":"vehicle.takeoff"' in section
    assert '"command":"mavlink.send_raw"' in section
