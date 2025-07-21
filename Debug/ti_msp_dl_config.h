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



/* Defines for PWM */
#define PWM_INST                                                           TIMA1
#define PWM_INST_IRQHandler                                     TIMA1_IRQHandler
#define PWM_INST_INT_IRQN                                       (TIMA1_INT_IRQn)
#define PWM_INST_CLK_FREQ                                               32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_C0_PORT                                                   GPIOB
#define GPIO_PWM_C0_PIN                                            DL_GPIO_PIN_0
#define GPIO_PWM_C0_IOMUX                                        (IOMUX_PINCM12)
#define GPIO_PWM_C0_IOMUX_FUNC                       IOMUX_PINCM12_PF_TIMA1_CCP0
#define GPIO_PWM_C0_IDX                                      DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_C1_PORT                                                   GPIOB
#define GPIO_PWM_C1_PIN                                            DL_GPIO_PIN_1
#define GPIO_PWM_C1_IOMUX                                        (IOMUX_PINCM13)
#define GPIO_PWM_C1_IOMUX_FUNC                       IOMUX_PINCM13_PF_TIMA1_CCP1
#define GPIO_PWM_C1_IDX                                      DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG0)
#define TIMER_0_INST_IRQHandler                                 TIMG0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                           (799U)




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


/* Defines for UART_WIT */
#define UART_WIT_INST                                                      UART2
#define UART_WIT_INST_FREQUENCY                                         32000000
#define UART_WIT_INST_IRQHandler                                UART2_IRQHandler
#define UART_WIT_INST_INT_IRQN                                    UART2_INT_IRQn
#define GPIO_UART_WIT_RX_PORT                                              GPIOB
#define GPIO_UART_WIT_RX_PIN                                      DL_GPIO_PIN_16
#define GPIO_UART_WIT_IOMUX_RX                                   (IOMUX_PINCM33)
#define GPIO_UART_WIT_IOMUX_RX_FUNC                    IOMUX_PINCM33_PF_UART2_RX
#define UART_WIT_BAUD_RATE                                                (9600)
#define UART_WIT_IBRD_32_MHZ_9600_BAUD                                     (208)
#define UART_WIT_FBRD_32_MHZ_9600_BAUD                                      (21)
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
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)





/* Defines for DMA_WIT */
#define DMA_WIT_CHAN_ID                                                      (0)
#define UART_WIT_INST_DMA_TRIGGER                            (DMA_UART2_RX_TRIG)


/* Port definition for Pin Group GPIO_MPU6050 */
#define GPIO_MPU6050_PORT                                                (GPIOB)

/* Defines for PIN_INT: GPIOB.13 with pinCMx 30 on package pin 1 */
// groups represented: ["SR04","GPIO_MPU6050"]
// pins affected: ["Echo","PIN_INT"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_MPU6050_PIN_INT_IIDX                           (DL_GPIO_IIDX_DIO13)
#define GPIO_MPU6050_PIN_INT_PIN                                (DL_GPIO_PIN_13)
#define GPIO_MPU6050_PIN_INT_IOMUX                               (IOMUX_PINCM30)
/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for PIN_22: GPIOB.22 with pinCMx 50 on package pin 21 */
#define LED_PIN_22_PIN                                          (DL_GPIO_PIN_22)
#define LED_PIN_22_IOMUX                                         (IOMUX_PINCM50)
/* Port definition for Pin Group GPIO_Servo */
#define GPIO_Servo_PORT                                                  (GPIOB)

/* Defines for PIN_15: GPIOB.15 with pinCMx 32 on package pin 3 */
#define GPIO_Servo_PIN_15_PIN                                   (DL_GPIO_PIN_15)
#define GPIO_Servo_PIN_15_IOMUX                                  (IOMUX_PINCM32)
/* Port definition for Pin Group OLED */
#define OLED_PORT                                                        (GPIOB)

