
# python ccs_workspace\odiseo-hemisferioDerecho\utils\rx.py

import serial

ser = serial.Serial('COM3', 9600)

print "RX:"

while True:
	b = ser.read()
	print b.encode("hex") ,
ser.close()
