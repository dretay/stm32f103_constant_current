#! /usr/bin/env python

import visa
import time
import csv

class Instrument(object):
	def __init__(self, visa_name):
		rm = visa.ResourceManager()
		self.resource = rm.open_resource(visa_name)
	
	def query(self, param):
		return self.resource.query(param)
	def write(self, param):
		return self.resource.write(param)
	def close(self):
		return self.resource.close()

class SerialInstrument(Instrument):
	def __init__(self, visa_name, baud_rate):		
		super(SerialInstrument, self).__init__(visa_name)
		self.resource.baud_rate = baud_rate

class ConstantCurrent(SerialInstrument):	
	def __init__(self,visa_name,baud_rate):
		super(ConstantCurrent, self).__init__(visa_name, baud_rate)
		self._dac = 0
		self.resource.tiemout = 0		

	@property
	def dac(self):
		return self._dac
	@dac.setter
	def dac(self,value):
		self.resource.write(f'dac {value}','\r')
		self._dac = value		

class K34461A(Instrument):
	# note: sending G1 command will disable prefixed data
	@property
	def voltage(self):
		return float(self.resource.query(""))

	def display_off(self):
		self.resource.query("D@X")
visa.log_to_screen()
#k34461a = K34461A('TCPIP0::192.168.1.48')
constant_current = ConstantCurrent('COM17',115200)

constant_current.dac = 100

constant_current.close()
#k34461a.close()

# dcsource = Agilent66309D('GPIB1::4::INSTR')
# dmm = Keithley2015('GPIB1::2::INSTR')
# dmm2 = Keithley196('GPIB1::3::INSTR')

# start by initializing good cal constants

# cv "L" readback 





# with open('calibration.csv', 'wb') as csvfile:
# 	writer = csv.writer(csvfile, delimiter=',')	
# 	for offset in xrange(0,1):
# 		# first set the output voltage to 6v
# 		pdvs2.voltage = 6

# 		# then set the offset
# 		fel150.offset = offset

# 		# now perform a binary search on the slope to find a value closest to 6v
# 		fel150.slope = 0.02000
# 		slope_adjustment = fel150.slope
# 		for x in xrange(1,20):
# 			slope_adjustment /= 2
# 			if fel150.voltage == 6.0:
# 				break
# 			elif fel150.voltage > 6.0:		
# 				fel150.slope -= slope_adjustment
# 			else:
# 				fel150.slope += slope_adjustment

	
# 		# finally measure the accuracy across the full voltage scale
# 		pdvs2.voltage = 1
# 		fel150_1v = fel150.voltage

# 		pdvs2.voltage = 10
# 		fel150_10v = fel150.voltage
# 		writer.writerow( (fel150.offset, fel150.slope, fel150_1v, fel150_10v) )

# 		print("{} - done({})\n".format(offset, x))
# for offset in xrange(0,1):
	
# 	# then set the offset
# 	fel150.offset = offset

# 	# now perform a binary search on the slope to find a value closest to 6v
# fel150.slope = 0.000716
# slope_adjustment = fel150.slope
# for x in xrange(0,20):
# 	fel150.query("curr 0.5")
# 	slope_adjustment /= 2
# 	current = dmm.current	
# 	if current == 0.5:
# 		break
# 	elif current > 0.5:			
# 		fel150.slope += .000001
# 	else:		
# 		fel150.slope -= .000001
# 	print("{}\n".format(fel150.slope))
	


# print("{} - done({})\n".format(fel150.slope, x))