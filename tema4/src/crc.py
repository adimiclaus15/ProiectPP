import struct
from flask import Flask
from flask import request
import json

app = Flask(__name__)


@app.route('/')
def hello():
    '''
    Istrati Lucian, Miclaus Adrian
    '''
    return "CRC API de la echipa Dunarea Calarasi!"

#functie care calculeaza xor-ul pentru 2 numere in binar
def xor(a, b):
    sol = []
    for i in range(1,len(b)):
        if a[i] == b[i]:
            sol.append('0')
        else:
            sol.append('1')
    return ''.join(sol)

#efectueaza impartire a doua polinoame din Z2[X]
def impartire(deimpartit, impartitor):
    sz = len(impartitor)
    tmp = deimpartit[0 : sz]
    n = len(deimpartit)
    while sz < n:
        if tmp[0] == '1':
            tmp = xor(impartitor, tmp) + deimpartit[sz]
        else:
            tmp = xor('0' * sz, tmp) + deimpartit[sz]
        sz += 1
    if tmp[0] == '1':
        tmp = xor(impartitor, tmp)
    else:
        tmp = xor('0' * sz, tmp)
    return tmp

def calculeaza_CRC(polinom, data):
    #transformam mesajul si polinomul in binar
    k = str(data,'utf-8')
    mesaj = (''.join(format(ord(x),'b') for x in k))
    key = bin(polinom)[2:]
    sz = len(key)
    #adauga 0-uri la finalul sirului
    tmp = mesaj + '0' * (sz - 1)
    #determinam restul impartiti dintre mesaj si polinom
    rest = impartire(tmp, key)
    #adaugam la mesajul initial restul
    res = mesaj + rest
    return res


@app.route('/crc', methods=['POST','GET'])
def post_method():
    '''
    TODO: implementati aici un endpoint care calculeaza CRC
    '''
    #struct.unpack(....)
    #CRC = calculeaza_CRC()
    print(len(request.data))
    #v = struct.unpack('!L3s',request.data)
    v = struct.unpack('!L{}s'.format(len(request.data) - 4),request.data)
    print('serverul a primit:', v[1])
    crc = calculeaza_CRC(v[0],v[1])
    data = {'mesaj':'hello'}
    #return crc
    return crc


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)