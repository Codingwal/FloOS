# FloOS

A baremetal operating system for a rasperry pi


## Compilation info:
- Compile using make:
    - Build kernel8.img with "make os"
    - Replace the existing kernel8.img file on the sd card

## TODO:
- printf using stdarg instead of PRINT macro
- asserts instead of exit codes
- mmu
    - check permission flags
- usb