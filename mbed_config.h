//	mbed_config.h

#ifndef __MBED_CONFIG_DATA__
#define __MBED_CONFIG_DATA__

// configuration parameters
#define MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE 9600 // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_BAUD_RATE          9600 // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_CONVERT_NEWLINES   0    // set by library:platform
#define MBED_CONF_PLATFORM_STDIO_FLUSH_AT_EXIT      1    // set by library:platform

//	lifted here from makefile
#define MBED_BUILD_TIMESTAMP 1567513594.91
#define __MBED__ 1
#define DEVICE_I2CSLAVE 1
#define TARGET_LIKE_MBED
#define DEVICE_PORTOUT 1
#define USBHOST_OTHER
#define DEVICE_PORTINOUT 1
#define TARGET_RTOS_M4_M7
#define DEVICE_LOWPOWERTIMER 1
#define DEVICE_RTC 1
#define DEVICE_SERIAL_ASYNCH 1
#define TARGET_STM32F4
#define __CMSIS_RTOS
#define __CORTEX_M4
#define TOOLCHAIN_GCC
#define DEVICE_I2C_ASYNCH 1
#define TARGET_CORTEX_M
#define TARGET_LIKE_CORTEX_M4
#define TARGET_M4
#define DEVICE_ANALOGIN 1
#define DEVICE_SPI_ASYNCH 1
#define TARGET_STM32F411xE
#define DEVICE_SERIAL 1
#define DEVICE_INTERRUPTIN 1
#define TARGET_CORTEX
#define DEVICE_I2C 1
#define TRANSACTION_QUEUE_SIZE_SPI 2
#define TARGET_NUCLEO_F411RE
#define DEVICE_STDIO_MESSAGES 1
#define TARGET_FF_MORPHO
#define __FPU_PRESENT 1
#define TARGET_FF_ARDUINO
#define DEVICE_PORTIN 1
#define TARGET_RELEASE
#define TARGET_STM
#define DEVICE_SERIAL_FC 1
#define __MBED_CMSIS_RTOS_CM
#define DEVICE_SLEEP 1
#define TOOLCHAIN_GCC_ARM
#define DEVICE_SPI 1
#define USB_STM_HAL
#define DEVICE_ERROR_RED 1
#define DEVICE_SPISLAVE 1
#define TARGET_NAME NUCLEO_F411RE
#define DEVICE_PWMOUT 1
#define TARGET_STM32F411RE
#define ARM_MATH_CM4
#define MBED_TRAP_ERRORS_ENABLED 1

#endif
