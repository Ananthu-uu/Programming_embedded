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
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

void main(void)
{
	int16_t quality;

	while (1) {
		quality = adc_read(adc_channels.dev, adc_sequence);
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

