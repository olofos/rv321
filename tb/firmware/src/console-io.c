#include <unistd.h>

#include "console.h"
#include "uart.h"
#include "cbuf.h"

void console_write(const char *buf, unsigned len)
{
    write(1, buf, len);
}

int console_getchar(void)
{
    if(cbuf_empty(rx_cbuf)) {
        return -1;
    }
    return cbuf_pop(rx_cbuf);
}
