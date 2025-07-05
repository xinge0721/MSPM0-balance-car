#ifndef MSPM0_I2C_H
#define MSPM0_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

int mspm0_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data);

int mspm0_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data);

#ifdef __cplusplus
}
#endif

#endif /* MSPM0_I2C_H */