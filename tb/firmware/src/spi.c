#include <stdint.h>

#ifndef UNIT_TESTING
#include "stm32g030xx.h"
#endif

#include "spi.h"

// TODO: stm32g030 has SPI fifos!

#define SPI_DUMMY 0x55

#define SPI_FRAME_SIZE 16

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
#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif
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

static uint8_t spi_receive_byte(void)
{
    return *((volatile uint8_t*)&SPI1->DR);
}

static void spi_send_byte(uint8_t data)
{
    *((volatile uint8_t*)&SPI1->DR) = data;
}

static uint16_t spi_receive_word(void)
{
    return SPI1->DR;
}

static void spi_send_word(uint16_t data)
{
    SPI1->DR = data;
}

void spi_transfer_bytes(const uint8_t *out, uint8_t *in, unsigned n)
{
    unsigned ind_in = 0;
    unsigned ind_out = 0;

#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    while(ind_in < n) {
        if((ind_out < n) && (SPI1->SR & SPI_SR_TXE)) {
            spi_send_byte(out[ind_out++]);
        }
        if((ind_in < n) && (SPI1->SR & SPI_SR_RXNE)) {
            in[ind_in++] = spi_receive_byte();
        }
    }

#if (SPI_FRAME_SIZE == 16)
     SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif
}

void spi_transfer_words(const uint16_t *out, uint16_t *in, unsigned n)
{
    unsigned ind_in = 0;
    unsigned ind_out = 0;

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    while(ind_in < n) {
        if((ind_out < n) && (SPI1->SR & SPI_SR_TXE)) {
            spi_send_word(out[ind_out++]);
        }
        if((ind_in < n) && (SPI1->SR & SPI_SR_RXNE)) {
            in[ind_in++] = spi_receive_word();
        }
    }

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif
}

void spi_deinit(void)
{
    spi_wait_for_rxne();
    spi_wait_for_txe();
    spi_wait_for_busy();

    uint32_t dummy __attribute__((unused)) = spi_receive_byte();

    SPI1->CR1 = 0x00;
    RCC->APBENR2 &= ~RCC_APBENR2_SPI1EN;
}

#else

void spi_wait_for_txe(void);
void spi_wait_for_rxne(void);
void spi_wait_for_busy(void);
uint8_t spi_receive_byte(void);
void spi_send_byte(uint8_t data);
uint8_t spi_receive_word(void);
void spi_send_word(uint8_t data);

#endif

void spi_write_byte(uint8_t out) {
#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    spi_wait_for_txe();
    spi_send_byte(out);

    spi_wait_for_rxne();
    uint32_t dummy __attribute__((unused)) = spi_receive_byte();

#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

}

uint8_t spi_read_byte(void)
{
#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    spi_wait_for_txe();

    spi_send_byte(SPI_DUMMY);

    spi_wait_for_rxne();

#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    return spi_receive_byte();
}

void spi_read_bytes(uint8_t *buf, uint32_t length)
{
#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    for(uint32_t i = 0; i < length; i++) {
        spi_wait_for_txe();
        spi_send_byte(SPI_DUMMY);

        spi_wait_for_rxne();
        buf[i] = spi_receive_byte();
    }

#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif
}

void spi_write_bytes(const uint8_t *buf, uint32_t length)
{
#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    for(uint32_t i = 0; i < length; i++) {
        spi_wait_for_txe();
        spi_send_byte(buf[i]);

        spi_wait_for_rxne();
        uint32_t dummy __attribute__((unused)) = spi_receive_byte();
    }

#if (SPI_FRAME_SIZE == 16)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif
}



void spi_write_word(uint16_t out) {
#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    spi_wait_for_txe();
    spi_send_word(out);

    spi_wait_for_rxne();
    uint32_t dummy __attribute__((unused)) = spi_receive_word();

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif
}

uint16_t spi_read_word(void)
{
#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    spi_wait_for_txe();

    spi_send_word(SPI_DUMMY);

    spi_wait_for_rxne();

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif

    return spi_receive_word();
}

void spi_read_words(uint16_t *buf, uint32_t length)
{
#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    for(uint32_t i = 0; i < length; i++) {
        spi_wait_for_txe();
        spi_send_word(SPI_DUMMY);

        spi_wait_for_rxne();
        buf[i] = spi_receive_word();
    }

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif
}

void spi_write_words(const uint16_t *buf, uint32_t length)
{
#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    unsigned ind_in = 0;
    unsigned ind_out = 0;

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 &= ~SPI_CR2_FRXTH;
#endif

    while(ind_in < length) {
        if((ind_out < length) && (SPI1->SR & SPI_SR_TXE)) {
            spi_send_word(buf[ind_out++]);
        }
        if((ind_in < length) && (SPI1->SR & SPI_SR_RXNE)) {
            uint16_t dummy __attribute__((unused)) = spi_receive_word();
            ind_in++;
        }
    }

#if (SPI_FRAME_SIZE == 8)
    SPI1->CR2 |= SPI_CR2_FRXTH;
#endif
}
