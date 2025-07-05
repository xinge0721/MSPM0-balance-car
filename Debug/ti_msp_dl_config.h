/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000




/* Defines for I2C_MPU6050 */
#define I2C_MPU6050_INST                                                    I2C0
#define I2C_MPU6050_INST_IRQHandler                              I2C0_IRQHandler
#define I2C_MPU6050_INST_INT_IRQN                                  I2C0_INT_IRQn
#define I2C_MPU6050_BUS_SPEED_HZ                                          400000
#define GPIO_I2C_MPU6050_SDA_PORT                                          GPIOA
#define GPIO_I2C_MPU6050_SDA_PIN                                  DL_GPIO_PIN_28
#define GPIO_I2C_MPU6050_IOMUX_SDA                                (IOMUX_PINCM3)
#define GPIO_I2C_MPU6050_IOMUX_SDA_FUNC                 IOMUX_PINCM3_PF_I2C0_SDA
#define GPIO_I2C_MPU6050_SCL_PORT                                          GPIOA
#define GPIO_I2C_MPU6050_SCL_PIN                                  DL_GPIO_PIN_31
#define GPIO_I2C_MPU6050_IOMUX_SCL                                (IOMUX_PINCM6)
#define GPIO_I2C_MPU6050_IOMUX_SCL_FUNC                 IOMUX_PINCM6_PF_I2C0_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                         DL_GPIO_PIN_1
#define GPIO_UART_0_TX_PIN                                         DL_GPIO_PIN_0
#define GPIO_UART_0_IOMUX_RX                                      (IOMUX_PINCM2)
#define GPIO_UART_0_IOMUX_TX                                      (IOMUX_PINCM1)
#define GPIO_UART_0_IOMUX_RX_FUNC                       IOMUX_PINCM2_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                       IOMUX_PINCM1_PF_UART0_TX
#define UART_0_BAUD_RATE                                                  (9600)
#define UART_0_IBRD_32_MHZ_9600_BAUD                                       (208)
#define UART_0_FBRD_32_MHZ_9600_BAUD                                        (21)





/* Port definition for Pin Group GPIO_MPU6050 */
#define GPIO_MPU6050_PORT                                                (GPIOB)

/* Defines for PIN_INT: GPIOB.13 with pinCMx 30 on package pin 1 */
// groups represented: ["encoder","GPIO_MPU6050"]
// pins affected: ["left_A","left_B","PIN_INT"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_MPU6050_PIN_INT_IIDX                           (DL_GPIO_IIDX_DIO13)
#define GPIO_MPU6050_PIN_INT_PIN                                (DL_GPIO_PIN_13)
#define GPIO_MPU6050_PIN_INT_IOMUX                               (IOMUX_PINCM30)
/* Port definition for Pin Group GPIO_OLED */
#define GPIO_OLED_PORT                                                   (GPIOA)

/* Defines for PIN_SCL: GPIOA.25 with pinCMx 55 on package pin 26 */
#define GPIO_OLED_PIN_SCL_PIN                                   (DL_GPIO_PIN_25)
#define GPIO_OLED_PIN_SCL_IOMUX                                  (IOMUX_PINCM55)
/* Defines for PIN_SDA: GPIOA.27 with pinCMx 60 on package pin 31 */
#define GPIO_OLED_PIN_SDA_PIN                                   (DL_GPIO_PIN_27)
#define GPIO_OLED_PIN_SDA_IOMUX                                  (IOMUX_PINCM60)
/* Defines for AIN1: GPIOA.8 with pinCMx 19 on package pin 54 */
#define Control_AIN1_PORT                                                (GPIOA)
#define Control_AIN1_PIN                                         (DL_GPIO_PIN_8)
#define Control_AIN1_IOMUX                                       (IOMUX_PINCM19)
/* Defines for AIN2: GPIOA.7 with pinCMx 14 on package pin 49 */
#define Control_AIN2_PORT                                                (GPIOA)
#define Control_AIN2_PIN                                         (DL_GPIO_PIN_7)
#define Control_AIN2_IOMUX                                       (IOMUX_PINCM14)
/* Defines for BIN1: GPIOB.14 with pinCMx 31 on package pin 2 */
#define Control_BIN1_PORT                                                (GPIOB)
#define Control_BIN1_PIN                                        (DL_GPIO_PIN_14)
#define Control_BIN1_IOMUX                                       (IOMUX_PINCM31)
/* Defines for BIN2: GPIOB.15 with pinCMx 32 on package pin 3 */
#define Control_BIN2_PORT                                                (GPIOB)
#define Control_BIN2_PIN                                        (DL_GPIO_PIN_15)
#define Control_BIN2_IOMUX                                       (IOMUX_PINCM32)
/* Defines for left_A: GPIOB.4 with pinCMx 17 on package pin 52 */
#define encoder_left_A_PORT                                              (GPIOB)
#define encoder_left_A_IIDX                                  (DL_GPIO_IIDX_DIO4)
#define encoder_left_A_PIN                                       (DL_GPIO_PIN_4)
#define encoder_left_A_IOMUX                                     (IOMUX_PINCM17)
/* Defines for left_B: GPIOB.5 with pinCMx 18 on package pin 53 */
#define encoder_left_B_PORT                                              (GPIOB)
#define encoder_left_B_IIDX                                  (DL_GPIO_IIDX_DIO5)
#define encoder_left_B_PIN                                       (DL_GPIO_PIN_5)
#define encoder_left_B_IOMUX                                     (IOMUX_PINCM18)
/* Defines for right_A: GPIOA.10 with pinCMx 21 on package pin 56 */
#define encoder_right_A_PORT                                             (GPIOA)
// pins affected by this interrupt request:["right_A","right_B"]
#define encoder_GPIOA_INT_IRQN                                  (GPIOA_INT_IRQn)
#define encoder_GPIOA_INT_IIDX                  (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define encoder_right_A_IIDX                                (DL_GPIO_IIDX_DIO10)
#define encoder_right_A_PIN                                     (DL_GPIO_PIN_10)
#define encoder_right_A_IOMUX                                    (IOMUX_PINCM21)
/* Defines for right_B: GPIOA.11 with pinCMx 22 on package pin 57 */
#define encoder_right_B_PORT                                             (GPIOA)
#define encoder_right_B_IIDX                                (DL_GPIO_IIDX_DIO11)
#define encoder_right_B_PIN                                     (DL_GPIO_PIN_11)
#define encoder_right_B_IOMUX                                    (IOMUX_PINCM22)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_I2C_MPU6050_init(void);
void SYSCFG_DL_UART_0_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
