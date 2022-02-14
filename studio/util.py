from binding_test import *


def create_reader_by_file_format(path: str) -> AbstractReader:
    if path.endswith(".skp"):
        return SketchupReader(path)
    elif path.endswith(".obj"):
        return AssimpReader(path)
    if not path:
        raise RuntimeError("Path must be given")
    else:
        raise RuntimeError("Unsupported file format")


def clamp(value: float, min_value: float, max_value: float) -> float:
    return max(min_value, min(value, max_value))
