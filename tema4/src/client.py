import requests
import json
import struct
from crc import impartire

def decodeData(polinom, data):
    sz = len(polinom)
    tmp = data + '0' * (sz - 1)
    rest = impartire(tmp, polinom)
    return rest

if __name__ == '__main__':    
    header = {'Content-Type': 'application/octet-stream'}
    polinom = 9
    mesaj = b'Salut'
    sz = len(mesaj)
    data = struct.pack('!L{}s'.format(sz),polinom,mesaj)
    data = bytes(data)
    url = 'ec2-3-82-27-168.compute-1.amazonaws.com/crc'
    response = requests.post(url, headers=header, data=data)
    print ('CRC calculat: ', response.content)
    key = bin(polinom)[2:]
    ok = '0' * (len(key) - 1)
    ans = decodeData(key,str(response.content,'utf-8'))
    if ans == ok:
        print("Datele transmise nu prezinta erori")
    else:
        print("Datele trimise prezinta erori")
