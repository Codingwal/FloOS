#include "gpio.h"

enum
{
    GPFSEL0 = PERIPHERAL_BASE + 0x200000,
    GPSET0 = PERIPHERAL_BASE + 0x20001C,
    GPCLR0 = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0 = PERIPHERAL_BASE + 0x2000E4,

    GPIO_MAX_PIN = 53,
    GPIO_FUNCTION_ALT5 = 2,

    PULL_NONE = 0,
};

void mmio_write(uint64 reg, uint value) { *(volatile uint *)reg = value; }
uint mmio_read(uint64 reg) { return *(volatile uint *)reg; }

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

ExitCode gpio_useAsAlt5(uint pinNumber)
{
    if (gpio_pull(pinNumber, PULL_NONE) == FAILURE)
        return FAILURE;
    if (gpio_function(pinNumber, GPIO_FUNCTION_ALT5) == FAILURE)
        return FAILURE;
    return SUCCESS;
}