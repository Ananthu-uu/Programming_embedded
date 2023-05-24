/*       
 *
 * **********************          TARGET DEVICE       **************************
 * 
 *  The Target device continuously reads the sensors and when controller 
 *  initiates read it sends all the sensor values to the controller
 * 
 *  It also generates pulse signals indicating the soil moisture level from GPIO-10
 *  which can be used as interrupt in controller
 * 
 * **********	SENSORS		*************
 * 	-> BME680
 * 	-> SEN0114
 * 	-> AIR QUALITY SENSOR V1.3
 * ***************************************
 * --------------------------------------------------
 * 
 * 	Editors : Anantha Krishnan
 *           Shalu Prakasi
 *           Shweta Rajasekhar
 * 
 * --------------------------------------------------
 * BME680 connected to I2C0 Grove3
 * Gets temperature value
 * 
 * Controller on I2C1 Grove2
 * 
 * --------------------------------------------------
 * Connect the soil moisture sensor to ADC0 (GPIO 26) 
 * Sensor value Range
 * 0 	~	400	 - dry soil
 * 400 	~	700  - humid soil
 * 700	~	1023 - water
 * --------------------------------------------------
 * Connect the air quality sensor to ADC1 (GPIO 27)
 * Sensor value Range
 * 0	~	300	- Fresh air             Send aq = 0      
 * 300	~	700	- Low pollution         Send aq = 1
 * > 700		- High pollution        Send aq = 2
 * --------------------------------------------------
 *
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
#include <sensor_target.h>


#define MOIST_HIGH  800
#define MOIST_LOW   350


#define SOIL_LED            DT_ALIAS(soilpin)           //LED FOR INTERRUPT
// #define AIR_QUALITY_LED     DT_ALIAS(airpin)           //LED FOR INTERRUPT

#define I2C_NODE            DT_NODELABEL(i2c1)          //CONTROLLER
#define I2C_NODE_SENSOR     DT_NODELABEL(i2c0)          // SENSOR

#define BUF_LEN             7

struct gpio_dt_spec soil_pin = GPIO_DT_SPEC_GET(SOIL_LED, gpios); 
// struct gpio_dt_spec air_pin = GPIO_DT_SPEC_GET(AIR_QUALITY_LED, gpios);

int ret;
uint8_t buffer[20];

uint8_t data[BUF_LEN];
uint32_t buff_idx;
bool m_read;

int32_t par_t1, par_t2, par_t3;
int32_t hum_comp;
int32_t temp_comp;

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


// read request callback
int master_read(struct i2c_target_config *config, uint8_t *val)
{
    m_read = true;
    *val = data[buff_idx];
    m_read = false;
    return 0;
};

// read processed callback for incrementing the data index
int master_read_continue(struct i2c_target_config *config, uint8_t *val)
{
    m_read = true;
    buff_idx = (buff_idx + 1) % BUF_LEN;
	*val = data[buff_idx];
    m_read = false;
    return 0;
};

// callbacks
struct i2c_target_callbacks cb = {
    .read_requested = master_read,
    .read_processed = master_read_continue,
};

// slave configurations
struct i2c_target_config slave_config = {
    .address = I2C_TARGET_ADDRESS,
    .callbacks = &cb,
};

// Temperature calibration parameter read function
static int read_calibration_parameters(const struct device *dev)
{
    ret = i2c_burst_read(dev, BME680_ADDR, BME680_TEMP_PAR1, buffer, 2);
    if(ret < 0){
       printf("\n Error reading calibration parameter");
       return ret;
    }
    else{
        par_t1 = (int32_t)(((uint16_t)buffer[1]) << 8) | (uint16_t)buffer[0]; 
    } 

    ret = i2c_burst_read(dev, BME680_ADDR, BME680_TEMP_PAR2, (buffer+2), 3);
    if(ret < 0){
       printf("\n Error reading calibration parameter");
       return ret;
    }
    else{
        par_t2 = (int32_t)(((int16_t)buffer[3]) << 8) | (int16_t)buffer[2];
        par_t3 = (int32_t)buffer[4];
    }
    return 0;
}

// Get the temperature sample reading from bme680
static int sample_read_temp(const struct device *dev)
{

    int32_t var1, var2, var3;
    int32_t t_fine, adc_temp;

    ret = i2c_reg_read_byte(dev, BME680_ADDR, BME680_EAS_STATUS_0, (buffer+5));
    if(buffer[5] & 0x80)
    {
        ret = i2c_burst_read(dev, BME680_ADDR, BME680_TEMP_MSB, (buffer+6), 3);
        if(ret<0){
            return ret;
        }
        adc_temp = (((int32_t)buffer[8])>>4) | (((int32_t)buffer[7])<<4) | (((int32_t)buffer[6])<<12);

        /*******************   TEMPERATURE CALCULATION   ******************************/  
        var1 = ((int32_t)adc_temp>>3) - ((int32_t)par_t1 << 1);
        var2 = (var1 * (int32_t)par_t2) >> 11;
        var3 = (int32_t)(((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14);
        t_fine = (int32_t)(var2 + var3);
        temp_comp = (int32_t)(((t_fine * 5) + 128 ) >> 8);
    }

    /*******            TRIGGER NEXT MEASUREMENT              ********************/         
    ret = i2c_reg_write_byte(dev, BME680_ADDR, BME680_CTRL_MEAS, TEMP_ENABLE);
    if(ret < 0){
        return ret;
    }
    ret = i2c_reg_write_byte(dev, BME680_ADDR, BME680_CTRL_HUM, HUM_OVERSAMPLE);
    if(ret < 0){
        return ret;
    }
    return 0;
}

