#! /usr/bin/python3

import os
from socket import timeout
import time
import sys
import serial

BAUD_RATE = 115200

def send_img(ser,kernel):
    print(ser.read_until(b"Please sent the kernel image size:").decode(), end="")
    kernel_size=os.stat(kernel).st_size
    ser.write((str(kernel_size)+"\n").encode())
    print(ser.read_until(b"Start to load the kernel image... \r\n").decode(), end="")

    with open(kernel, "rb") as image:
        while kernel_size > 0:
            kernel_size -= ser.write(image.read(1))
            ser.read_until(b".")
    print(ser.read_until(b"$ ").decode(), end="")
    return

if __name__ == "__main__":
    # try:
    #     ser = serial.Serial("/dev/ttyUSB0", BAUD_RATE, timeout=5)
    #     print("success access raspi3")
    # except:
    #     print("Error to open the path with raspi3")
    ser = serial.Serial("/dev/ttyUSB0", BAUD_RATE, timeout=5)


    input_f=open("./load_img_input","r")
    print(ser.read_until(b"$ ").decode(), end="")
    while True:
        command=(input_f.readline()).rstrip()
        ser.write((command+"\n").encode())
        if command=="." :
            break
        elif command=="load_img" :
            kernel=(input_f.readline()).rstrip()
            send_img(ser,kernel)
        else:
            print(ser.read_until(b"$ ").decode(), end="")

    

