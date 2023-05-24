#include <zephyr/device.h>
#include <zephyr/toolchain.h>

/* 1 : /soc/reset-controller@4000c000:
 * Supported:
 *    - /soc/uart@40034000
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_7[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, 2, DEVICE_HANDLE_ENDS };

/* 2 : /soc/uart@40034000:
 * Direct Dependencies:
 *    - /soc/reset-controller@4000c000
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_44[] = { 1, DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 3 : /soc/gpio@40014000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_11[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 4 : /soc/i2c@40048000:
 * Supported:
 *    - /soc/i2c@40048000/ec2023@17
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_54[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, 6, DEVICE_HANDLE_ENDS };

/* 5 : /soc/i2c@40044000:
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_38[] = { DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };

/* 6 : /soc/i2c@40048000/ec2023@17:
 * Direct Dependencies:
 *    - /soc/i2c@40048000
 */
const Z_DECL_ALIGN(device_handle_t) __attribute__((__section__(".__device_handles_pass2")))
__devicehdl_dts_ord_55[] = { 4, DEVICE_HANDLE_SEP, DEVICE_HANDLE_SEP, DEVICE_HANDLE_ENDS };
