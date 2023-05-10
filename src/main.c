/*
 * **********	SENSORS		***********
 * 	-> BME680
 * 	-> SEN0114
 * 	-> AIR QUALITY SENSOR V1.3
 * 
 * --------------------------------------------------
 * 
 * 	Editor : Shalu Prakasia
 *           Anantha Krishnan
 *           Shweta Rajasekhar
 * 
 * --------------------------------------------------
 * Connect the soil moisture sensor to ADC0 (GPIO 26) 
 * Sensor value Range
 * 0 	~	300	 - dry soil
 * 300 	~	700  - humid soil
 * 700	~	1023 - water
* --------------------------------------------------
 * Connect the air quality sensor to ADC1 (GPIO 27)
 * Sensor value Range
 * 0	~	300	- Fresh air 
 * 300	~	700	- Low pollution
 * > 700		- High pollution
* --------------------------------------------------
 * BME680 connected to I2C0
 * 
*/

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#include <bme680_reg.h>

#define CONFIG_VALUE        (2<<2)         //FILTER COEFFICIENT = 15
#define TEMP_ENABLE         ((2<<5)|(1<<0))   //ENABLE TEMP BITS AND FORCED MODE
#define HUM_OVERSAMPLE      2               // HUMIDITY OVERSAMPLE 2

#define BME680_ADDR     0x77           //ADDRESS OF THE SENSOR

#define I2C_NODE    DT_NODELABEL(i2c0) 	

#define STACK_SIZE  500 

static const struct adc_dt_spec soil_moisture_chan0 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
static const struct adc_dt_spec air_quality_chan1 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);

int16_t soilbuf; 
struct adc_sequence soil_sequence = {
	.buffer = &soilbuf,
	.buffer_size = sizeof(soilbuf),
};

int16_t airbuf;
struct adc_sequence air_sequence = {
	.buffer = &airbuf,
	.buffer_size = sizeof(airbuf),
};


int32_t hum_comp;
int32_t temp_comp;

// Thread task to read the adc channel
void adc_task(struct adc_dt_spec* chan, struct adc_sequence* seq)
{
	int err;
	// check if the sensor device is ready
	if (!device_is_ready(chan->dev)) {
		printk("ADC controller device not ready\n");
		return;
	}
	/* Configure channel0 according to the device tree, before reading. */
	err = adc_channel_setup_dt(chan);
	if (err < 0) {
		printk("Could not setup channel\n");
		return;
	}
	while (1) {
		(void)adc_sequence_init_dt(chan, seq);
		err = adc_read(chan->dev, seq);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		} 
		// else {
		// 	// printk("\nChannel %d: ",chan->channel_id);
		// }
		k_sleep(K_MSEC(3000));
	}
}

