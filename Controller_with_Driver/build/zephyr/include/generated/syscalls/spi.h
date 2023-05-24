/* auto-generated by gen_syscalls.py, don't edit */

#ifndef Z_INCLUDE_SYSCALLS_SPI_H
#define Z_INCLUDE_SYSCALLS_SPI_H


#include <zephyr/tracing/tracing_syscall.h>

#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <zephyr/syscall.h>

#include <zephyr/linker/sections.h>


#ifdef __cplusplus
extern "C" {
#endif

extern int z_impl_spi_transceive(const struct device * dev, const struct spi_config * config, const struct spi_buf_set * tx_bufs, const struct spi_buf_set * rx_bufs);

__pinned_func
static inline int spi_transceive(const struct device * dev, const struct spi_config * config, const struct spi_buf_set * tx_bufs, const struct spi_buf_set * rx_bufs)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		union { uintptr_t x; const struct spi_config * val; } parm1 = { .val = config };
		union { uintptr_t x; const struct spi_buf_set * val; } parm2 = { .val = tx_bufs };
		union { uintptr_t x; const struct spi_buf_set * val; } parm3 = { .val = rx_bufs };
		return (int) arch_syscall_invoke4(parm0.x, parm1.x, parm2.x, parm3.x, K_SYSCALL_SPI_TRANSCEIVE);
	}
#endif
	compiler_barrier();
	return z_impl_spi_transceive(dev, config, tx_bufs, rx_bufs);
}

#if defined(CONFIG_TRACING_SYSCALL)
#ifndef DISABLE_SYSCALL_TRACING

#define spi_transceive(dev, config, tx_bufs, rx_bufs) ({ 	int retval; 	sys_port_trace_syscall_enter(K_SYSCALL_SPI_TRANSCEIVE, spi_transceive, dev, config, tx_bufs, rx_bufs); 	retval = spi_transceive(dev, config, tx_bufs, rx_bufs); 	sys_port_trace_syscall_exit(K_SYSCALL_SPI_TRANSCEIVE, spi_transceive, dev, config, tx_bufs, rx_bufs, retval); 	retval; })
#endif
#endif


extern int z_impl_spi_release(const struct device * dev, const struct spi_config * config);

__pinned_func
static inline int spi_release(const struct device * dev, const struct spi_config * config)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		union { uintptr_t x; const struct device * val; } parm0 = { .val = dev };
		union { uintptr_t x; const struct spi_config * val; } parm1 = { .val = config };
		return (int) arch_syscall_invoke2(parm0.x, parm1.x, K_SYSCALL_SPI_RELEASE);
	}
#endif
	compiler_barrier();
	return z_impl_spi_release(dev, config);
}

#if defined(CONFIG_TRACING_SYSCALL)
#ifndef DISABLE_SYSCALL_TRACING

#define spi_release(dev, config) ({ 	int retval; 	sys_port_trace_syscall_enter(K_SYSCALL_SPI_RELEASE, spi_release, dev, config); 	retval = spi_release(dev, config); 	sys_port_trace_syscall_exit(K_SYSCALL_SPI_RELEASE, spi_release, dev, config, retval); 	retval; })
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif
#endif /* include guard */
