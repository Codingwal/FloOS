# FloOS

A baremetal operating system for a rasperry pi


## Compilation info:
- Compile using make:
    - Build kernel8.img with "make os"
    - Replace the existing kernel8.img file on the sd card

## TODO:
- asserts instead of exit codes
- move gpio to drivers
- mmu
    - check permission flags
- usb