/* Defines for SCL: GPIOB.9 with pinCMx 26 on package pin 61 */
#define OLED_SCL_PIN                                             (DL_GPIO_PIN_9)
#define OLED_SCL_IOMUX                                           (IOMUX_PINCM26)
/* Defines for SDA: GPIOB.8 with pinCMx 25 on package pin 60 */
#define OLED_SDA_PIN                                             (DL_GPIO_PIN_8)
#define OLED_SDA_IOMUX                                           (IOMUX_PINCM25)
/* Defines for AIN1: GPIOA.2 with pinCMx 7 on package pin 42 */
#define Control_AIN1_PORT                                                (GPIOA)
#define Control_AIN1_PIN                                         (DL_GPIO_PIN_2)
#define Control_AIN1_IOMUX                                        (IOMUX_PINCM7)
/* Defines for AIN2: GPIOA.7 with pinCMx 14 on package pin 49 */
#define Control_AIN2_PORT                                                (GPIOA)
#define Control_AIN2_PIN                                         (DL_GPIO_PIN_7)
#define Control_AIN2_IOMUX                                       (IOMUX_PINCM14)
/* Defines for BIN1: GPIOB.10 with pinCMx 27 on package pin 62 */
#define Control_BIN1_PORT                                                (GPIOB)
#define Control_BIN1_PIN                                        (DL_GPIO_PIN_10)
#define Control_BIN1_IOMUX                                       (IOMUX_PINCM27)
/* Defines for BIN2: GPIOB.11 with pinCMx 28 on package pin 63 */
#define Control_BIN2_PORT                                                (GPIOB)
#define Control_BIN2_PIN                                        (DL_GPIO_PIN_11)
#define Control_BIN2_IOMUX                                       (IOMUX_PINCM28)
/* Port definition for Pin Group encoder */
#define encoder_PORT                                                     (GPIOA)

/* Defines for left_A: GPIOA.15 with pinCMx 37 on package pin 8 */
// pins affected by this interrupt request:["left_A","left_B","right_A","right_B"]
#define encoder_INT_IRQN                                        (GPIOA_INT_IRQn)
#define encoder_INT_IIDX                        (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define encoder_left_A_IIDX                                 (DL_GPIO_IIDX_DIO15)
#define encoder_left_A_PIN                                      (DL_GPIO_PIN_15)
#define encoder_left_A_IOMUX                                     (IOMUX_PINCM37)
/* Defines for left_B: GPIOA.16 with pinCMx 38 on package pin 9 */
#define encoder_left_B_IIDX                                 (DL_GPIO_IIDX_DIO16)
#define encoder_left_B_PIN                                      (DL_GPIO_PIN_16)
#define encoder_left_B_IOMUX                                     (IOMUX_PINCM38)
/* Defines for right_A: GPIOA.17 with pinCMx 39 on package pin 10 */
#define encoder_right_A_IIDX                                (DL_GPIO_IIDX_DIO17)
#define encoder_right_A_PIN                                     (DL_GPIO_PIN_17)
#define encoder_right_A_IOMUX                                    (IOMUX_PINCM39)
/* Defines for right_B: GPIOA.22 with pinCMx 47 on package pin 18 */
#define encoder_right_B_IIDX                                (DL_GPIO_IIDX_DIO22)
#define encoder_right_B_PIN                                     (DL_GPIO_PIN_22)
#define encoder_right_B_IOMUX                                    (IOMUX_PINCM47)
/* Defines for TRIG: GPIOB.14 with pinCMx 31 on package pin 2 */
#define HCSR04_TRIG_PORT                                                 (GPIOB)
#define HCSR04_TRIG_PIN                                         (DL_GPIO_PIN_14)
#define HCSR04_TRIG_IOMUX                                        (IOMUX_PINCM31)
/* Defines for ECHO: GPIOA.12 with pinCMx 34 on package pin 5 */
#define HCSR04_ECHO_PORT                                                 (GPIOA)
#define HCSR04_ECHO_PIN                                         (DL_GPIO_PIN_12)
#define HCSR04_ECHO_IOMUX                                        (IOMUX_PINCM34)
/* Port definition for Pin Group SR04 */
#define SR04_PORT                                                        (GPIOB)

/* Defines for Trig: GPIOB.4 with pinCMx 17 on package pin 52 */
#define SR04_Trig_PIN                                            (DL_GPIO_PIN_4)
#define SR04_Trig_IOMUX                                          (IOMUX_PINCM17)
/* Defines for Echo: GPIOB.5 with pinCMx 18 on package pin 53 */
#define SR04_Echo_IIDX                                       (DL_GPIO_IIDX_DIO5)
#define SR04_Echo_PIN                                            (DL_GPIO_PIN_5)
#define SR04_Echo_IOMUX                                          (IOMUX_PINCM18)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_I2C_MPU6050_init(void);
void SYSCFG_DL_UART_WIT_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
