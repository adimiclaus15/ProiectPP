import struct
from flask import Flask
from flask import request

app = Flask(__name__)


@app.route('/')
def hello():
    '''
    Scrieti aici numele voastre
    '''
    return "CRC API de la echipa XYXYYXY!"


def calculeaza_CRC(polinom, date):
    '''
    TODO: de implementat
    '''
    CRC = b''
    return CRC


@app.route('/crc', methods=['POST'])
def post_method():
    '''
    TODO: implementati aici un endpoint care calculeaza CRC
    '''
    #struct.unpack(....)
    #CRC = calculeaza_CRC()
    return request.data
    #return CRC


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8001)
