#include "drivers/gpio.h"
#include "mem.h"
#include "assert.h"

enum
{
    GPFSEL0 = PERIPHERAL_BASE + 0x200000,
    GPSET0 = PERIPHERAL_BASE + 0x20001C,
    GPCLR0 = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0 = PERIPHERAL_BASE + 0x2000E4,

    GPIO_MAX_PIN = 53,
    GPIO_FUNCTION_ALT3 = 7,
    GPIO_FUNCTION_ALT5 = 2,

    PULL_NONE = 0,
};

void mmio_write(uint64 reg, uint value) { *(volatile uint *)reg = value; }
uint mmio_read(uint64 reg) { return *(volatile uint *)reg; }

void gpio_call(uint pinNumber, uint value, uint base, uint fieldSize, uint fieldMax)
{
    uint fieldMask = (1 << fieldSize) - 1; // fieldSize=5->00011111

    assert(pinNumber <= fieldMax, "gpio_call: pinNumber exceeds pin count");
    assert(value <= fieldMask, "gpio_call: value exceeds field size");

    // Calculate reg and position in reg
    uint fieldsPerReg = 32 / fieldSize;
    uint reg = base + ((pinNumber / fieldsPerReg) * 4);
    uint shift = (pinNumber % fieldsPerReg) * fieldSize;

    // Update relevant bits with value
    uint curval = mmio_read(reg);
    curval &= ~(fieldMask << shift); // Zero out relevant bits
    curval |= value << shift;        // Set relevant bits to new value
    mmio_write(reg, curval);
}

void gpio_set(uint pinNumber, uint value) { gpio_call(pinNumber, value, GPSET0, 1, GPIO_MAX_PIN); }
void gpio_clear(uint pinNumber, uint value) { gpio_call(pinNumber, value, GPCLR0, 1, GPIO_MAX_PIN); }
void gpio_pull(uint pinNumber, uint value) { gpio_call(pinNumber, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
void gpio_function(uint pinNumber, uint value) { gpio_call(pinNumber, value, GPFSEL0, 3, GPIO_MAX_PIN); }

void gpio_useAsAlt3(uint pinNumber)
{
    gpio_pull(pinNumber, PULL_NONE);
    gpio_function(pinNumber, GPIO_FUNCTION_ALT3);
}
void gpio_useAsAlt5(uint pinNumber)
{
    gpio_pull(pinNumber, PULL_NONE);
    gpio_function(pinNumber, GPIO_FUNCTION_ALT5);
}