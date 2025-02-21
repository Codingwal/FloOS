#include "drivers/uart.h"
#include "gpio.h"

enum
{
    AUX_BASE = PERIPHERAL_BASE + 0x215000,
    AUX_ENABLES = AUX_BASE + 4,
    AUX_MU_IO_REG = AUX_BASE + 64,
    AUX_MU_IER_REG = AUX_BASE + 68,
    AUX_MU_IIR_REG = AUX_BASE + 72,
    AUX_MU_LCR_REG = AUX_BASE + 76,
    AUX_MU_MCR_REG = AUX_BASE + 80,
    AUX_MU_LSR_REG = AUX_BASE + 84,
    AUX_MU_CNTL_REG = AUX_BASE + 96,
    AUX_MU_BAUD_REG = AUX_BASE + 104,
    AUX_UART_CLOCK = 500000000,
    UART_MAX_QUEUE = 16 * 1024
};

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK / (baud * 8)) - 1)

ExitCode uart_init()
{
    mmio_write(AUX_ENABLES, 1); // enable UART1
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_CNTL_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3); // 8 bits
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6); // disable interrupts
    mmio_write(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    RETURN_ON_FAILURE(gpio_useAsAlt5(14))
    RETURN_ON_FAILURE(gpio_useAsAlt5(15))
    mmio_write(AUX_MU_CNTL_REG, 3); // enable RX/TX
    return SUCCESS;
}

static bool uart_isWriteByteReady() { return mmio_read(AUX_MU_LSR_REG) & 0x20; }
static bool uart_isReadByteReady() { return mmio_read(AUX_MU_LSR_REG) & 0x01; }

ExitCode uart_writeByte(byte b)
{
    while (!uart_isWriteByteReady())
    {
    }
    mmio_write(AUX_MU_IO_REG, (uint)b);
    return SUCCESS;
}
byte uart_readByte()
{
    while (!uart_isReadByteReady())
    {
    }
    return (byte)mmio_read(AUX_MU_IO_REG);
}