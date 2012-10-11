#include <common.h>
#include <flash.h>

#ifdef CONFIG_SPI_FLASH
#include <spi.h>
#include <spi_flash.h>

struct spi_flash *flash;

unsigned long spi_flash_init(void)
{
	flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, CONFIG_ENV_SPI_MAX_HZ, SPI_MODE_3);
	if (!flash) {
		printf("Failed to initialize SPI flash at %u:%u\n", CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS);
		return 1;
	}
	return 0;
}

#endif