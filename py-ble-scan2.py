#!/usr/bin/env python3

import asyncio
from bleak import BleakScanner


dmm_names = [ "BDM", "Bluetooth DMM" ]

async def run():
    devices = await BleakScanner.discover(timeout=5, return_adv=False)
    for d in devices:
        if d.name.strip() in dmm_names:
            print(d.address, d.name)
            
loop = asyncio.get_event_loop()
loop.run_until_complete(run())
