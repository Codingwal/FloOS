# FloOS

An operating system for the rpi4b, written in c


## Compilation info:
- Compile using make:
    - Build kernel8.img with "make os"
    - Replace the existing kernel8.img file on the sd card

## TODO:
- void instead of no params
- usb


spsr_el3 (Saved Program Status Register EL3)
    - https://developer.arm.com/documentation/ddi0595/2021-03/AArch64-Registers/SPSR-EL3--Saved-Program-Status-Register--EL3-
    - Holds the process state when an exception is taken to EL3
scr_el3 (Secure Configuration Register)
    - https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/SCR-EL3--Secure-Configuration-Register
    - defined whether EL0, ElL and EL2 are secure or non-secure
    - defines execution states of lower exception levels
sctlr_el2 (System Control Register EL2)
    - https://developer.arm.com/documentation/ddi0595/2020-12/AArch64-Registers/SCTLR-EL2--System-Control-Register--EL2-?lang=en
    - top level control, including the memory system (mmu, vm) at EL2
hcr_el2 (Hypervisor Configuration Register)
    - https://developer.arm.com/documentation/ddi0595/2021-06/AArch64-Registers/HCR-EL2--Hypervisor-Configuration-Register
    - defines config for virtualization, including information whether various operations are trapped to El2
elr_el3 (Exception link Register EL3)
    - https://developer.arm.com/documentation/ddi0601/2020-12/AArch64-Registers/ELR-EL3--Exception-Link-Register--EL3-
    - holds the address for an exception return from EL3