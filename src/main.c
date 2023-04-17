
//Controlled Environmental System

// Authors 
/*
    Anantha Krishnan
    Shweta Rajasekhar
    Shalu Prakasia
*/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#include <bme680_reg.h>





#define I2C_LABEL   i2c0

#define MY_STACK_SIZE   5000
// #if DT_NODE_HAS_STATUS(DT_ALIAS(i2c_0), okay)
// #define I2C_DEV_NODE	DT_ALIAS(i2c_0)
// #elif DT_NODE_HAS_STATUS(DT_ALIAS(i2c_1), okay)
// #define I2C_DEV_NODE	DT_ALIAS(i2c_1)
// #elif DT_NODE_HAS_STATUS(DT_ALIAS(i2c_2), okay)
// #define I2C_DEV_NODE	DT_ALIAS(i2c_2)
// #else
// #error "Please set the correct I2C device"
// #endif
//uint32_t i2c_cfg = I2C_SPEED_SET(I2C_SPEED_STANDARD) | I2C_MODE_CONTROLLER;
const struct device * i2c_dev = DEVICE_DT_GET(DT_NODELABEL(I2C_LABEL));


int main(void)
{
    printk("I'm in main\n");
    
    i2c_reg_write_byte(i2c_dev, BME680_ADDR, BME680_CTRL_MEAS ,0b01000001);    // CONFIGURING THE TEMP SENSOR  
    i2c_reg_write_byte(i2c_dev, BME680_ADDR, BME680_CTRL_HUM ,0b00000001);     //configuring the humidity sensor     
    //CONFIGURING THE IIR filter
    i2c_reg_write_byte(i2c_dev, BME680_ADDR, BME680_CONFIG ,0b00001000);   

    //temperature variables 
    uint8_t temp_buf[3];
    int32_t temp_adc;
    int32_t tmp[3];
    int32_t var1, var2, var3;
    uint8_t par_t1_lsb,par_t1_msb, par_t2_lsb,par_t2_msb,par_t3;
    int32_t par_t1, par_t2, t11, t12, t21, t22;

    
    //temperature

    i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_T1_LSB, (&par_t1_lsb));
    printk("par t1 LSB: %d\n",par_t1_lsb);
    i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_T1_MSB, (&par_t1_msb));
    printk("par t1 MSB: %d\n",par_t1_msb);
    i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_T2_LSB, (&par_t2_lsb));
    printk("par t2 LSB: %d\n",par_t2_lsb);
    i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_T2_MSB, (&par_t2_msb));
    printk("par t1 MSB: %d\n",par_t2_msb);
    i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_T3, (&par_t3));
    t11 = (int32_t)(par_t1_lsb);
    t12 = (int32_t)(par_t1_msb);
    //printk("I'm in part1\n");
    par_t1 = t11 | (t12 << 8);
    //printk("par t1: %d\n", par_t1);
    t21 = (int32_t)(par_t2_lsb);
    t22 = (int32_t)(par_t2_msb);
    par_t2 = t21 | (t22<<8);
    //printk("par t2: %d\n", par_t2);

    //humidity variables
    uint8_t hum_msb,hum_lsb;
    int16_t hum;
    uint8_t par_h1_lsb, par_h1_msb, par_h2_lsb, par_h2_msb, par_h3, par_h4, par_h5, par_h6, par_h7, hum_adc_lsb, hum_adc_msb;
    int32_t par_h1, par_h2;
    int16_t hum_adc;
    int32_t temp_scaled;
    int32_t v1,v2,v3,v4,v5,v6, hum_comp;

    //humidity
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H1_LSB, (&par_h1_lsb));
        printk("par h1 lsb: %d\n",par_h1_lsb);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H1_MSB, (&par_h1_msb));
        printk("par h1 msb: %d\n",par_h1_msb);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H2_MSB, (&par_h2_msb));
        printk("par h2 msb: %d\n",par_h2_msb);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H3, (&par_h3));
        printk("par h3 : %d\n",par_h3);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H4, (&par_h4));
        printk("par h4 : %d\n",par_h4);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H5, (&par_h5));
        printk("par h5 : %d\n",par_h5);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H6, (&par_h6));
        printk("par h6 : %d\n",par_h6);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_PAR_H7, (&par_h7));
        printk("par h7 : %d\n",par_h7);
        par_h1 = (par_h1_lsb && 0b00001111) || ((int32_t)par_h1_msb << 4);
        par_h2 = (par_h1_lsb>>4) || ((int32_t)par_h2_msb << 4);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_HUM_ADC_LSB, (&hum_adc_lsb));
        printk("hum_adc_lsb : %d\n",hum_adc_lsb);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_HUM_ADC_MSB, (&hum_adc_msb));
        printk("hum_adc_msb : %d\n",hum_adc_msb);
        hum_adc = (hum_adc_lsb) || ((int16_t)hum_adc_msb << 8);
    while(1)
    {
        //temperature
        i2c_reg_write_byte(i2c_dev, BME680_ADDR, BME680_CTRL_MEAS ,0b01000001);  
        printk("I'm in loop\n");
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_TEMP_XLSB, temp_buf);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_TEMP_LSB, temp_buf+1);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_TEMP_MSB, temp_buf+2);
        //humidity
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_HUM_LSB, (&hum_lsb));
        printk("hum LSB: %d\n",hum_lsb);
        i2c_reg_read_byte(i2c_dev, BME680_ADDR, BME680_HUM_MSB, (&hum_msb));
        printk("hum MSB: %d\n",hum_msb);
        
    //temperature calculation
    // type casting to 32 bit
    tmp[0] = (int32_t)(temp_buf[0])>>4;
    tmp[1] = (int32_t)(temp_buf[1])<<4;
    tmp[2] = (int32_t)(temp_buf[2])<<12;
    temp_adc = (tmp[0]|tmp[1]|tmp[2]);
    //printk("temp adc: %d\n",temp_adc);

    var1 = ((int32_t)temp_adc >>3) - ((int32_t)par_t1 << 1);
    var2 = (var1 *(int32_t)par_t2) >> 11;
    var3 = ((((var1>>1)*(var1 >> 1))>>12)*((int32_t)par_t3 << 4))>> 14;
    int32_t t_fine =  var2 + var3 ;
    int32_t temp_comp = (int32_t)((t_fine*5)+ 128)>> 8;
    printk("Temperature: %d.%06d \n", (temp_comp/100), ((temp_comp%100)*1000));

    k_sleep(K_MSEC(5000));


    //humidity calculation
    temp_scaled = (int32_t)temp_comp;
    v1 = (int32_t)hum_adc - (int32_t)((int32_t)par_h1 << 4) - (((temp_scaled * (int32_t)par_h3)/((int32_t)100)) >> 1);
    v2 = ((int32_t)par_h2 * (((temp_scaled *
    (int32_t)par_h4) / ((int32_t)100)) +
    (((temp_scaled * ((temp_scaled * (int32_t)par_h5) /
    ((int32_t)100))) >> 6) / ((int32_t)100)) + ((int32_t)(1 << 14)))) >> 10;
    v3 = v1 * v2;
    v4 = (((int32_t)par_h6 << 7) +
    ((temp_scaled * (int32_t)par_h7) / ((int32_t)100))) >> 4; v5 = ((v3 >> 14) * (v3 >> 14)) >> 10;
    v6 = (v4 * v5) >> 1;
    hum_comp = (((v3 + v6) >> 10) * ((int32_t) 1000)) >> 12;
    printk("Humidity: %d\n", hum_comp);

    }
    printk("I'm done\n");
}
