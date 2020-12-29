import serial

ser = serial.Serial('COM3', 9600)

print "Sending test frame..."

# FRAME TO REQUEST SendTelemetry
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\x1c\xf0\xf5\xcb\xce\x8b\xf6\x48\xa7\x21\x83\xe7\x19\x40\x4c\x1d\x7e\x7e")

# FRAME TO WRITE IN MEMORY
#ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\xe6\x67\x7d\xc5\x97\x7d\xca\xcb\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\xc1\xd8\xe3\xd8\x98\x5a\xf4\x4f\x64\x7f\x4c\x1d\x7e\x7e")

# FRAME TO OVERFLOW MEMORY
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\xe6\x67\x7d\xc5\x97\x7d\xca\xcb\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\x34\xe6\xe9\x3b\xc1\xd8\xec\x84\x24\x48\x26\xce\x05\xb0\x4c\x1d\x7e\x7e")

# FRAME TO REQUEST MEMORY
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\x13\xa3\xef\x07\x4d\x18\x3e\x84\x50\x96\xf9\x60\xca\xb0\x4c\x1d\x7e\x7e")

# FRAME TO REQUEST LOGS
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\x13\x59\x8d\xb1\x49\xa2\x91\xc0\x53\x1a\x39\xb2\x30\x5c\xbc\x1d\x7e\x7e")

# FRAME TO CLEAR LOGS
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\xe6\x92\xb6\xf1\x8b\x03\x79\x0b\x13\x2d\xa6\xa5\x26\x40\x4c\x1d\x7e\x7e")

# FRAME TO CLEAN MEMORY
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\xe6\x9d\xea\x42\xc5\x62\x43\xc0\x28\x6d\x9e\x9c\xef\xf2\x73\x1d\x7e\x7e")

# FRAME TO CHANGE BEACON FREQUENCY to 250 ms
# WARNING
# ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\x1c\x05\x3e\xf0\x7b\x8d\x3f\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf5\xcb\xce\x8b\xf9\x14\xee\x02\xf2\x08\x6a\x29\x13\xac\xbc\x1d\x7e\x7e")

# FRAME TO CHANGE BEACON FREQUENCY to 2^32 ms
#ser.write("\x7e\x7e\x7e\x7e\x06\xc7\xf4\xe8\x01\x02\x21\x93\x0f\x5c\xbc\x1d\x81\x6e\x9c\x1a\x39\xbd\x6c\x15\x90\xca\xb0\xb9\xd9\x13\xac\x46\x70\x64\x7f\x43\x4e\x6e\x93\xbc\x12\xd2\x81\x61\xcf\x0f\x53\x1a\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\xbf\xea\x39\xbd\x6c\x15\x90\xca\x45\x7d\x44\x82\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xbd\x6c\x1a\x39\xb2\x3f\x0f\xa9\x77\xd3\xf0\xf5\xcb\x34\xe6\x1c\x05\x3e\xf0\x8e\x46\xfe\x2d\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\x27\xb0\xcd\x07\x39\xc6\xee\x83\x1d\x7e\x7e")

print "SENT"

# Wait for confirmation
print "Waiting for confirmation..."
response = ser.read(90)
print ' '.join(x.encode('hex') for x in response)

print "CONFIRMED"

while True:
	b = ser.read()
	print b.encode("hex") ,
ser.close()

# python ccs_workspace\odiseo-hemisferioDerecho\utils\ax25_test.py
