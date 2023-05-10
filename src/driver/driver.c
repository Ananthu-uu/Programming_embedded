
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

#define RPI_PICO 0x07

#define DT_DRV_COMPAT rpi_pico

#define RPI_PICO_I2C DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)

union rpi_pico {
#if RPI_PICO_I2C
	struct i2c_dt_spec i2c;
#endif
};

typedef int (*rpi_pico_read)(const struct device *dev,
				  uint8_t start, uint8_t *buf, int size);

struct rpi_pico_io {
	rpi_pico_read read;
};

#if RPI_PICO_I2C
extern const struct rpi_pico_io rpi_pico_io_i2c;
#endif

struct rpi_pico_config {
	union rpi_pico bus;
	const struct rpi_pico_io *bus_io;
};


#if RPI_PICO_I2C

static int rpi_pico_read_i2c(const struct device *dev,
			       uint8_t start, uint8_t *buf, int size)
{
	const struct rpi_pico_config *config = dev->config;

	return i2c_read_dt(&config->bus.i2c, start, buf, size);
}

const struct rpi_pico_io rpi_pico_io_i2c = {
	.read = rpi_pico_read_i2c,
};
#endif /* RPI_PICO_I2C */
