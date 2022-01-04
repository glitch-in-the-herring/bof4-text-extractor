import argparse
import os
import sys

argparser = argparse.ArgumentParser(
    description="Mass deduplicator"
)
argparser.add_argument(
    "source", metavar="SOURCE", type=str, help="Source folder"
)
argparser.add_argument(
    "target", metavar="TARGET", type=str, help="Output folder"
)

args = argparser.parse_args()


def main():
    if not os.path.exists(args.source):
        print("Folder not found!")
        sys.exit(1)

    if not os.path.exists(args.target):
        os.mkdir(args.target)     

    files = os.listdir(path=args.source)
    dialogues = []

    for i in files:
        with open("/".join([args.source, i]), "r") as f:
            c = f.read()

            if len(c) == 0:
                continue

            if c not in dialogues:
                dialogues.append(c)
                with open("/".join([args.target, i]), "w") as ff:
                    ff.write(c)

if __name__ == "__main__":
    main()