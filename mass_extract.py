import argparse
import os
import subprocess
import sys

argparser = argparse.ArgumentParser(
    description="Mass extractor for Breath of Fire IV dialogue sections"
)
argparser.add_argument(
    "source", metavar="SOURCE", type=str, help="Source folder containing the .EMI files"
)
argparser.add_argument(
    "target",
    metavar="TARGET",
    type=str,
    help="Output folder containing the extracted dialogues",
)
argparser.add_argument("-j", "--jp", "--japanese", action="store_true")
argparser.add_argument("-v", "--verbose", action="store_true")
args = argparser.parse_args()


def main():
    files = os.listdir(path=args.source)

    if not os.path.exists(args.target):
        os.mkdir(args.target)

    extractor = "jpextractor.exe" if args.jp else "extractor.exe"

    for i in files:
        if args.verbose:
            print("Extracting", i)
        subprocess.run(
            [
                extractor,
                "/".join([args.source, i]),
                "".join([args.target, "/", i, "_output.txt"]),
            ]
        )


if __name__ == "__main__":
    main()
