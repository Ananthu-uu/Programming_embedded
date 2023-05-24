/********   Sensor EC2023  ***********/


#define DT_DRV_COMPAT ec2023

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(EC2023, CONFIG_SENSOR_LOG_LEVEL);

#define EC2023_TARGET_ADDRESS      0x07
// #define EC2023_LEN_FIELD    12
#define EC2023_LEN_FIELD    7

struct ec2023_data {
	int16_t soil_moisture;
    int16_t air_quality;
    int32_t temperature;
};

struct ec2023_config {
	struct i2c_dt_spec i2c;
};


static inline int ec2023_reg_read(const struct ec2023_config *cfg, uint8_t *buf, uint32_t size)
{
	return i2c_read_dt(&cfg->i2c, buf, size);
}

static inline int ec2023_fetch_all(const struct ec2023_config *cfg, struct ec2023_data *data)
{
	int ret;
	uint8_t data_read[EC2023_LEN_FIELD];
	int16_t soil;
    uint8_t air;
	int32_t temp;

	ret = ec2023_reg_read(cfg, data_read, sizeof(data_read));
	if (ret < 0) {
		LOG_ERR("Could not fetch temperature [%d]", ret);
		return -EIO;
	}
	
	soil = (int16_t)(((int16_t)data_read[0] << 8) | data_read[1]);
	air = (uint8_t)data_read[2];
	// air = (int16_t)(((int16_t)data_read[2] << 8) | data_read[3]);
	temp = (int32_t)((int32_t)data_read[3] << 24) | ((int32_t)data_read[4] << 16) | ((int16_t)data_read[5] << 8) | (data_read[6]);

	data->soil_moisture = soil;
	data->air_quality = air;
	data->temperature = temp;
	return 0;
}

static int ec2023_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct ec2023_data *data = dev->data;
	const struct ec2023_config *cfg = dev->config;

    __ASSERT_NO_MSG(chan == SENSOR_CHAN_ALL);

	return ec2023_fetch_all(cfg, data);
}

static int ec2023_channel_get(const struct device *dev, enum sensor_channel chan, struct sensor_value *val)
{
	struct ec2023_data *data = dev->data;

	switch (chan) {
	case SENSOR_CHAN_AMBIENT_TEMP:
		val->val1 = data->temperature / 100;
		val->val2 = (data->temperature % 100) * 1000;
		return 0;
	case SENSOR_CHAN_HUMIDITY:
		val->val1 = data->soil_moisture;
		val->val2 = 0;
		return 0;
	case SENSOR_CHAN_VOC:
		val->val1 = data->air_quality;
		val->val2 = 0;
		return 0;
	default:
		return -ENOTSUP;
	}
}

static const struct sensor_driver_api ec2023_driver_api = {
	.sample_fetch = ec2023_sample_fetch,
	.channel_get = ec2023_channel_get,
};

int ec2023_init(const struct device *dev)
{
	const struct ec2023_config *cfg = dev->config;

	if (device_is_ready(cfg->i2c.bus)) {
		return 0;
	}
	
	return -ENODEV;
}

#define EC2023_INST(inst)                                             \
static struct ec2023_data ec2023_data_##inst;                           \
static const struct ec2023_config ec2023_config_##inst = {              \
	.i2c = I2C_DT_SPEC_INST_GET(inst),                          \
};                                                                  \
SENSOR_DEVICE_DT_INST_DEFINE(inst, ec2023_init, NULL, &ec2023_data_##inst,     \
		      &ec2023_config_##inst, POST_KERNEL,             \
		      CONFIG_SENSOR_INIT_PRIORITY, &ec2023_driver_api);

DT_INST_FOREACH_STATUS_OKAY(EC2023_INST)
