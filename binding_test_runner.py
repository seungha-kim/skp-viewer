import argparse
import os.path
import sys


parser = argparse.ArgumentParser(description='Binding test runner')
parser.add_argument('location', metavar='LOCATION', type=str,
                    help='build dir of dll/so (e.g. cmake_build_release)')
args = parser.parse_args()

abs_build_path = args.location

if not os.path.isabs(args.location):
    abs_build_path = os.path.join(os.path.dirname(__file__), abs_build_path)

sys.path.append(abs_build_path)


def main():
    import binding_test_qt
    binding_test_qt.main()


if __name__ == '__main__':
    main()
