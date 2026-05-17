import sys
import os.path
import argparse

parser = argparse.ArgumentParser(description="find python paths")
parser.add_argument(
    "--include", dest="is_include", action="store_const", const=True, default=False
)
parser.add_argument(
    "--lib", dest="is_lib", action="store_const", const=True, default=False
)
parser.add_argument(
    "--name", dest="is_name", action="store_const", const=True, default=False
)

args = parser.parse_args()

result = sys.executable
version_name = f"python{sys.version_info.major}.{sys.version_info.minor}"

if sys.platform == "win32":
    # Windows Python layout:
    #   Include: <prefix>/include
    #   Libs:    <prefix>/libs
    #   Name:    python<major><minor>  (e.g. python313)
    base = os.path.dirname(result)  # Python base dir on Windows
    if args.is_include:
        result = os.path.join(base, "include")
    elif args.is_lib:
        result = os.path.join(base, "libs")
    elif args.is_name:
        result = f"python{sys.version_info.major}{sys.version_info.minor}"
else:
    # Unix/macOS Python layout:
    #   Include: <prefix>/include/python<major>.<minor>
    #   Libs:    <prefix>/lib
    #   Name:    python<major>.<minor>
    base = os.path.dirname(os.path.dirname(result))
    if args.is_include:
        result = os.path.join(base, "include", version_name)
    elif args.is_lib:
        result = os.path.join(base, "lib")
    elif args.is_name:
        result = version_name

print(result, end="")
