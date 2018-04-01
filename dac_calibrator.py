#! /usr/bin/env python

import ivi
import time
import csv 
import serial

meter = ivi.agilent.agilent34461A("TCPIP0::192.168.1.48::INSTR")
currentsink = serial.Serial('COM15',115200)


for i in range(0,4100,100):
	
	# first set the dac  
	currentsink.write(f"dacset {i}\r\n".encode())
	
	#yawn
	time.sleep(1)

	# now read the output
	reading = float(meter.measurement.read(1))

	# currentsink.write(f"dacsave {i} {round(reading,3):.3f}\r\n".encode())
	print(f"{round(reading,3):.3f} {i}")

	#yawn
	time.sleep(1)

currentsink.write(f"dacset 0\r\n".encode())
time.sleep(1)
currentsink.write(f"dacfin\r\n".encode())
