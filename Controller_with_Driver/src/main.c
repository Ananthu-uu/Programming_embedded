/*       
 *
 * **********************          CONTROLLER DEVICE       **************************
 * 
 *  An interrupt occures on GPIO20 whenever soil moisture reading is dry
 *  An led on GPIO10 indicates the interrupt
 *  A status led keeps blinking ensuring the workflow
 *  
 * --------------------------------------------------
 * 
 * 	Editors : Anantha Krishnan
 *           Shalu Prakasi
 *           Shweta Rajasekhar
 * --------------------------------------------------
 * Sensor value Range
 * 0	~	300	- Fresh air             aq = 0      
 * 300	~	700	- Low pollution         aq = 1
 * > 700		- High pollution        aq = 2
 * --------------------------------------------------
 *
 * 
*/
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>
#include <stdio.h>

#define MOIST_HIGH  800
#define MOIST_LOW   350

#define STAT_LED		DT_ALIAS(statled)               // 11
#define AQ_LED			DT_ALIAS(aqled)                 //12
#define SOIL_LED    	DT_ALIAS(ledsoil)           //LED FOR INTERRUPT INDICATOR 10
#define SOIL_INTR   	DT_ALIAS(pinled)           //LED FOR INTERRUPT 20
#define STACK_SIZE  	500
#define AQDELAYHIGH		150
#define AQDELAYLOW		800			

struct gpio_dt_spec status = GPIO_DT_SPEC_GET(STAT_LED, gpios); 
struct gpio_dt_spec aq_led = GPIO_DT_SPEC_GET(AQ_LED, gpios); 
struct gpio_dt_spec soil_intr = GPIO_DT_SPEC_GET(SOIL_INTR, gpios); 
struct gpio_dt_spec soil_led = GPIO_DT_SPEC_GET(SOIL_LED, gpios); 
static struct gpio_callback soil_cb_data;

bool moisture_flag;

int aq_delay;

K_SEM_DEFINE(sem, 0, 1);
K_MUTEX_DEFINE(mutex);

void response_isr() {
    k_sem_give(&sem);
}

void main_task(void)
{
    int ret;
	gpio_pin_configure_dt(&status, GPIO_OUTPUT);
	gpio_pin_configure_dt(&aq_led, GPIO_OUTPUT);

	const struct device *const dev = DEVICE_DT_GET_ONE(ec2023);
	struct sensor_value temp, moisture, air_qual;

	if (!device_is_ready(dev)) {
		printk("sensor: device not ready.\n");
		return;
	}
	while (1) {
		gpio_pin_toggle_dt(&status);				//Status led just toggles each loop

		ret = sensor_sample_fetch(dev);
        if(ret < 0){
            printk("Error");
        }
		sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &moisture);
		sensor_channel_get(dev, SENSOR_CHAN_VOC, &air_qual);
		
		printk("Temperature\t: %d.%06d\t*\nSoil Moisture\t: %d\t\t*\nAQ index\t: %d\t\t*\n",temp.val1, temp.val2, moisture.val1, air_qual.val1);
		printk("*********************************\n");
		if(moisture.val1 > MOIST_LOW && moisture.val1 < MOIST_HIGH)
        {
            k_mutex_lock(&mutex, K_FOREVER);
            moisture_flag = 0;
			k_mutex_unlock(&mutex);
        }

		if(air_qual.val1 == 2){
			gpio_pin_set_dt(&aq_led, 1);
		}
		else{
			gpio_pin_set_dt(&aq_led, 0);
		}
		k_sleep(K_MSEC(3000));
	}
}

// For interrupt 
void trigger_task(){
    gpio_pin_configure_dt(&soil_intr, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&soil_intr, GPIO_INT_EDGE_FALLING);

    gpio_init_callback(&soil_cb_data, response_isr, BIT(soil_intr.pin));
    gpio_add_callback(soil_intr.port, &soil_cb_data);

    while(1){
        k_sem_take(&sem, K_FOREVER);
        // printk("\nInterrupt");
        k_mutex_lock(&mutex, K_FOREVER);
        moisture_flag = 1;
        k_mutex_unlock(&mutex);
    }

}

void led_task(){
	bool f;
    gpio_pin_configure_dt(&soil_led, GPIO_OUTPUT);

    while (1)
    {
        k_mutex_lock(&mutex, K_FOREVER);
        f = moisture_flag;
        k_mutex_unlock(&mutex);
        if(f){
            // printk("\ntoggle led");
            gpio_pin_toggle_dt(&soil_led);
        }
        else{
            // printk("\noff led");
            gpio_pin_set_dt(&soil_led,0);
        } 
        k_msleep(250);  
    }
}

K_THREAD_DEFINE(thread1_id, STACK_SIZE, main_task, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread2_id, STACK_SIZE, led_task, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(thread3_id, STACK_SIZE, trigger_task, NULL, NULL, NULL, 5, 0, 0);
