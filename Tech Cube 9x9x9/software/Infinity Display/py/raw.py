import sys
import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 3333

if len(sys.argv)>1:
    srate = int(sys.argv[1])
else:
    print("Defaulting to 44.1KHz sample rate")
    srate=44100

RAW_OUTPUT_FILENAME = "output_"+str(srate)+".raw"

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

frames = b''

try:
    while True:
        data, addr = sock.recvfrom(1024)
        frames = frames+data
    
except KeyboardInterrupt:  
    print("Writing for rate ",srate)
    rf = open(RAW_OUTPUT_FILENAME, "wb")
    rf.write(frames)
    rf.close()