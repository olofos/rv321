#include <stdint.h>

#include "spi.h"
#include "mcp2317.h"
#include "stm32g030xx.h"

#define MCP2317_READ(a)  (0b01000001 | (a << 1))
#define MCP2317_WRITE(a) (0b01000000 | (a << 1))

static inline void spi_mcp2317_cs_assert(void)
{
    GPIOA->BSRR = GPIO_BSRR_BR4;
}

static inline void spi_mcp2317_cs_deassert(void)
{
    GPIOA->BSRR = GPIO_BSRR_BS4;
}


void mcp2317_write(uint8_t dev, uint8_t address, uint16_t data)
{
    spi_mcp2317_cs_assert();

    uint16_t packet[] = { MCP2317_WRITE(dev) | (address << 8), data };

    spi_write_words(packet, 2);

    spi_mcp2317_cs_deassert();
}

uint16_t mcp2317_read(uint8_t dev, uint8_t address)
{
    spi_mcp2317_cs_assert();

    uint16_t packet[] = { MCP2317_READ(dev) | (address << 8), 0 };
    uint16_t reply[2];

    spi_transfer_words(packet, reply, 2);

    spi_mcp2317_cs_deassert();

    return reply[1];
    // return res;
}

void mcp2317_init(void)
{
    const uint8_t init_seq[] = {
        0xFF, 0xFF, // IODIRA, IODIRB
        0x00, 0x00, // IPOLA, IPOLB
        0x00, 0x00, // GPINTENA, GPINTENB
        0x00, 0x00, // DEFVALA, DEFVALB
        0x00, 0x00, // INTCONA, INTCONB
        MCP2317_MIRROR | MCP2317_HAEN | MCP2317_ODR, // IOCONA
        MCP2317_MIRROR | MCP2317_HAEN | MCP2317_ODR, // IOCONB
        0xFF, 0xFF, // GPPUA, GPPUB
    };

    for(int i = 0; i < MCP2317_NUM; i++) {
        spi_mcp2317_cs_assert();
        spi_write_byte(MCP2317_WRITE(i));
        spi_write_byte(MCP2317_REG_IOCON);
        spi_write_byte(MCP2317_MIRROR | MCP2317_HAEN | MCP2317_ODR);
        spi_mcp2317_cs_deassert();

        // mcp2317_write(i, MCP2317_REG_IOCON, MCP2317_MIRROR | MCP2317_HAEN | MCP2317_ODR);

        spi_mcp2317_cs_assert();

        spi_write_byte(MCP2317_WRITE(i));
        spi_write_byte(0x00);
        spi_write_bytes(init_seq, sizeof(init_seq));

        spi_mcp2317_cs_deassert();
    }
}
