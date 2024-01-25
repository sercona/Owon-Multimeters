#!/usr/bin/env python3

"""
take raw semi-json lines and make a proper json-parsable file.

from this, you can run it thru json.tool:

$ ./wrap_json_lines.py -f ./ow18e-ref-data-ohms.txt | python -mjson.tool

(c) 2024 linux-works
"""

import os
import sys
import argparse



def do_json_wrap(out_file):

    lines_list = None
    first_comma = True
    with open(out_file, "r") as lines:
        lines_list = lines.readlines()
        
        for line in lines_list:
            txt = line.strip()
            if first_comma:
                first_comma = False
                sys.stdout.write(f' {txt}')
            else:
                sys.stdout.write(f',\n {txt}')
                
    return



if __name__ == "__main__":

    # get all CLI args
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', required=True, type=str, help='filename to read from')
    args = parser.parse_args()

    in_file = None
    if args.f is not None:
        in_file = args.f
    else:
        print("must include -f filename")
        sys.exit(0)

    print("[");
    do_json_wrap(in_file)  # our only argument
    print("\n]\n");
