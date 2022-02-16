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

if args.is_include:
    base = os.path.dirname(os.path.dirname(result))
    result = os.path.join(base, "include", version_name)
elif args.is_lib:
    base = os.path.dirname(os.path.dirname(result))
    result = os.path.join(base, "lib")
elif args.is_name:
    result = version_name

print(result, end="")
