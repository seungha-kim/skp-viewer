def init() -> None: ...


class SurfaceInfo:
    def __init__(self, logical_width: int, logical_height: int, physical_width: int, physical_height: int, content_scale_x: float, content_scale_y: float): ...


class Engine:
    def __init__(self, surface_info: SurfaceInfo): ...
    def render(self, playback_value: float) -> None: ...
    def resize(self, surface_info: SurfaceInfo) -> None: ...
    def setRandomGlobalDiffuse(self) -> None: ...
    def currentCameraStateMut(self) -> CameraState: ...


class Vec3:
    def __init__(self) -> None: ...
    def __mul__(self, other: float) -> Vec3: ...
    def __rmul__(self, other: float) -> Vec3: ...
    def __add__(self, other: Vec3) -> Vec3: ...
    def __sub__(self, other: Vec3) -> Vec3: ...
    x: float
    y: float
    z: float


class CameraState:
    def __init__(self) -> None: ...
    def front(self) -> Vec3: ...
    def left(self) -> Vec3: ...
    fovyDef: float
    aspectWidth: float
    aspectHeight: float
    zNear: float
    zFar: float
    pos: Vec3
    up: Vec3
    speed: float
    pitch: float
    yaw: float
