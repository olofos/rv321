#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "startup.h"
#include "cbuf.h"
#include "uart.h"

#define UART_TX_DMA 0
#define UART_TX_IRQ 1

// #define BAUD 38400
#define BAUD 2000000

// LEN should be power of 2 and strictly less than 2^sizeof(head)

#define tx_cbuf_LEN 128

struct tx_cbuf {
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t next_tail;
    volatile uint8_t buf[tx_cbuf_LEN];
};

volatile struct tx_cbuf tx_cbuf;

struct rx_cbuf rx_cbuf;

void uart_init_common(void)
{
}

void uart_init_tx(void)
{
#if UART_TX_DMA
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR3 |= USART_CR3_DMAT;
    NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
    DMA1_Channel2->CPAR = (uint32_t) &USART2->TDR;
    DMAMUX1_Channel1->CCR = DMAMUX_CCR_REQ_USART2_TX;
#else
    USART2->CR1 |= USART_CR1_TE;
    NVIC_EnableIRQ(USART2_IRQn);
#endif
    cbuf_init(tx_cbuf);
}

void uart_init_rx(void)
{
    USART2->CR1 |= USART_CR1_RXNEIE_RXFNEIE | USART_CR1_RE;
    NVIC_EnableIRQ(USART2_IRQn);
    cbuf_init(rx_cbuf);
}

void uart_init(void)
{
    if(!(USART2->CR1 & USART_CR1_UE)) {
        RCC->APBENR1 |= RCC_APBENR1_USART2EN;
        USART2->BRR = CPU_FREQ / BAUD;

        USART2->CR1 = 0;
        uart_init_tx();
        uart_init_rx();

        USART2->CR1 |= USART_CR1_UE;
    }
}

void uart_deinit(void)
{
    while(!(USART2->ISR & USART_ISR_TC)) {
    }

    USART2->CR1 = 0;
    NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
    RCC->APBENR1 &= ~RCC_APBENR1_USART2EN;
}

#if UART_TX_DMA
static void uart_dma_tx_start(void)
{
    uint32_t len = cbuf_linear_len(tx_cbuf);

    if(len > 0) {
        // Use tx_cbuf.tail and not cbuf_tail(tx_cbuf) so we count mod sizeof(tail) and not mod LEN
        tx_cbuf.next_tail = tx_cbuf.tail + len;

        DMA1_Channel2->CMAR = (uint32_t) cbuf_tail(tx_cbuf);
        DMA1_Channel2->CNDTR = len;
        DMA1_Channel2->CCR = DMA_CCR_MINC | DMA_CCR_DIR | DMA_CCR_TCIE | DMA_CCR_EN;
    }
}

void pwm_set_anim(int n);
ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t count)
{
    (void) reent;

    if(fd == 1) {
        const char *s = buf;

        if(USART2->CR3 & USART_CR3_DMAT) {
            size_t num_left = count;

            while(num_left > 0) {
                size_t cap = cbuf_capacity(tx_cbuf);
                size_t num_to_write = num_left < cap ? num_left : cap;

                while(num_to_write) {
                    cbuf_push(tx_cbuf, *s++);
                    num_to_write--;
                    num_left--;
                }

                if(!(DMA1_Channel2->CCR & DMA_CCR_EN)) {
                    uart_dma_tx_start();
                }
            }
        } else {
            for(size_t i = 0; i < count; i++) {
                while(!(USART2->ISR & USART_ISR_TXE_TXFNF)) {
                }

                USART2->TDR = s[i];
            }
        }

        return count;
    } else {
        return -1;
    }
}

void DMA1_Channel2_3_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF2) {
        DMA1_Channel2->CCR = 0;

        tx_cbuf.tail = tx_cbuf.next_tail;

        if(!cbuf_empty(tx_cbuf)) {
            uart_dma_tx_start();
        }

        DMA1->IFCR = DMA_IFCR_CTCIF2 | DMA_IFCR_CHTIF2 | DMA_IFCR_CGIF2;
    }
}
#elif UART_TX_IRQ
void pwm_set_anim(int n);
ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t count)
{
    (void) reent;

    if(fd == 1) {
        const uint8_t *s = buf;
        for(size_t i = 0; i < count; i++) {
            if(cbuf_full(tx_cbuf)) {
                pwm_set_anim(1);
                // USART2->CR1 |= USART_CR1_TXEIE_TXFNFIE;
            }

            cbuf_push(tx_cbuf, s[i]);
        }
        USART2->CR1 |= USART_CR1_TXEIE_TXFNFIE;
        return count;
    } else {
        return -1;
    }
}

#else
ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t count)
{
    (void) reent;

    if(fd == 1) {
        const char *s = buf;
        for(size_t i = 0; i < count; i++) {
            while(!(USART2->ISR & USART_ISR_TXE_TXFNF)) {
            }

            USART2->TDR = s[i];
        }
        return count;
    } else {
        return -1;
    }
}

#endif

void USART2_IRQHandler(void)
{
    if(USART2->ISR & USART_ISR_RXNE_RXFNE) {
        uint8_t c = USART2->RDR;
        cbuf_push(rx_cbuf, c);
    }

    if(USART2->ISR & USART_ISR_ORE) {
        pwm_set_anim(1);
        USART2->ICR = USART_ICR_ORECF;
    }

    if(USART2->ISR & USART_ISR_TXE_TXFNF) {
        if(cbuf_empty(tx_cbuf)) {
            USART2->CR1 &= ~USART_CR1_TXEIE_TXFNFIE;
        } else {
            USART2->TDR = cbuf_pop(tx_cbuf);
        }
    }
}
