# coding: utf-8

_debug = True
import time
import serial

def CalculateCRC8(data):
    crc = 0xff
    for index,item in enumerate(data):
        crc = crc ^ data[index]
        for i in range(8):
            if crc & 0x80:
                crc = ((crc << 1) & 0xff) ^ 0x31
            else:
                crc = (crc << 1) & 0xff
    return crc




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

print ("Sending set_config: receiver mode...");
port.write(b'at+set_config=lorap2p:transfer_mode:1\r\n')
answer = port.read(5)
print (b'Answer:' + answer)
if not b'OK \r\n' in answer:
    print ("No reply from module")
    if not _debug:
        exit(1)

while True:
    answer = port.readline()
    port.reset_input_buffer()
    # print(answer)

    if (b'at+recv' in answer):
        body = answer[answer.index(b':')+1:answer.index(b'\r')]
        # print('Body ' + str(body))
        bodyList = []
        for i in range(0,len(body),2):
            symbol = int(body[i:i+2],16)
            bodyList.append(symbol)
        checkSumReceived = bodyList[len(bodyList)-1]
        checkSumCalculated = CalculateCRC8(bodyList[0:len(bodyList) - 1])
        # print('Check sum received = ' + hex(checkSumReceived))
        # print('Calculate check sum = ' + hex(checkSumCalculated))
        if checkSumReceived == checkSumCalculated:
            print("")

            TimeStamp = int(body[0:12],16)
            print("Time stamp [ms] = {}".format(TimeStamp))

            PositionCode = int(body[21:28],16)
            # print("PositionCode = {}".format(PositionCode))
            FactorOfposition = 332.4 / 16384
            Position = (PositionCode * FactorOfposition)

            # print("body[13] = {}".format(body[13:15]))

            if body[13:15] == b'FF':
                # print(body[12:20])
                Velocity = int('FFFFFFFF',16) - int(body[12:20], 16)
            else:
                Velocity = int(body[13:20], 16) * (-1)

            print('Position = ' + str(Position))
            print ('Velocity = ' + str(Velocity))

    time.sleep(0.5)