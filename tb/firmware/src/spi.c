#include <stdint.h>

#ifndef UNIT_TESTING
#include "stm32g030xx.h"
#endif

#include "spi.h"

// TODO: stm32g030 has SPI fifos!

#define SPI_DUMMY 0x55

#ifndef UNIT_TESTING

#define SPI_CR_BR_DIV_2   (0x0 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_4   (0x1 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_8   (0x2 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_16  (0x3 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_32  (0x4 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_64  (0x5 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_128 (0x6 << SPI_CR1_BR_Pos)
#define SPI_CR_BR_DIV_256 (0x7 << SPI_CR1_BR_Pos)

void spi_init(void)
{
    RCC->APBENR2 |= RCC_APBENR2_SPI1EN;
    SPI1->CR2 |= SPI_CR2_FRXTH;
    SPI1->CR1 = SPI_CR_BR_DIV_8 | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE;
}

static void spi_wait_for_txe(void)
{
    while(!(SPI1->SR & SPI_SR_TXE)) {
    }
}

static void spi_wait_for_rxne(void)
{
    while(!(SPI1->SR & SPI_SR_RXNE)) {
    }
}

static void spi_wait_for_busy(void)
{
    while(SPI1->SR & SPI_SR_BSY) {
    }
}

static uint8_t spi_receive(void)
{
    return *((volatile uint8_t*)&SPI1->DR);
}

static void spi_send(uint8_t data)
{
    *((volatile uint8_t*)&SPI1->DR) = data;
}

void spi_transfer(int8_t *in, uint8_t *out, unsigned n)
{
    unsigned ind_in = 0;
    unsigned ind_out = 0;

    while(ind_in < n) {
        if((ind_out < n) && (SPI1->SR & SPI_SR_TXE)) {
            spi_send(in[ind_in++]);
        }
        if((ind_in < n) && (SPI1->SR & SPI_SR_RXNE)) {
            out[ind_out++] = spi_receive();
        }
    }
}

void spi_deinit(void)
{
    spi_wait_for_rxne();
    spi_wait_for_txe();
    spi_wait_for_busy();

    uint32_t dummy __attribute__((unused)) = spi_receive();

    SPI1->CR1 = 0x00;
    RCC->APBENR2 &= ~RCC_APBENR2_SPI1EN;
}

#else

void spi_wait_for_txe(void);
void spi_wait_for_rxne(void);
void spi_wait_for_busy(void);
uint8_t spi_receive(void);
void spi_send(uint8_t data);

#endif

void spi_write_byte(uint8_t out) {
    spi_wait_for_txe();
    spi_send(out);

    spi_wait_for_rxne();
    uint32_t dummy __attribute__((unused)) = spi_receive();
}

uint8_t spi_read_byte(void)
{
    spi_wait_for_txe();

    spi_send(SPI_DUMMY);

    spi_wait_for_rxne();

    return spi_receive();
}

void spi_read(uint8_t *buf, uint32_t length)
{
    for(uint32_t i = 0; i < length; i++) {
        spi_wait_for_txe();
        spi_send(SPI_DUMMY);

        spi_wait_for_rxne();
        buf[i] = spi_receive();
    }
}

void spi_write(const uint8_t *buf, uint32_t length)
{
    for(uint32_t i = 0; i < length; i++) {
        spi_wait_for_txe();
        spi_send(buf[i]);

        spi_wait_for_rxne();
        uint32_t dummy __attribute__((unused)) = spi_receive();
    }
}
