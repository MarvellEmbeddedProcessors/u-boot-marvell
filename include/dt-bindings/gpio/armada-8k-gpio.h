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

/* The driver module of the U-Boot append all the GPIOs,
** example: if need to get GPIO #12 at CP0, need to call GPIO functions with 20+12=32
** The below macros will help the user to define the GPIO number without any base */
#define AP_GPIO(offset)			(offset)
#define CP0_GPIO(offset)		(ARMADA_CP0_B0_GPIO_BASE + offset)
#define CP1_GPIO(offset)		(ARMADA_CP1_B0_GPIO_BASE + offset)

#endif /* _DT_BINDINGS_GPIO_ARMADA_8K_GPIO_H */
