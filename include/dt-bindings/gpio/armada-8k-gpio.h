/*
 * This header provides constants for binding marvell,mvebu-gpio
 *
 * First cell used as pointer to GPIO bank node.
 * Second cell set the GPIO number
 * Third cell contains standard flag values specified in gpio.h.
 */

#ifndef _DT_BINDINGS_GPIO_ARMADA_8K_GPIO_H
#define _DT_BINDINGS_GPIO_ARMADA_8K_GPIO_H

#include <dt-bindings/gpio/gpio.h>

/* Armada-8k has 3 GPIO banks:
** AP-806: bank0, it has 20 gpio pins
** CP0: bank1, it has 32 gpio pins
** CP1: bank2, it has 31 gpio pins */
#define ARMADA_AP806_GPIO_BASE		0
#define ARMADA_AP806_GPIO_COUNT		20
#define ARMADA_CP0_B0_GPIO_BASE		(ARMADA_AP806_GPIO_BASE + ARMADA_AP806_GPIO_COUNT)
#define ARMADA_CP0_B0_GPIO_COUNT	32
#define ARMADA_CP0_B1_GPIO_BASE		(ARMADA_CP0_B0_GPIO_BASE + ARMADA_CP0_B0_GPIO_COUNT)
#define ARMADA_CP0_B1_GPIO_COUNT	31
#define ARMADA_CP1_B0_GPIO_BASE		(ARMADA_CP0_B1_GPIO_BASE + ARMADA_CP0_B1_GPIO_COUNT)
#define ARMADA_CP1_B0_GPIO_COUNT	32
#define ARMADA_CP1_B1_GPIO_BASE		(ARMADA_CP1_B0_GPIO_BASE + ARMADA_CP1_B0_GPIO_COUNT)
#define ARMADA_CP1_B1_GPIO_COUNT	31

/* Armada-8k has 2 IO-expanders:
 * expander0 under 0x21 i2c address, it has 16 gpio pins it controls e.g. usb vbus from CP0
 * expander1 under 0x25 i2c address, it has 16 gpio pins it controls e.g. usb vbus from CP1
 */
#define ARMADA_CP0_EXPANDER0_GPIO_BASE	(ARMADA_CP1_B1_GPIO_BASE + ARMADA_CP1_B1_GPIO_COUNT)
#define ARMADA_CP0_EXPANDER0_GPIO_SIZE	16
#define ARMADA_CP0_EXPANDER1_GPIO_BASE	(ARMADA_CP0_EXPANDER0_GPIO_BASE + ARMADA_CP0_EXPANDER0_GPIO_SIZE)

/* The driver module of the U-Boot append all the GPIOs,
** example: if need to get GPIO #12 at CP0, need to call GPIO functions with 20+12=32
** The below macros will help the user to define the GPIO number without any base */
#define AP_GPIO(offset)			(offset)
#define CP0_GPIO(offset)		(ARMADA_CP0_B0_GPIO_BASE + offset)
#define CP1_GPIO(offset)		(ARMADA_CP1_B0_GPIO_BASE + offset)

#define EXPANDER0_GPIO(offset)		(ARMADA_CP0_EXPANDER0_GPIO_BASE + offset)
#define EXPANDER1_GPIO(offset)		(ARMADA_CP0_EXPANDER1_GPIO_BASE + offset)

#endif /* _DT_BINDINGS_GPIO_ARMADA_8K_GPIO_H */
