from __future__ import annotations

import uvicorn

from app.config import AgentSettings
from app.main import create_app


def main() -> None:
    settings = AgentSettings.from_env()
    uvicorn.run(
        create_app(settings),
        host=settings.host,
        port=settings.port,
        log_level=settings.log_level.lower(),
    )


if __name__ == "__main__":
    main()
