#ifndef SPI_H_
#define SPI_H_

void spi_init(void);
void spi_deinit(void);

uint8_t spi_read_byte(void);
void spi_write_byte(uint8_t out);

void spi_read(uint8_t *buf, uint32_t length);
void spi_write(const uint8_t *buf, uint32_t length);


#endif
