#! /usr/bin/env python

import ivi
import time
import csv 
import serial
import vxi11

# supply = vxi11.Instrument("192.168.1.36")
# meter = ivi.agilent.agilent34461A("TCPIP0::192.168.1.27::INSTR")
currentsink = serial.Serial('COM26',115200, timeout=0.1)

def send(input):
  currentsink.write(input)
  time.sleep(0.1)

def send_and_recieve(input):
  currentsink.flushInput()
  currentsink.flushOutput()
  currentsink.write(input)
  while True:
    try: 
    	time.sleep(0.01)
    	state = currentsink.readline()
    	return state
    except:
    	pass
  time.sleep(0.1)

#current calibration loop
def calibrate_current_sense():
	send(f"setdac 0\r\n".encode())
	time.sleep(2)
	for i in range(0,4100,100):
		
		# first set the dac  	
		send(f"setdac {i}\r\n".encode())
		# #yawn
		time.sleep(2)
			
		# now read the output
		
		supply_reading = float(supply.ask("MEAS:CURR? CH1"))*2
			
		sink_reading = int(send_and_recieve(f"getrawc\r\n".encode()))
		
		# sink_reading = float(currentsink.readline().rstrip())
		

		# # currentsink.write(f"dacsave {i} {round(reading,3):.3f}\r\n".encode())
		print(f"{sink_reading} {round(supply_reading,3):.3f} ")


def calibrate_dac():
	send(f"setdac 0\r\n".encode())
	time.sleep(2)
	for i in range(0,4100,100):
		
		# first set the dac  	
		send(f"setdac {i}\r\n".encode())
		time.sleep(2)
			
		# now read the output
		supply_reading = float(supply.ask("MEAS:CURR? CH1"))*2
		
		# sink_reading = float(currentsink.readline().rstrip())
		

		# # currentsink.write(f"dacsave {i} {round(reading,3):.3f}\r\n".encode())
		print(f"{round(supply_reading,3):.3f} {i}")

def calibrate_voltage_sense():
	for i in range(0,31,1):
		
		# first set the dac  					
		supply.write(f"CH1:VOLT {i}")
		time.sleep(2)
			
		# now read the output
		dmm_reading = float(meter.measurement.read(1))
			
		sink_reading = int(send_and_recieve(f"getrawv\r\n".encode()))
		
		# sink_reading = float(currentsink.readline().rstrip())
		

		# # currentsink.write(f"dacsave {i} {round(reading,3):.3f}\r\n".encode())
		print(f"{sink_reading} {round(dmm_reading,3):.3f} ")

		# #yawn
		time.sleep(1)

#calibrate_current_sense()
send(f"setcont 90\r\n".encode())

# supply.write(f"CH1:VOLT 0")
# while(True):
# 	print(float(supply.ask("MEAS:CURR? CH1"))*2)
# 	time.sleep(1)
