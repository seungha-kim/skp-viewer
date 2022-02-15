from dataclasses import dataclass
import os.path
import subprocess
import sys
from typing import Union, Optional

from studio.startup_options import StartupOptions

project_root = os.path.abspath(os.path.dirname(__file__))
release_build_dir = os.path.join(project_root, "build")
release_bindings_dir = os.path.join(release_build_dir, "bindings")
if sys.platform == "win32":
    release_bindings_dir = os.path.join(release_bindings_dir, "Release")

sys.path.append(release_bindings_dir)
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


def config():
    os.makedirs(release_build_dir, exist_ok=True)
    subprocess.run(["cmake", "-B", release_build_dir, "-D", "CMAKE_BUILD_TYPE=Release"], check=True)


def build_binding():
    if not os.path.isdir(release_build_dir):
        config()
    cmd = ["cmake", "--build", release_build_dir, "--config", "Release", "--target", "binding_test"]
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


if __name__ == '__main__':
    main()
