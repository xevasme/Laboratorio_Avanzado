# Escrfrom machine import Pin, I2C
from mpu6050 import MPU6050
import time

# Configuración del bus I2C para ESP32 D1 R32
# GPIO21 → SDA | GPIO22 → SCL
i2c = I2C(0, scl=Pin(22), sda=Pin(21))

mpu = MPU6050(i2c)

while True:
    data = mpu.read()
    roll, pitch = MPU6050.accel_to_angles(data['ax'], data['ay'], data['az'])

    print("Ax={:.2f} Ay={:.2f} Az={:.2f} | Roll={:.2f}° Pitch={:.2f}°".format(
        data['ax'], data['ay'], data['az'], roll, pitch))

    time.sleep(0.5)
ibe tu código aquí :-)
