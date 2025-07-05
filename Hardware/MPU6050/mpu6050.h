#ifndef MPU6050_H
#define MPU6050_H

#ifdef __cplusplus
extern "C" {
#endif

void mpu6050_init(void);
int Read_Quad(void);

extern float pitch, roll, yaw;

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_H */