#include <zephyr/types.h>
#include <inttypes.h>

#ifndef _BME680_H_
#define _BME680_H_

/* BME680 REGISTERS */
#define BME680_ADDR         0x77           //ADDRESS OF THE SENSOR
#define BME680_STATUS       0x73
#define BME680_CONFIG       0x75
#define BME680_CTRL_MEAS    0x74
#define BME680_CTRL_HUM     0x72 
#define BME680_HUM_LSB      0x26
#define BME680_HUM_MSB      0x25 
#define BME680_TEMP_XLSB    0x24
#define BME680_TEMP_LSB     0x23 
#define BME680_TEMP_MSB     0x22 
#define BME680_EAS_STATUS_0 0x1D
#define BME680_TEMP_PAR1    0xE9
#define BME680_TEMP_PAR2    0x8A 
#define CONFIG_VALUE        (2<<2)         //FILTER COEFFICIENT = 15
#define TEMP_ENABLE         ((2<<5)|(1<<0))   //ENABLE TEMP BITS AND FORCED MODE
#define HUM_OVERSAMPLE      2               // HUMIDITY OVERSAMPLE 2

#define I2C_TARGET_ADDRESS  0x17                  //ADDRESS OF THE SLAVE 


#endif /* _BME680_H_ */