/*  Function to initialize the adc channel
 *
 *  Configure chan0 or chan1 according to the device tree, before reading.
 *  using adc_channel_setup_dt()
 */ 
static int adc_channel_init(const struct adc_dt_spec* chan)
{
    if (!device_is_ready(chan->dev)) {
		printk("ADC controller device not ready\n");
		return ret;
	}
	ret = adc_channel_setup_dt(chan);
	if (ret < 0) {
		printk("Could not setup channel\n");
		return ret;
	}
    return ret;
}

// Function to get adc readings
static int sample_read_adc_channels(const struct adc_dt_spec* chan, struct adc_sequence* seq){
    (void)adc_sequence_init_dt(chan, seq);
	ret = adc_read(chan->dev, seq);
	if (ret < 0) {
		printk("Could not read");
        return ret;
	} 
	return 0;
}

// Send interrupt signals to the controller when soil is dry
void trigger_pins(){
    if(soilbuf > MOIST_HIGH || soilbuf < MOIST_LOW){
        printk("\n*****Immediate attention required******\n");
        gpio_pin_set_dt(&soil_pin, 0);
    }
    else{
        gpio_pin_set_dt(&soil_pin, 1);
    }
    // if(airbuf > 700){
    //     gpio_pin_set_dt(&air_pin, 0);
    // }
    // else{
    //     gpio_pin_set_dt(&air_pin, 1);
    // }
}

void main()
{
    uint8_t aq;
    gpio_pin_configure_dt(&soil_pin, GPIO_OUTPUT_HIGH); 
    // gpio_pin_configure_dt(&air_pin, GPIO_OUTPUT_HIGH); 

/*******************   DEVICES READY CHECK   **************************/
    const struct device *const target_device = DEVICE_DT_GET(I2C_NODE);
    if (!device_is_ready(target_device)) { 
        printk("error");
        return;
	}
    else{
        printk("Bus %s  ready\n", target_device->name);
    }

    const struct device *const bme_device = DEVICE_DT_GET(I2C_NODE_SENSOR);
    if (!device_is_ready(bme_device)) {
		printk("Bus %s not ready\n", bme_device->name);
		return;
	}
    else{
        printk("Bus %s ready\n", bme_device->name);
    }

/*******************   ADC CHANNEL CHECK   **************************/
    static const struct adc_dt_spec soil_moisture_chan0 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);
    static const struct adc_dt_spec air_quality_chan1 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);
    ret = adc_channel_init(&soil_moisture_chan0);
    if (ret < 0) {
		printk("Could not setup channel\n");
		return;
	}
    ret = adc_channel_init(&air_quality_chan1);
    if (ret < 0) {
		printk("Could not setup channel\n");
		return;
	}

 /*******************   REGISTER THE SLAVE ADDRESS  ******************************/  
    ret = i2c_target_register(target_device, &slave_config);
    if (ret < 0) { 
        printk("error value %d",ret); 
        return;
    }
    else{
        printk("\nSuccessfully registered!!");
    }

/******* INITIALIZATION OF BME680 SENSOR TO READ TEMP *******************/    
    // 1. Configure the sensor register 0x75 with filter coefficient
    ret = i2c_reg_write_byte(bme_device, BME680_ADDR, BME680_CONFIG, CONFIG_VALUE);
    if(ret<0){
        printk("\nError!! ");
        return;
    }

    // 2. Configure the register 0x74(Temperature oversampling) to measure temperature
    ret = i2c_reg_write_byte(bme_device, BME680_ADDR, BME680_CTRL_MEAS, TEMP_ENABLE);
    if(ret<0){
        printk("\nError!! ");
        return;
    }

/*************              CALIBRATION PARAMETERS                  ********************/ 
    ret = read_calibration_parameters(bme_device);
    if(ret < 0){
        printk("\nError reading calibration parameters");
    }

    while (1)
    {
    /*******************          READ SENSOR DATAS                 ********************/ 
        ret = sample_read_temp(bme_device);
        if(ret < 0){
            printk("\nError reading teperature");
        }
        ret = sample_read_adc_channels(&soil_moisture_chan0, &soil_sequence);
        if(ret < 0){
            printk("\nError reading moisture sensor");
        }
        ret = sample_read_adc_channels(&air_quality_chan1, &air_sequence);
        if(ret < 0){
            printk("\nError reading air quality sensor");
        }
        soilbuf = soilbuf & 0x3FF;      //Ensures 10-bit adc value

        if(airbuf > 700){
            aq = 2;     //HIGH POLLUTION
        }
        else if(airbuf > 350){
            aq = 1;     //LOW POLLUTION
        }
        else{
            aq = 0;     //FRESH AIR
        }

        printk("\nTemperature :: %d.%06d degC", (temp_comp/100), ((temp_comp%100)*1000));
		printk("\nSoil moisture Channel %d value = %"PRId16, soil_moisture_chan0.channel_id, soilbuf);
        printk("\nAir quality Channel %d value = %"PRId16, air_quality_chan1.channel_id, airbuf);

        if(!m_read){
            data[0] = (uint8_t)((soilbuf & 0xFF00) >> 8);       //soilbuf MSB
            data[1] = (uint8_t)((soilbuf & 0x00FF));            //soilbuf LSB

            data[2] = (uint8_t)aq;                              // AQ index

            data[3] = (uint8_t)((temp_comp >> 24) & 0xFF);      //temp MSB     
            data[4] = (uint8_t)((temp_comp >> 16) & 0xFF);
            data[5] = (uint8_t)((temp_comp >> 8) & 0xFF);
            data[6] = (uint8_t)(temp_comp & 0xFF);             //temp LSB
        }
        trigger_pins();
        k_sleep(K_MSEC(500));  
    }
}






