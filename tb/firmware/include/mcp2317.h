#ifndef MCP2317_H_
#define MCP2317_H_

#define MCP2317_NUM 5

void mcp2317_init(void);
uint8_t mcp2317_read(uint8_t dev, uint8_t address);
void mcp2317_write(uint8_t dev, uint8_t address, uint8_t data);

// This mapping assumes BANK=0 (default)

#define MCP2317_REG_IODIRA    0x00
#define MCP2317_REG_IODIRB    0x01
#define MCP2317_REG_IPOLA     0x02
#define MCP2317_REG_IPOLB     0x03
#define MCP2317_REG_GPINTENA  0x04
#define MCP2317_REG_GPINTENB  0x05
#define MCP2317_REG_DEFVALA   0x06
#define MCP2317_REG_DEFVALB   0x07
#define MCP2317_REG_INTCONA   0x08
#define MCP2317_REG_INTCONB   0x09
#define MCP2317_REG_IOCON     0x0A
// Also MCP2317_REG_IOCON     0x0B
#define MCP2317_REG_GPPUA     0x0C
#define MCP2317_REG_GPPUB     0x0D
#define MCP2317_REG_INTFA     0x0E
#define MCP2317_REG_INTFB     0x0F
#define MCP2317_REG_INTCAPA   0x10
#define MCP2317_REG_INTCAPB   0x11
#define MCP2317_REG_GPIOA     0x12
#define MCP2317_REG_GPIOB     0x13
#define MCP2317_REG_OLATA     0x14
#define MCP2317_REG_OLATB     0x15

#define MCP2317_BANK   (1 << 7)
#define MCP2317_MIRROR (1 << 6)
#define MCP2317_SEQOP  (1 << 5)
#define MCP2317_DISSLW (1 << 4)
#define MCP2317_HAEN   (1 << 3)
#define MCP2317_ODR    (1 << 2)
#define MCP2317_INTPOL (1 << 1)


#endif
