from app import __main__ as main_module


def test_python_module_entrypoint_uses_settings(monkeypatch):
    captured = {}

    def fake_run(app, **kwargs):
        captured["app"] = app
        captured["kwargs"] = kwargs

    monkeypatch.setenv("MERIVUS_AGENT_HOST", "0.0.0.0")
    monkeypatch.setenv("MERIVUS_AGENT_PORT", "9876")
    monkeypatch.setenv("MERIVUS_AGENT_LOG_LEVEL", "DEBUG")
    monkeypatch.setattr(main_module.uvicorn, "run", fake_run)

    main_module.main()

    assert captured["kwargs"] == {
        "host": "127.0.0.1",
        "port": 9876,
        "log_level": "debug",
    }
    assert captured["app"].title == "MERIVUS Local Agent"
