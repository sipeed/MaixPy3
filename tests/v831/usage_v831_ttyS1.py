import serial

ser = serial.Serial("/dev/ttyS1",115200)

print('serial test start ...')
ser.write(b"Hello Wrold !!!\n")
try:
    while True:
        ser.setDTR(True)
        ser.setRTS(True)
        tmp = ser.readline()
        print(tmp)
        ser.write(tmp)
        ser.setDTR(False)
        ser.setRTS(False)
except KeyboardInterrupt:
    if ser != None:
        ser.close()