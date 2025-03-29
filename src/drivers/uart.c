#include "drivers/uart.h"
#include "drivers/gpio.h"
#include "mem.h"

#define AUX_BASE (PERIPHERAL_BASE + 0x215000)
#define AUX_ENABLES (AUX_BASE + 4)
#define AUX_MU_IO_REG (AUX_BASE + 64)
#define AUX_MU_IER_REG (AUX_BASE + 68)
#define AUX_MU_IIR_REG (AUX_BASE + 72)
#define AUX_MU_LCR_REG (AUX_BASE + 76)
#define AUX_MU_MCR_REG (AUX_BASE + 80)
#define AUX_MU_LSR_REG (AUX_BASE + 84)
#define AUX_MU_CNTL_REG (AUX_BASE + 96)
#define AUX_MU_BAUD_REG (AUX_BASE + 104)

#define AUX_UART_CLOCK (500000000)
#define UART_MAX_QUEUE (16 * 102)

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK / (baud * 8)) - 1)

void uart_init(void)
{
    mmio_write(AUX_ENABLES, 1); // enable UART1
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3); // 8 bits
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); // disable interrupts
    mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    gpio_useAsAlt5(14);
    gpio_useAsAlt5(15);
    mmio_write(AUX_MU_CNTL_REG, 3); // enable RX/TX
}

static bool uart_isWriteByteReady(void) { return mmio_read(AUX_MU_LSR_REG) & 0x20; }
static bool uart_isReadByteReady(void) { return mmio_read(AUX_MU_LSR_REG) & 0x01; }

void uart_writeByte(byte b)
{
    while (!uart_isWriteByteReady())
    {
    }
    mmio_write(AUX_MU_IO_REG, (uint)b);
}
byte uart_readByte(void)
{
    while (!uart_isReadByteReady())
    {
    }
    return (byte)mmio_read(AUX_MU_IO_REG);
}