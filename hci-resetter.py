#!/usr/bin/env python3

import glob
import os
import re
import subprocess
import sys
import time


regexes = [
    re.compile(x) for x in [
        "Bluetooth: (hci[0-9]*): hardware error",
        "Bluetooth: (hci[0-9]*): unexpected event",
        "Bluetooth: (hci[0-9]*): start background scanning failed",
        "Bluetooth: (hci[0-9]*): CSR:.*failed",
        "Bluetooth: (hci[0-9]*): command.*timeout",
        "Bluetooth: (hci[0-9]*): unexpected event.*length",
    ]
]


def get_device_and_driver_path(hci):
    base_folder = f"/sys/class/bluetooth/{hci}"
    device_folder = os.path.join(base_folder, "device")
    driver_folder = os.path.join(device_folder, "driver")
    device_path = os.path.realpath(device_folder)
    driver_path = os.path.realpath(driver_folder)
    device = os.path.basename(device_path)
    return device, driver_path


def shut_off_and_on(device, driver_path):
    unbind, bind = (
        os.path.join(driver_path, "unbind"),
        os.path.join(driver_path, "bind"),
    )
    with open(unbind, "w") as f:
        f.write(device + "\n")
        
    time.sleep(0.5)
    
    with open(bind, "w") as f:
        f.write(device + "\n")
        
    time.sleep(0.5)


def start_monitor():
    return subprocess.Popen(["dmesg", "-w"],
        universal_newlines=True,
        stdout=subprocess.PIPE
    )


def reset_hci(hci):
    device, driver_path = get_device_and_driver_path(hci)
    print(f"Resetting HCI {hci}", file=sys.stderr)
    shut_off_and_on(device, driver_path)
    print(f"HCI {hci} has been reset", file=sys.stderr)


if sys.argv[1:] and sys.argv[1] == "--now":
    retval = 0
    for hci in glob.glob("/sys/class/bluetooth/hci*"):
        if not os.path.isdir(hci):
            continue
        try:
            reset_hci(os.path.basename(hci))
        except Exception as e:
            print("Error resetting HCI: %s" % e)
            retval = 1
    sys.exit(retval)


p = start_monitor()
while True:
    line = p.stdout.readline().rstrip()
    if not line:
        break
    else:
        hci = None
        for r in regexes:
            match = r.search(line)
            if match is not None:
                hci = match.group(1)
        if hci is not None:
            print(f"HCI {hci} has failed", file=sys.stderr)
            reset_hci(hci)
            p.stdout.close()
            p.kill()
            p.wait()
            p = start_monitor()

p.stdout.close()
sys.exit(p.wait())
