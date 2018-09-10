# Copyright (C) 2018 Ozan Egitmen
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import struct

flightConfig = "E:/FLT_LOG.BIN"
# TODO: Also write a reader in arduino so data can be read without removing sd card


def getf(handle):
    return struct.unpack("f", handle.read(4))[0]


with open(flightConfig, mode='rb') as f:

    # TODO: Read the 28 initial bytes, then read below untill we get 8 null, then repeat
    print("currentTime =", int.from_bytes(f.read(4), byteorder='little', signed=False))
    print("altitude =", getf(f))
    accX = getf(f)
    print("accX =", accX)
    accY = getf(f)
    print("accY =", accX)
    accZ = getf(f)
    print("accZ =", accX)
    print("acceleration =", pow(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2), 0.5))
    print("angleX =", getf(f))
    print("angleY =", getf(f))
    print("angleZ =", getf(f))
    print("bmpTemperature =", getf(f))
    print("mpuTemperature =", getf(f))
    print("stageSeperated =", f.read(1) == 1)
    print("parachuteDeployed =", f.read(1) == 1)
    print("\n")
