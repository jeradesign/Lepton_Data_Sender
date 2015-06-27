#include "mraa.hpp"

#include <iostream>
#include <unistd.h>

void setupSPI()
{
	mraa_gpio_context cs = mraa_gpio_init(10);
	mraa_gpio_dir(cs, MRAA_GPIO_OUT);

	mraa_spi_context spi = mraa_spi_init(5);	// bus 5 for edison
	mraa_spi_mode(spi, MRAA_SPI_MODE3);
	mraa_spi_frequency(spi, 10000000);
	mraa_spi_lsbmode(spi, 0);
	mraa_spi_bit_per_word(spi, 8);
}

int processScanLine()
{
	return 0;
}

int main()
{
	setupSPI();
	printf("%x", processScanLine());
}

