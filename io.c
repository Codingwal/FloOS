#include "io.h"
#include "defs.h"

// GPIO
enum
{
    PERIPHERAL_BASE = 0xFE000000,
    GPFSEL0 = PERIPHERAL_BASE + 0x200000,
    GPSET0 = PERIPHERAL_BASE + 0x20001C,
    GPCLR0 = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0 = PERIPHERAL_BASE + 0x2000E4,

    GPIO_MAX_PIN = 53,
    GPIO_FUNCTION_ALT5 = 2,

    PULL_NONE = 0,
};

void mmio_write(long reg, uint value) { *(volatile uint *)reg = value; }
uint mmio_read(long reg) { return *(volatile uint *)reg; }

ExitCode gpio_call(uint pinNumber, uint value, uint base, uint fieldSize, uint fieldMax)
{
    uint fieldMask = (1 << fieldSize) - 1; // fieldSize=5->00011111

    if (pinNumber > fieldMax || value > fieldMask)
        return FAILURE;

    // Calculate reg and position in reg
    uint fieldsPerReg = 32 / fieldSize;
    uint reg = base + ((pinNumber / fieldsPerReg) * 4);
    uint shift = (pinNumber % fieldsPerReg) * fieldSize;

    // Update relevant bits with value
    uint curval = mmio_read(reg);
    curval &= ~(fieldMask << shift); // Zero out relevant bits
    curval |= value << shift;        // Set relevant bits to new value
    mmio_write(reg, curval);

    return SUCCESS;
}

ExitCode gpio_set(uint pinNumber, uint value) { return gpio_call(pinNumber, value, GPSET0, 1, GPIO_MAX_PIN); }
ExitCode gpio_clear(uint pinNumber, uint value) { return gpio_call(pinNumber, value, GPCLR0, 1, GPIO_MAX_PIN); }
ExitCode gpio_pull(uint pinNumber, uint value) { return gpio_call(pinNumber, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
ExitCode gpio_function(uint pinNumber, uint value) { return gpio_call(pinNumber, value, GPFSEL0, 3, GPIO_MAX_PIN); }

void gpio_useAsAlt5(uint pinNumber)
{
    gpio_pull(pinNumber, PULL_NONE);
    gpio_function(pinNumber, GPIO_FUNCTION_ALT5);
}

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

void uart_init()
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

bool uart_isWriteByteReady() { return mmio_read(AUX_MU_LSR_REG) & 0x20; }

void uart_writeByte(byte b)
{
    while (!uart_isWriteByteReady())
    {
    }
    mmio_write(AUX_MU_IO_REG, (uint)b);
}

void uart_writeText(const char *str)
{
    while (*str)
    {
        if (*str == '\n')
            uart_writeByte('\r');
        uart_writeByte(*str++);
    }
}
