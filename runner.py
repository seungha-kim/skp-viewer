import os.path
import subprocess
import sys


project_root = os.path.abspath(os.path.dirname(__file__))
release_build_dir = os.path.join(project_root, "cmake-build-release")
release_bindings_dir = os.path.join(release_build_dir, "bindings")

sys.path.append(release_bindings_dir)
os.chdir(project_root)


def config():
    subprocess.run(["git", "submodule", "update", "--init", "--recursive"])
    # TODO: sketchup sdk binary
    os.makedirs(release_build_dir, exist_ok=True)
    subprocess.run(["cmake", "-B", release_build_dir, "-D", "CMAKE_BUILD_TYPE=Release"])


def build_binding():
    if not os.path.isdir(release_build_dir):
        config()
    subprocess.run(["cmake", "--build", release_build_dir, "--target", "binding_test"])


def run_studio():
    import studio.main
    studio.main.main()


def main():
    command = sys.argv[1] if len(sys.argv) > 1 else None
    if command == "config":
        config()
    else:
        build_binding()
        run_studio()


if __name__ == '__main__':
    main()
