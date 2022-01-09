from dataclasses import dataclass
from typing import Optional


@dataclass
class StartupOptions:
    model_path: Optional[str] = None
