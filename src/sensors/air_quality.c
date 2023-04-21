#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

void main(void)
{
	int quality;
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};


	while (1) {
		quality = adc_read(adc_channels.dev, &sequence);
		if (quality >= 0)// if a valid data returned.
		{
        	if (quality==0)
            	printk("High pollution! Force signal active");
        	else if (quality==1)
            	printk("High pollution!");
        	else if (quality==2)
            	printk("Low pollution!");
        	else if (quality==3)
            	printk("Fresh air");
    	}
	k_sleep(K_MSEC(1000));
	}
}

