#! /usr/bin/env python3
# ECE 631 Spring 2021
# Author: Aidan Harries
# Demo of NFC UID get.
#
"""
This example shows connecting to the PN532 with I2C (requires clock
stretching support), SPI, or UART. SPI is best, it uses the most pins but
is the most reliable and universally supported.
After initialization, try waving various 13.56MHz RFID cards over it!
"""

import time
import datetime
import binascii
import RPi.GPIO as GPIO
import pn532
import json
import MQTTClients
import subprocess

if __name__ == '__main__':
    IsConnected = False
    while(not IsConnected):
        try:
            MQTTPub = MQTTClients.MQTTPusher('144.126.217.239', 1883)
            IsConnected = True
        except:
            time.sleep(2)

    try:
        PN532 = pn532.PN532_SPI(debug=False, reset=22, cs=8)
        ic, ver, rev, support = PN532.get_firmware_version()
        print('Found PN532 with firmware version: {0}.{1}'.format(ver, rev))

        # Configure PN532 to communicate with MiFare cards
        PN532.SAM_configuration()
        print('Waiting for RFID/NFC card...')

        last_time_sent = 0  # To rate-limit UID messages

        while True:  # Main Loop
            try:
                # Check if a card is available to read with 100ms timeout
                uid = PN532.read_passive_target(timeout=0.1)
                if uid is None:
                    continue  # No card is available.

                uid_str = binascii.hexlify(uid).decode()
                print('Found card with UID: %s' % uid_str)

                # Prepare and send UID message
                if binascii.hexlify(uid).decode() == "3914273e":
                    uid_payload = json.dumps({"ID": "Welcome Andy :)"})
                    MQTTPub.PushData('ateam/NFC/UID', uid_payload)
                    uid_picture = json.dumps({"Pic": "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTHoEDAdSasrNgaKcxf_hrl87YNfxvxSbVs4HtMznV_8g&s"})
                    MQTTPub.PushData('ateam/NFC/Pic', uid_picture)
                    subprocess.run(["aplay", "test2.wav"])
                else:
                    uid_payload = json.dumps({"ID": "INTRUDER ALERT"})
                    MQTTPub.PushData('ateam/NFC/UID', uid_payload)
                    uid_picture = json.dumps({"Pic": "https://www.pngkey.com/png/detail/569-5690954_report-abuse-sad-drake-hotline-bling.png"})
                    MQTTPub.PushData('ateam/NFC/Pic', uid_picture)
                    subprocess.run(["aplay", "test.wav"])
                print(f"Sent UID: {uid_payload}")
                time.sleep(1)  # Rate limit to one message per second

            except Exception as e:
                print("Loop Exception: %s" % e)
    except Exception as e:
        print("Main Exception: %s" % e)
    finally:
        GPIO.cleanup()
