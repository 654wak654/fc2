import struct

flightConfig = "E:/FLT_LOG.BIN"


def getf(data):
    return struct.unpack("f", data)[0]


with open(flightConfig, mode='rb') as f:
    while f.readable:
        print("currentTime =", int.from_bytes(f.read(4), byteorder='little'))
        print("altitude =", getf(f.read(4)))
        accX = getf(f.read(4))
        print("accX =", accX)
        accY = getf(f.read(4))
        print("accY =", accX)
        accZ = getf(f.read(4))
        print("accZ =", accX)
        print("calculated acceleration =", pow(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2), 0.5))
        print("angleX =", getf(f.read(4)))
        print("angleY =", getf(f.read(4)))
        print("angleZ =", getf(f.read(4)))
        print("stageSeperated =", f.read(1) == 1)
        print("parachuteDeployed =", f.read(1) == 1)
        print("\n")
