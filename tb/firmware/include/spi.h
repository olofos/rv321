#ifndef SPI_H_
#define SPI_H_

void spi_init(void);
void spi_deinit(void);

uint8_t spi_read_byte(void);
void spi_write_byte(uint8_t out);

void spi_read_bytes(uint8_t *buf, uint32_t length);
void spi_write_bytes(const uint8_t *buf, uint32_t length);

uint16_t spi_read_word(void);
void spi_write_word(uint16_t out);

void spi_read_words(uint16_t *buf, uint32_t length);
void spi_write_words(const uint16_t *buf, uint32_t length);
void spi_transfer_words(const uint16_t *in, uint16_t *out, unsigned n);

#endif
