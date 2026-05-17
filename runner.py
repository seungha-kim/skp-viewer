from dataclasses import dataclass
import os.path
import subprocess
import sys
import shutil
import sysconfig
from typing import Union, Optional

from studio.startup_options import StartupOptions

project_root = os.path.abspath(os.path.dirname(__file__))
release_build_dir = os.path.join(project_root, "build")
release_bindings_dir = os.path.join(release_build_dir, "bindings")
if sys.platform == "win32":
    release_bindings_dir_msvc = os.path.join(release_bindings_dir, "Release")
    if os.path.exists(release_bindings_dir_msvc):
        release_bindings_dir = release_bindings_dir_msvc

sys.path.append(release_bindings_dir)
if sys.platform == 'win32':
    try:
        os.add_dll_directory(os.path.join(project_root, 'extern', 'sketchup', 'windows', 'binaries'))
    except:
        pass
if sys.platform == "win32" and hasattr(os, "add_dll_directory"):
    os.add_dll_directory(release_bindings_dir)
    try:
        os.add_dll_directory(r"C:\msys64\mingw64\bin")
    except:
        pass
os.chdir(project_root)


class RunnerCommand:
    @dataclass
    class Config:
        pass

    @dataclass
    class RunStudio:
        opts: StartupOptions

    AnyCommand = Union[Config, RunStudio]

    @classmethod
    def parse(cls) -> AnyCommand:
        def get_item(lst: list[str], index: int) -> Optional[str]:
            return lst[index] if len(lst) > index else None

        command = get_item(sys.argv, 1)
        if command == "config":
            return cls.Config()
        elif command == "open":
            path = get_item(sys.argv, 2)
            if path:
                return cls.RunStudio(StartupOptions(model_path=path))
            else:
                raise RuntimeError("path needed")
        else:
            return cls.RunStudio(StartupOptions())


def get_cmake_path():
    # First try PATH
    cmake_path = shutil.which("cmake")
    if cmake_path:
        return cmake_path

    exe_name = "cmake.exe" if sys.platform == "win32" else "cmake"

    # Try the active Python environment's scripts directory.
    script_dirs = [
        sysconfig.get_path("scripts"),
        os.path.dirname(sys.executable),
    ]
    for script_dir in script_dirs:
        if not script_dir:
            continue
        cmake_path = os.path.join(script_dir, exe_name)
        if os.path.exists(cmake_path):
            return cmake_path
    
    # Try typical pip user install location on Windows
    if sys.platform == "win32":
        user_script_dir = os.path.join(os.environ.get("APPDATA", ""), "Python", f"Python{sys.version_info.major}{sys.version_info.minor}", "Scripts")
        cmake_path = os.path.join(user_script_dir, exe_name)
        if os.path.exists(cmake_path):
            return cmake_path
            
    # Try importing cmake package (if installed via pip)
    try:
        import cmake

        cmake_bin_dir = getattr(cmake, "CMAKE_BIN_DIR", None)
        if cmake_bin_dir:
            cmake_path = os.path.join(cmake_bin_dir, exe_name)
            if os.path.exists(cmake_path):
                return cmake_path

        package_dir = os.path.dirname(cmake.__file__)
        candidate_paths = [
            os.path.join(package_dir, "data", "bin", exe_name),
            os.path.join(package_dir, "bin", exe_name),
        ]
        for cmake_path in candidate_paths:
            if os.path.exists(cmake_path):
                return cmake_path
    except ImportError:
        pass
        
    return "cmake"  # fallback

def config():
    os.makedirs(release_build_dir, exist_ok=True)
    cmd = [get_cmake_path(), "-B", release_build_dir, "-D", "CMAKE_BUILD_TYPE=Release", "-D", "CMAKE_POLICY_VERSION_MINIMUM=3.5", "-D", f"PYTHON_EXECUTABLE={sys.executable}"]
    if sys.platform == "win32":
        cmd.extend(["-G", "Ninja"])
        # Find ninja installed via pip
        ninja_path = shutil.which("ninja")
        if not ninja_path:
            try:
                import ninja as ninja_mod
                ninja_path = os.path.join(ninja_mod.BIN_DIR, "ninja.exe")
            except ImportError:
                pass
        if ninja_path and os.path.exists(ninja_path):
            cmd.extend(["-D", f"CMAKE_MAKE_PROGRAM={ninja_path}"])
    subprocess.run(cmd, check=True)


def build_binding():
    if not os.path.isfile(os.path.join(release_build_dir, "CMakeCache.txt")):
        config()
    cmd = [
        get_cmake_path(),
        "--build",
        release_build_dir,
        "--config",
        "Release",
        "--target",
        "binding_test",
    ]
    subprocess.run(cmd, check=True)


def run_app(command):
    from studio.app import App

    App(command.opts).run()


def main():
    command = RunnerCommand.parse()
    if isinstance(command, RunnerCommand.Config):
        config()
    elif isinstance(command, RunnerCommand.RunStudio):
        build_binding()
        run_app(command)


if __name__ == "__main__":
    main()
