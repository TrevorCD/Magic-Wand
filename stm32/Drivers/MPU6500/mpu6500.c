/* mpu6500.c
 *
 * Device driver source for MPU-6500 Accelerometer. Depends on stm32's HAL.
 *
 *-----------------------------------------------------------------------------
 *
 *   Copyright 2026 Trevor B. Calderwood
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *----------------------------------------------------------------------------*/

/* Hardware Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "mpu6500.h"

/* MPU-6500 Registers */
#define SELF_TEST_X_GYRO   (0x00)
#define SELF_TEST_Y_GYRO   (0x01)
#define SELF_TEST_Z_GYRO   (0x02)
#define SELF_TEST_X_ACCEL  (0x0D)
#define SELF_TEST_Y_ACCEL  (0x0E)
#define SELF_TEST_Z_ACCEL  (0x0F)
#define XG_OFFSET_H        (0x13) /* X_OFFS_USR        [15:8] */
#define XG_OFFSET_L        (0x14) /* X_OFFS_USR        [7:0]  */
#define YG_OFFSET_H        (0x15) /* Y_OFFS_USR        [15:8] */
#define YG_OFFSET_L        (0x16) /* Y_OFFS_USR        [7:0]  */
#define ZG_OFFSET_H        (0x17) /* Z_OFFS_USR        [15:8] */
#define ZG_OFFSET_L        (0x18) /* Z_OFFS_USR        [7:0]  */
#define SMPLRT_DIV         (0x19)
#define CONFIG             (0x1A) /* FIFO_MODE         [6]
									 EXT_SYNC_SET      [4:3]
									 DLPF_CFG          [2:0]  */
#define GYRO_CONFIG        (0x1B) /* XG_ST             [7]
									 YG_ST             [6]
									 ZG_ST             [5]
									 GYRO_FS_SEL       [4:3]
									 FCHOICE_B         [1:0]  */
#define ACCEL_CONFIG       (0x1C) /* XA_ST             [7]
									 YA_ST             [6]
									 ZA_ST             [5]
									 ACCEL_FS_SEL      [4:3]  */
#define ACCEL_CONFIG2      (0x1D) /* ACCEL_FC_HOICE_B  [3]
									 A_DLPF_CFG        [2:0]  */
#define LP_ACCEL_ODR       (0x1E) /* LPOSC_CLKSEL      [3:0]  */
#define WOM_THR            (0x1F)
#define FIFO_EN            (0x23) /* TEMP_FIFO_EN      [7]
									 GYRO_XOUT         [6]
									 GYRO_YOUT         [5]
									 GYRO_ZOUT         [4]
									 ACCEL             [3]
									 SLV2              [2]
									 SLV1              [1]
									 SLV0              [0]    */
#define I2C_MST_CTRL       (0x24) /* MULT_MST_EN       [7]
									 WAIT_FOR_ES       [6]
									 SLV3_FIFO_EN      [5]
									 I2C_MST_P_NSR     [4]
									 I2C_MST_CLK       [3:0]  */
#define I2C_SLV0_ADDR      (0x25)
#define I2C_SLV0_REG       (0x26)
#define I2C_SLV0_CTRL      (0x27)
#define I2C_SLV1_ADDR      (0x28)
#define I2C_SLV1_REG       (0x29)
#define I2C_SLV1_CTRL      (0x2A)
/* ... more I2C addrs ...*/
#define I2C_MST_STATUS     (0x36)
#define INT_PIN_CFG        (0x27) /* ACTL              [7]
									 OPEN              [6]
									 LATCH_INT_EN      [5]
									 INT_ANYRD_2CLEAR  [4]
									 ACTL_FSY_EN       [3]
									 FSYNC_INT_MODE_EN [2]
									 BYPASS_EN         [1]    */
#define INT_ENABLE         (0x38) /* WOM_EN            [6]
									 FIFO_OFLOW_EN     [4]
									 FSYNC_INT_EN      [3]
									 RAW_RDY_EN        [0]    */
#define INT_STATUS         (0x3A) /* WOM_INT           [6]
									 FIFO_OFLOW_INT    [4]
									 FSYNC_INT         [3]
									 DMP_INT           [1]
									 RAW_DATA_RDY_INT  [0]    */
