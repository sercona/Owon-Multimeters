#!/usr/bin/env python3

"""
line by line difference viewer tool

for each line in the file, compare the whole line (up to \n)
with the previous whole line and show any chars that are different.

this assumes the lines are the same length and only chars in stable locations
are going to be changing.

sticky-mode latches each column.

(c) 2023 linux-works
"""

import os
import sys
import argparse


sticky = False   # if true, then latch each change


def do_line_diffs(out_file, sticky):

    # init our changed_value array to all False
    changed = {}
    for i in range(132):
        changed[i] = False

    lines_list = None
    with open(out_file, "r") as lines:
        lines_list = lines.readlines()
        
    # debug
    if False:
        for line in lines_list:
            txt = line.strip()
            print(txt)

            
    # line by line compare
    line_count = 0
    last_line = ""
    
    for line in lines_list:
        if last_line == "":
            last_line = line.strip()
            continue
        
        this_line = line.strip()
        
        if last_line != "":

            sys.stdout.write("[{:3d}] ".format(line_count))
            
            # char by char diff on the prev and cur lines
            if True:  #else:
                for i in range(len(last_line)):
                    if last_line[i] != this_line[i]:

                        # set a flag that 'this column had a value-change'
                        changed[i] = True
                        
                        # print inverse
                        sys.stdout.write("\033[7m")  # inverse-video
                        sys.stdout.write(this_line[i])
                        sys.stdout.write("\033[0m")  # normal video
                    else:
                        # before we print in normal mode, check if this column had its changed_flag set
                        if sticky and changed[i]:
                            sys.stdout.write("\033[7m")  # inverse-video
                            sys.stdout.write(this_line[i])
                            sys.stdout.write("\033[0m")  # normal video
                        else:
                            # print normal
                            sys.stdout.write(this_line[i])
                                
                sys.stdout.write("\n")  # ending newline
                
        
        # copy this to the 'last' buffer
        last_line = this_line

        line_count += 1


if __name__ == "__main__":

    # get all CLI args
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', required=True, type=str, help='filename to read from')
    parser.add_argument('--sticky', action='store_true', required=False,  help='Latch each changed column')
    args = parser.parse_args()

    if args.f is not None:
        out_file = args.f
    else:
        print("must include -f filename")
        sys.exit(0)

    if args.sticky is not None:
        sticky = args.sticky
    else:
        sticky = False
        
    do_line_diffs(out_file, sticky)  # our only argument
