
import serial
import time
import numpy as np
import paho.mqtt.client as paho
mqttc = paho.Client()
# XBee setting
serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600)

host = "172.16.209.0"
topic = "velocity"
port = 1883

s.write("+++".encode())
char = s.read(2)
print("Enter AT mode.")
print(char.decode())

s.write("ATMY 0x140\r\n".encode())
char = s.read(3)
print("Set MY 0x140.")
print(char.decode())

s.write("ATDL 0x240\r\n".encode())
char = s.read(3)
print("Set DL 0x240.")
print(char.decode())

s.write("ATID 0x1\r\n".encode())
char = s.read(3)
print("Set PAN ID 0x1.")
print(char.decode())

s.write("ATWR\r\n".encode())
char = s.read(3)
print("Write config.")
print(char.decode())

s.write("ATMY\r\n".encode())
char = s.read(4)
print("MY :")
print(char.decode())

s.write("ATDL\r\n".encode())
char = s.read(4)
print("DL : ")
print(char.decode())

s.write("ATCN\r\n".encode())
char = s.read(3)
print("Exit AT mode.")
print(char.decode())

print("start sending RPC")

Fs = 100.0;  # sampling rate

Ts = 10.0/Fs; # sampling interval

X = np.arange(0,10,Ts) # signal vector; create Fs samples

Y = np.arange(0,10,Ts) 



s.write("\r".encode()) #CLEAN IT FIRST
time.sleep(1)

for a in range(0, int(Fs)):
    # send RPC to remote
    s.write("/getvelocity/run\r".encode())
    
    char = s.readline()

    charx = char[0:9]
    X[a] = float(charx)

    chary = char[9:18]
    Y[a] = float(chary)
    
    print(char.decode())
    time.sleep(0.1) # get velocity every 0.1s


print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

for a in range(0, int(Fs)):
    mesg = X[a]
    mqttc.publish(topic, mesg)
    print(mesg)
    time.sleep(1)

s.close()