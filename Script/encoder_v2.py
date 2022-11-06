# coding: utf-8

_debug = True
import time
import serial

port = serial.Serial('COM4', 115200, timeout=1)

port.reset_input_buffer()

print ("Sending set_config: work mode...");
port.write(b'at+set_config=lora:work_mode:1\r\n')
answer = port.read(1000)
print (b'Answer:' + answer)
if not b'OK \r\n' in answer:
    print ("No reply from module")
    if not _debug:
        exit(1)

print ("Sending set_config: parameters setup...");    
port.write(b'at+set_config=lorap2p:869525000:7:0:1:5:5\r\n')
answer = port.read(100)
print (b'Answer:' + answer)
if not b'OK \r\n' in answer:
    print ("No reply from module")
    if not _debug:
        exit(1)   

while True:        
    print ("Sending set_config: transmitter mode"); 
    port.write(b'at+set_config=lorap2p:transfer_mode:2\r\n')
    answer = port.read(100)
    print (b'Answer:' + answer)
    if not b'OK \r\n' in answer:
        print ("No reply from module")
        if not _debug:
            exit(1)

    print ("Sending Modbus request..."); 
    port.write(b'at+send=lorap2p:3A30313033303030303030303446380D0A\r\n')
    answer = port.read(5)
    print (b'Answer:' + answer)
    if not b'OK \r\n' in answer:
        print ("No reply from module")
        if not _debug:
            exit(1)   
        
    print ("Sending set_config: receiver mode..."); 
    port.write(b'at+set_config=lorap2p:transfer_mode:1\r\n')
    answer = port.read(5)
    print (b'Answer:' + answer)
    if not b'OK \r\n' in answer:
        print ("No reply from module")
        if not _debug:
            exit(1)
    
    time.sleep(1)    
    answer = port.read(100)

    #answer = b'at+recv=-30,21,5:3A3031303330383245333341363439303130303031303037310D0A\r\n'


    if (b'at+recv' in answer):
        body = answer[answer.index(b':')+1:answer.index(b'\r')]
        #print (body)
        Modbus_ascii = b''
        for i in range (0,len(body),2):
            st = body[i:i+2]
            symbol = bytes([int(body[i:i+2],16)])
            #print (st + b' -> ' + symbol)
            Modbus_ascii += symbol
        #print (b'Modbus packet: ' + Modbus_ascii)
        
        Position = Modbus_ascii[7:15]
        Velocity = Modbus_ascii[15:23]
        
        print (b'Position = ' + Position)
        print (b'Velocity = ' + Velocity)    
        
        Position = int(Position[0:4], 16)
        Velocity = int(Velocity[0:4], 16)
        
        print ('Position = ' + str(Position))
        print ('Velocity = ' + str(Velocity))
   