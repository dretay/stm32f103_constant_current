import ivi
import numpy as np
import vxi11
import re
from numpy import *
from matplotlib import pyplot as plt

awg = ivi.tektronix.tektronixAWG2021("ASRL::COM29,19200::INSTR")
tds = vxi11.Instrument("192.168.1.25")
class AsciiWaveform:

	def __init__(self, waveformString ):
		waveform_list = re.split(';',waveformString)
		self.waveformSettings = {}
		self.CURVE(waveform_list[17])
		# self.waveFormOptions= {
		# 	0:self.BYT_NR, 1:self.BIT_NR, 2:self.ENCDG, 3:self.BN_FMT, 4:self.BYT_OR, 5:self.WFID,
		# 	6:self.NR_PT, 7:self.PT_FMT, 8:self.PT_ORDER, 9:self.XUNIT, 10:self.XINCR, 
		# 	11:self.XZERO, 12:self.PT_OFF, 13:self.YUNIT, 14:self.YMULT, 15:self.YOFF, 16:self.YOFF, 
		# 	17:self.YZERO, 22:self.CURVE
		# }
		# for idx, val in enumerate(waveform_list):
		# 	try:
		# 		self.waveFormOptions[idx](val)
		# 	except:
		# 		pass

	def BYT_NR(self,value):
		self.waveformSettings['BYTE_NR'] = value

	def BIT_NR(self,value):
		self.waveformSettings['BIT_NR'] = value
	
	def ENCDG(self,value):
		self.waveformSettings['ENCDG'] = value

	def BN_FMT(self,value):
		self.waveformSettings['BN_FMT'] = value

	def BYT_OR(self,value):
		self.waveformSettings['BYT_OR'] = value

	def WFID(self,value):
		self.waveformSettings['WFID'] = value

	def NR_PT(self,value):
		self.waveformSettings['NR_PT'] = value

	def PT_FMT(self,value):
		self.waveformSettings['PT_FMT'] = value

	def PT_ORDER(self,value):
		self.waveformSettings['PT_ORDER'] = value

	def XUNIT(self,value):
		self.waveformSettings['XUNIT'] = value

	def XINCR(self,value):
		self.waveformSettings['XINCR'] = value

	def XZERO(self,value):
		self.waveformSettings['XZERO'] = value

	def PT_OFF(self,value):
		self.waveformSettings['PT_OFF'] = value

	def YUNIT(self,value):
		self.waveformSettings['YUNIT'] = value

	def YMULT(self,value):
		self.waveformSettings['YMULT'] = value

	def YOFF(self,value):
		self.waveformSettings['YOFF'] = value

	def YZERO(self,value):
		self.waveformSettings['YZERO'] = value

	def CURVE(self,value):
		# Make this faster later
		waveform = []
		for val in re.split(',',value):
			try:
				waveform.append(float(val)/14000)
			except:
				pass
		self.waveform = np.array(waveform)

# 1. Select the waveform source(s) using DATa:SOUrce.
# 2. Specify the waveform data format using DATa:ENCdg.
# 3. Specify the number of bytes per data point using
# WFMOutpre:BYT_Nr.
# Note: MATH waveforms (and REF waveforms that came
# from a MATH) are always set to four bytes.
# 4. Specify the portion of the waveform that you want to
# transfer using DATa:STARt and DATa:STOP.
# 5. Transfer waveform preamble information using
# WFMOutpre?.
# 6. Transfer waveform data from the oscilloscope using
# CURVe?.
tds.write("DATA:SOURCE CH1")
tds.write("DATA:ENCDG ASCII")
# tds.write("WFMINPRE:BN_FMT FP")
tds.write("DATa:BYT_NR 1")
record_length = int(tds.ask("HORizontal:RECOrdlength?"))
tds.write(f'DATa:STOP {record_length}')
tds.write("CURSor:VBArs SNAp")
waveform_data = tds.ask("WAVFrm?")
awf = AsciiWaveform( waveform_data )
plt.plot( awf.waveform )
plt.show()



n = 128
f = 1
a = 1
wfm = a*sin(2*pi/n*f*arange(0,n))
# print(awg._ask(":memory:catalog:all?"))
awg.outputs[0].arbitrary.create_waveform(awf.waveform)
# # transfer to AWG2021
# awg.outputs[0].arbitrary.create_waveform(wfm)
# # 2 volts peak to peak
# awg.outputs[0].arbitrary.gain = 2.0
# # zero offset
# awg.outputs[0].arbitrary.offset = 0.0
# # sample rate 128 MHz
# awg.arbitrary.sample_rate = 128e6
# # enable ouput
# awg.outputs[0].enabled = True