void temp_sensor_task()
{
	int ret;
    uint8_t buffer[20];

    int32_t var1, var1_h, var2_1, var2_2, var2_h, var3_h, var2, var3, var4, var5, var6;
    
    int32_t par_t1, par_t2, par_t3;
    int32_t t_fine, adc_temp;

    uint16_t par_h1, par_h2;
    int8_t par_h3, par_h4, par_h5, par_h7;
    uint8_t par_h6;

    uint16_t hum_adc;

    const struct device *const dev1 = DEVICE_DT_GET(I2C_NODE);

/*******************   DEVICE READY CHECK   **************************/
    if (!device_is_ready(dev1)) {
		printk("Bus %s not ready\n", dev1->name);
		return;
	}
/*********************************************************************/

/*******************   INITIALIZATION   ******************************/    

    // 1. Configure the sensor register 0x75 with filter coefficient
    ret = i2c_reg_write_byte(dev1, BME680_ADDR, BME680_CONFIG, CONFIG_VALUE);
    if(ret<0){
        // printk("\nError!! ");
        return;
    }

    // 2. Configure the register 0x74(Temperature oversampling) to measure temperature
    ret = i2c_reg_write_byte(dev1, BME680_ADDR, BME680_CTRL_MEAS, TEMP_ENABLE);
    if(ret<0){
        // printk("\nError!! ");
        return;
    }
    // 3. Configure the register 0x75(Humidity oversampling)
    ret = i2c_reg_write_byte(dev1, BME680_ADDR, BME680_CTRL_HUM, HUM_OVERSAMPLE);
    if(ret<0){
        // printk("\nError!! ");
        return;
    }

/*******************************************************************************************************/

/******* CALIBRATION PARAMETERS ********************/ 
    ret = i2c_burst_read(dev1, BME680_ADDR, BME680_PAR, buffer, 10);
    if(ret<0){
    //    printf("\n Error reading calibration parameter");
    }
    else{
        par_h1 = (uint16_t)(((uint16_t)buffer[2] << 4) | (buffer[1] & 0x0F));
        par_h2 = (uint16_t)(((uint16_t)buffer[0] << 4) | (buffer[1] >> 4));
        par_h3 = (int8_t)buffer[3];
        par_h4 = (int8_t)buffer[4];
        par_h5 = (int8_t)buffer[5];
        par_h6 = (uint8_t)buffer[6];
        par_h7 = (int8_t)buffer[7];

        par_t1 = (int32_t)(((uint16_t)buffer[9]) << 8) | (uint16_t)buffer[8]; 
    } 
    ret = i2c_burst_read(dev1, BME680_ADDR, BME680_TEMP_PAR2, (buffer+10), 3);
    if(ret<0){
       printf("\n Error reading calibration parameter");
    }
    else{
        par_t2 = (int32_t)(((int16_t)buffer[11]) << 8) | (int16_t)buffer[10];
        par_t3 = (int32_t)buffer[12];
    }

/*******************************************************************************************************/

while (1)
    {
        k_sleep(K_MSEC(3000));

/*******                READ NEW DATA BIT               *********************/
        ret = i2c_reg_read_byte(dev1, BME680_ADDR, BME680_EAS_STATUS_0, (buffer+13));
        if(buffer[13] & 0x80){
/********           READ TEMP (XLSB LSB MSB) AND HUMIDITY (MSB AND LSB)    *******************/ 
            ret = i2c_burst_read(dev1, BME680_ADDR, BME680_TEMP_MSB, (buffer+14), 5);
            if(ret<0){
                // printf("\n Error reading temperature data");
            }
            else{
                // printk("\nHUM_MSB = %d \tHUM_LSB = %d",buffer[17], buffer[18]);
            }
/*******               (xlsb (7 to 4 bits hence right shift))    ***************/ 
            adc_temp = (((int32_t)buffer[16])>>4) | (((int32_t)buffer[15])<<4) | (((int32_t)buffer[14])<<12);
            hum_adc = (((uint16_t)buffer[17] << 8) | (uint16_t)buffer[18]);
            // printk("\nHUMIDITY_ADC RAW VALUE IS %d", hum_adc);

/*******************   TEMPERATURE CALCULATION   ******************************/  
            var1 = ((int32_t)adc_temp>>3) - ((int32_t)par_t1 << 1);
            var2 = (var1 * (int32_t)par_t2) >> 11;
            var3 = (int32_t)(((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14);
            t_fine = (int32_t)(var2 + var3);
            temp_comp = (int32_t)(((t_fine * 5) + 128 ) >> 8);
/*******************************************************************************************************/

/*******************   HUMIDITY CALCULATION   ******************************/  
            var1_h = (int32_t)hum_adc - (int32_t)((int32_t)par_h1 << 4) - ((temp_comp * (int32_t)par_h3) / ((int32_t)100) >> 1);
            var2_1 = (int32_t)par_h2;
            var2_2 = ((temp_comp * (int32_t)par_h4) / (int32_t)100) + (((temp_comp * ((temp_comp * (int32_t)par_h5) / ((int32_t)100))) >> 6) / ((int32_t)100)) +  (int32_t)(1 << 14);
            var2_h = (var2_1 * var2_2) >> 10;
            var3_h = var1_h * var2_h;
            var4 = (((int32_t)par_h6 << 7) + ((temp_comp * (int32_t)par_h7) / ((int32_t)100))) >> 4;
            var5 = ((var3_h >> 14) * (var3_h >> 14)) >> 10;
            var6 = (var4 * var5) >> 1;
            hum_comp = (((var3_h + var6) >> 10) * ((int32_t)1000)) >> 12;
            
/*******************************************************************************************************/
        }

        else{
            // printk("\nNo new data at the moment!! %x", buffer[0]);
        }
/*******            TRIGGER NEXT MEASUREMENT              ********************/         
        ret = i2c_reg_write_byte(dev1, BME680_ADDR, BME680_CTRL_MEAS, TEMP_ENABLE);
        if(ret<0){
            // printk("\nError!! ");
            return;
        }
    }

}

// Display task
void display_task()
{
	while(1){
/*******************************************************************************************************/
        printk("\n*************************************");
		printk("\nSoil moisture Channel %d value = %"PRId16, soil_moisture_chan0.channel_id, soilbuf);
		printk("\nAir quality Channel %d value = %"PRId16, air_quality_chan1.channel_id, airbuf);
        printk("\nTemperature :: %d.%06d degC", (temp_comp / 100), ((temp_comp%100)*1000));
        printk("\nHumidity :: %d.%06d",(hum_comp / 1000),((hum_comp % 1000)*1000));
/*******************************************************************************************************/
		k_sleep(K_MSEC(5000));
	}
}

// Thread define
K_THREAD_DEFINE(thread1_id, STACK_SIZE, adc_task, &soil_moisture_chan0, &soil_sequence, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread2_id, STACK_SIZE, adc_task, &air_quality_chan1, &air_sequence, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread3_id, STACK_SIZE, display_task, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread4_id, STACK_SIZE, temp_sensor_task, NULL, NULL, NULL, 5, 0, 0);




