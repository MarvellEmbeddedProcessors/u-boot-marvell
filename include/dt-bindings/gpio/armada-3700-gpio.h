/*
 * This header provides constants for binding marvell,armada-3700-gpio.
 *
 * The first cell in armada-3700's GPIO specifier is the GPIO bank reference.
 *
 * The second cell in armada-3700's GPIO specifier is the global GPIO ID. The macros below
 * provide names for this.
 *
 * The third cell contains standard flag values specified in gpio.h.
 */

#ifndef _DT_BINDINGS_GPIO_ARMADA_3700_GPIO_H
#define _DT_BINDINGS_GPIO_ARMADA_3700_GPIO_H

#include <dt-bindings/gpio/gpio.h>

/* armada3700 has 2 gpio banks */
/* bank0 is north bridge gpio, it has 61 gpio pins, its global gpio base number is 0 */
/* bank0 is south bridge gpio, it has 30 gpio pins, its global gpio base number is 61 */
#define ARMADA_3700_BANK_0_GPIO_BASE 0
#define ARMADA_3700_BANK_0_GPIO_COUNT 61
#define ARMADA_3700_BANK_1_GPIO_BASE (ARMADA_3700_BANK_0_GPIO_BASE + ARMADA_3700_BANK_0_GPIO_COUNT)
#define ARMADA_3700_BANK_1_GPIO_COUNT 30

#define ARMADA_3700_GPIO(bank, offset) \
	(ARMADA_3700_##bank##_GPIO_BASE + offset)

#endif
