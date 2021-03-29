#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_deinit(void);

#define rx_cbuf_LEN 64
struct rx_cbuf {
    volatile uint8_t head;
    volatile uint8_t tail;
    volatile uint8_t buf[rx_cbuf_LEN];
};
extern struct rx_cbuf rx_cbuf;

#endif