#define ACCEL_XOUT_H       (0x3B)
#define ACCEL_XOUT_L       (0x3C)
#define ACCEL_YOUT_H       (0x3D)
#define ACCEL_YOUT_L       (0x3E)
#define ACCEL_ZOUT_H       (0x3F)
#define ACCEL_ZOUT_L       (0x40)
#define TEMP_OUT_H         (0x41)
#define TEMP_OUT_L         (0x42)
#define GYRO_XOUT_H        (0x43)
#define GYRO_XOUT_L        (0x44)
#define GYRO_YOUT_H        (0x45)
#define GYRO_YOUT_L        (0x46)
#define GYRO_ZOUT_H        (0x47)
#define GYRO_ZOUT_L        (0x48)
#define EXT_SENS_DATA_00   (0x49)
#define EXT_SENS_DATA_01   (0x4A)
#define EXT_SENS_DATA_02   (0x4B)
#define EXT_SENS_DATA_03   (0x4C)
#define EXT_SENS_DATA_04   (0x4D)
#define EXT_SENS_DATA_05   (0x4E)
#define EXT_SENS_DATA_06   (0x4F)
#define EXT_SENS_DATA_07   (0x50)
#define EXT_SENS_DATA_08   (0x51)
#define EXT_SENS_DATA_09   (0x52)
#define EXT_SENS_DATA_10   (0x53)
#define EXT_SENS_DATA_11   (0x54)
#define EXT_SENS_DATA_12   (0x55)
#define EXT_SENS_DATA_13   (0x56)
#define EXT_SENS_DATA_14   (0x57)
#define EXT_SENS_DATA_15   (0x58)
#define EXT_SENS_DATA_16   (0x59)
#define EXT_SENS_DATA_17   (0x5A)
#define EXT_SENS_DATA_18   (0x5B)
#define EXT_SENS_DATA_19   (0x5C)
#define EXT_SENS_DATA_20   (0x5D)
#define EXT_SENS_DATA_21   (0x5E)
#define EXT_SENS_DATA_22   (0x5F)
#define EXT_SENS_DATA_23   (0x60)
#define I2C_SLV0_DO        (0x63)
#define I2C_SLV1_DO        (0x64)
#define I2C_SLV2_DO        (0x65)
#define I2C_SLV3_DO        (0x66)
#define I2C_MST_DELAY_CTRL (0x67) /* DELAY_ES_SHADOW   [7]
									 I2C_SLV4_DLY_EN   [4]
									 I2C_SLV3_DLY_EN   [3]
									 I2C_SLV2_DLY_EN   [2]
									 I2C_SLV1_DLY_EN   [1]
									 I2C_SLV0_DLY_EN   [0]    */
#define SIGNAL_PATH_REST   (0x68) /* GYRO_RST          [2]
									 ACCEL_RST         [1]
									 TEMP_RST          [0]    */
#define ACCEL_INTEL_CTRL   (0x69) /* ACCEL_INT_EL_EN   [7]
									 ACCEL_INT_EL_MODE [6]    */
#define USER_CTRL          (0x6A) /* DMP_EN            [7]
									 FIFO_EN           [6]
									 I2C_MST_EN        [5]
									 I2C_IF_DIS        [4]
									 DMP_RST           [3]
									 FIFO_RST          [2]
									 I2C_MST_RST       [1]
									 SIG_COND_RST      [0]    */
#define PWR_MGMT_1         (0x6B) /* DEVICE_RESET      [7]
									 SLEEP             [6]
									 CYCLE             [5]
									 GYRO_STANDBY      [4]
									 TEMP_DIS          [3]
									 CLKSEL            [2:0]  */
#define PWR_MGMT_2         (0x6C) /* DIS_XA            [5]
									 DIS_YA            [4]
									 DIS_ZA            [3]
									 DIS_XG            [2]
									 DIS_YG            [1]
									 DIS_ZG            [0]    */
#define FIFO_COUNT_H       (0x72) /* FIFO_CNT[12:8]    [4:0]  */
#define FIFO_COUNT_L       (0x73) /* FIFO_CNT[7:0]     [7:0]  */
#define FIFO_R_W           (0x74)
#define WHO_AM_I           (0x75)
#define XA_OFFSET_H        (0x77)
#define XA_OFFSET_L        (0x78)
#define YA_OFFSET_H        (0x7A)
#define YA_OFFSET_L        (0x7B)
#define ZA_OFFSET_H        (0x7D)
#define ZA_OFFSET_L        (0x7E)

/* Public Prototypes */
int MPU6500_Init(MPU6500_HandleTypeDef *dev);
int MPU6500_Read(MPU6500_HandleTypeDef *dev, uint8_t *data);
int MPU6500_Write(MPU6500_HandleTypeDef *dev, uint8_t *data);
int MPU6500_Ioctl(MPU6500_HandleTypeDef *dev, uint8_t type, uint8_t *data);

/* Private Prototypes */


/* Public Functions */


/* Private Functions */

