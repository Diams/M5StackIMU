#ifndef M5STACK_IMU_SRC_IMU_IMU_H_
#define M5STACK_IMU_SRC_IMU_IMU_H_

typedef struct {
  float x;
  float y;
  float z;
} AccelerationSensorData;

typedef struct {
  float x;
  float y;
  float z;
} GyroSensorData;

typedef struct {
  float pitch;
  float roll;
  float yaw;
} AhrsData;

typedef struct {
  AccelerationSensorData acceleration_sensor;
  GyroSensorData gyro_sensor;
  AhrsData ahrs;
} ImuData;

#endif  //  M5STACK_IMU_SRC_IMU_IMU_H_
