# mpu6050.py
from machine import I2C
import math
import ustruct
import time

MPU_ADDR = 0x68
PWR_MGMT_1 = 0x6B
ACCEL_XOUT_H = 0x3B
GYRO_XOUT_H = 0x43
WHO_AM_I = 0x75
SMPLRT_DIV = 0x19
CONFIG = 0x1A
GYRO_CONFIG = 0x1B
ACCEL_CONFIG = 0x1C


class MPU6050:
    def __init__(self, i2c, addr=MPU_ADDR):
        self.i2c = i2c
        self.addr = addr
        # Wake up
        try:
            self.i2c.readfrom_mem(self.addr, WHO_AM_I, 1)
        except Exception as e:
            raise OSError("MPU not found on I2C bus: " + str(e))

        self.i2c.writeto_mem(self.addr, PWR_MGMT_1, bytes([0]))  # wake
        # set default ranges (±2g, ±250°/s)
        self.i2c.writeto_mem(self.addr, SMPLRT_DIV, bytes([7]))
        self.i2c.writeto_mem(self.addr, CONFIG, bytes([0]))
        self.i2c.writeto_mem(self.addr, GYRO_CONFIG, bytes([0]))
        self.i2c.writeto_mem(self.addr, ACCEL_CONFIG, bytes([0]))
        time.sleep_ms(100)

    def _read_raw(self):
        # Read 14 bytes from ACCEL_XOUT_H
        b = self.i2c.readfrom_mem(self.addr, ACCEL_XOUT_H, 14)
        vals = ustruct.unpack('>hhhhhhh', b)  # ax, ay, az, temp, gx, gy, gz
        ax, ay, az, temp, gx, gy, gz = vals
        return ax, ay, az, gx, gy, gz

    def read(self):
        ax, ay, az, gx, gy, gz = self._read_raw()
        # convert to g and deg/s
        accel_scale = 16384.0  # ±2g
        gyro_scale = 131.0     # ±250°/s

        axg = ax / accel_scale
        ayg = ay / accel_scale
        azg = az / accel_scale
        gx_dps = gx / gyro_scale
        gy_dps = gy / gyro_scale
        gz_dps = gz / gyro_scale

        return {'ax': axg, 'ay': ayg, 'az': azg, 'gx': gx_dps, 'gy': gy_dps, 'gz': gz_dps}

    @staticmethod
    def accel_to_angles(ax, ay, az):
        # Roll: rotation around X-axis, Pitch: rotation around Y-axis
        roll = math.degrees(math.atan2(ay, az))
        denom = math.sqrt(ay * ay + az * az)
        pitch = math.degrees(math.atan2(-ax, denom if denom != 0 else 1e-6))
        return roll, pitch

