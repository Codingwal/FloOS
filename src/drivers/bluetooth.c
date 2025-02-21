#include "drivers/bluetooth.h"
#include "defs.h"
#include "gpio.h"
#include "io.h"
#include "stringFormat.h"

enum
{
    ARM_UART0_BASE = PERIPHERAL_BASE + 0x201000,
    ARM_UART0_DR = ARM_UART0_BASE + 0x00,
    ARM_UART0_FR = ARM_UART0_BASE + 0x18,
    ARM_UART0_IBRD = ARM_UART0_BASE + 0x24,
    ARM_UART0_FBRD = ARM_UART0_BASE + 0x28,
    ARM_UART0_LCRH = ARM_UART0_BASE + 0x2C,
    ARM_UART0_CR = ARM_UART0_BASE + 0x30,
    ARM_UART0_IFLS = ARM_UART0_BASE + 0x34,
    ARM_UART0_IMSC = ARM_UART0_BASE + 0x38,
    ARM_UART0_RIS = ARM_UART0_BASE + 0x3C,
    ARM_UART0_MIS = ARM_UART0_BASE + 0x40,
    ARM_UART0_ICR = ARM_UART0_BASE + 0x44
};

static byte lowByte(uint val) { return (byte)(val & 0xff); }
static byte highByte(uint val) { return (byte)((val & 0xff00) >> 8); }

bool bt_isReadByteReady() { return (!(mmio_read(ARM_UART0_FR) & 0x10)); }
static bool bt_isWriteByteReady() { return (!(mmio_read(ARM_UART0_FR) & 0x20)); }

byte bt_readByte()
{
    return lowByte(mmio_read(ARM_UART0_DR));
}

byte bt_waitReadByte()
{
    while (!bt_isReadByteReady())
    {
    }
    return bt_readByte();
}

void bt_writeByte(byte b)
{
    while (bt_isWriteByteReady())
    {
    }
    mmio_write(ARM_UART0_DR, (uint)b);
}

static void bt_flushrx()
{
    while (bt_isReadByteReady())
        bt_readByte();
}

static void wait_msec(uint n)
{
    register unsigned long f, t, r;

    // Get the current counter frequency
    asm volatile("mrs %0, cntfrq_el0" : "=r"(f));
    // Read the current counter
    asm volatile("mrs %0, cntpct_el0" : "=r"(t));
    // Calculate expire value for counter
    t += ((f / 1000) * n) / 1000;
    do
    {
        asm volatile("mrs %0, cntpct_el0" : "=r"(r));
    } while (r < t);
}

void bt_init()
{
    gpio_useAsAlt3(30);
    gpio_useAsAlt3(31);
    gpio_useAsAlt3(32);
    gpio_useAsAlt3(33);

    bt_flushrx();

    mmio_write(ARM_UART0_IMSC, 0x00);
    mmio_write(ARM_UART0_ICR, 0x7ff);
    mmio_write(ARM_UART0_IBRD, 0x1a);
    mmio_write(ARM_UART0_FBRD, 0x03);
    mmio_write(ARM_UART0_IFLS, 0x08);
    mmio_write(ARM_UART0_LCRH, 0x70);
    mmio_write(ARM_UART0_CR, 0xB01);
    mmio_write(ARM_UART0_IMSC, 0x430);

    wait_msec(0x100000);
}

enum
{
    OGF_HOST_CONTROL = 0x03,
    OGF_LE_CONTROL = 0x08,
    OGF_VENDOR = 0x3f,

    COMMAND_SET_BDADDR = 0x01,
    COMMAND_RESET_CHIP = 0x03,
    COMMAND_SET_BAUD = 0x18,
    COMMAND_LOAD_FIRMWARE = 0x2e,

    HCI_COMMAND_PKT = 0x01,
    HCI_ACL_PKT = 0x02,
    HCI_EVENT_PKT = 0x04,
    COMMAND_COMPLETE_CODE = 0x0e,
    CONNECT_COMPLETE_CODE = 0x0f,

    LL_SCAN_ACTIVE = 0x01,
    LL_ADV_NONCONN_IND = 0x03
};

int hciCommandBytes(byte *opcode, volatile byte *data, byte length)
{
    // Write header (command type, opcode, data length)
    bt_writeByte(HCI_COMMAND_PKT);
    bt_writeByte(opcode[0]);
    bt_writeByte(opcode[1]);
    bt_writeByte(length);
    

        // Write data
        for (uint i = 0; i < length; i++)
            bt_writeByte(data[i]);

    if (bt_waitReadByte() != HCI_EVENT_PKT)
        return 1;

    byte code = bt_waitReadByte();
    if (code == CONNECT_COMPLETE_CODE)
    {
        if (bt_waitReadByte() != 4)
            return 2;

        byte err = bt_waitReadByte();
        if (err != 0) // Error (0 means success)
        {
            PRINT("Saw HCI COMMAND STATUS error %x\n", err)
            return 12;
        }

        if (bt_waitReadByte() == 0)
            return 3;
        if (bt_waitReadByte() != opcode[0])
            return 4;
        if (bt_waitReadByte() != opcode[1])
            return 5;
    }
    else if (code == COMMAND_COMPLETE_CODE)
    {
        if (bt_waitReadByte() != 4)
            return 6;
        if (bt_waitReadByte() == 0)
            return 7;
        if (bt_waitReadByte() != opcode[0])
            return 8;
        if (bt_waitReadByte() != opcode[1])
            return 9;
        if (bt_waitReadByte() != 0)
            return 10;
    }
    else
        return 11;

    return 0;
}

int hciCommand(unsigned short ogf, unsigned short ocf, volatile byte *data, byte length)
{
    unsigned short opcode = ogf << 10 | ocf;
    byte opcodeBytes[2] = {lowByte(opcode), highByte(opcode)};

    return hciCommandBytes(opcodeBytes, data, length);
}

void bt_reset()
{
    volatile byte empty[] = {};
    if (hciCommand(OGF_HOST_CONTROL, COMMAND_RESET_CHIP, empty, 0))
        print("bt_reset() failed\n");
}

// void bt_loadfirmware()
// {
//     volatile byte empty[] = {};
//     if (hciCommand(OGF_VENDOR, COMMAND_LOAD_FIRMWARE, empty, 0))
//         print("loadFirmware() failed\n");

//     extern byte _binary_BCM4345C0_hcd_start[];
//     extern byte _binary_BCM4345C0_hcd_size[];

//     uint c = 0;
//     uint size = (long)&_binary_BCM4345C0_hcd_size;

//     byte opcode[2];
//     byte length;
//     byte *data = &(_binary_BCM4345C0_hcd_start[0]);

//     while (c < size)
//     {
//         opcode[0] = *data;
//         opcode[1] = *(data + 1);
//         length = *(data + 2);
//         data += 3;

//         if (hciCommandBytes(opcode, data, length))
//         {
//             print("Firmware data load failed\n");
//             break;
//         }

//         data += length;
//         c += 3 + length;
//     }

//     wait_msec(0x100000);
// }

void bt_setbaud()
{
    volatile byte command[6] = {0, 0, 0x00, 0xc2, 0x01, 0x00}; // little endian, 115200
    if (hciCommand(OGF_VENDOR, COMMAND_SET_BAUD, command, 6))
        print("bt_setbaud() failed\n");
}

void bt_setbdaddr()
{
    volatile byte command[6] = {0xee, 0xff, 0xc0, 0xee, 0xff, 0xc0}; // reversed
    if (hciCommand(OGF_VENDOR, COMMAND_SET_BDADDR, command, 6))
        print("bt_setbdaddr() failed\n");
}

void bt_getbdaddr(byte *bdaddr)
{
    bt_writeByte(HCI_COMMAND_PKT);
    bt_writeByte(0x09);
    bt_writeByte(0x10);
    bt_writeByte(0x00);

    if (bt_waitReadByte() != HCI_EVENT_PKT)
        return;
    if (bt_waitReadByte() != COMMAND_COMPLETE_CODE)
        return;
    if (bt_waitReadByte() != 0x0a)
        return;
    if (bt_waitReadByte() != 1)
        return;
    if (bt_waitReadByte() != 0x09)
        return;
    if (bt_waitReadByte() != 0x10)
        return;
    if (bt_waitReadByte() != 0x00)
        return;

    for (int c = 0; c < 6; c++)
        bdaddr[c] = bt_waitReadByte();
}

void sendACLsubscribe(uint handle)
{
    bt_writeByte(HCI_ACL_PKT);

    bt_writeByte(lowByte(handle));
    bt_writeByte(highByte(handle));

    uint length = 0x0009;
    bt_writeByte(lowByte(length));
    bt_writeByte(highByte(length));

    uint data_length = 0x0005;
    bt_writeByte(lowByte(data_length));
    bt_writeByte(highByte(data_length));

    uint channel = 0x0004;
    bt_writeByte(lowByte(channel));
    bt_writeByte(highByte(channel));

    volatile byte command[5] = {0x12, 0x2b, 0x00, 0x01, 0x00};

    uint c = 0;
    while (c++ < data_length)
        bt_writeByte(command[c - 1]);
}

void setLEeventmask(byte mask)
{
    volatile byte command[8] = {0};
    command[0] = mask;

    if (hciCommand(OGF_LE_CONTROL, 0x01, command, 8))
        print("setLEeventmask failed\n");
}

void setLEscanenable(byte state, byte duplicates)
{
    volatile byte command[2];
    command[0] = state;
    command[1] = duplicates;
    if (hciCommand(OGF_LE_CONTROL, 0x0c, command, 2))
        print("setLEscanenable failed\n");
}

void setLEscanparameters(byte type, byte linterval, byte hinterval, byte lwindow, byte hwindow, byte own_address_type, byte filter_policy)
{
    volatile byte command[7];
    command[0] = type;
    command[1] = linterval;
    command[2] = hinterval;
    command[3] = lwindow;
    command[4] = hwindow;
    command[5] = own_address_type;
    command[6] = filter_policy;
    if (hciCommand(OGF_LE_CONTROL, 0x0b, command, 7))
        print("setLEscanparameters failed\n");
}

void setLEadvertenable(byte state)
{
    volatile byte command[1];
    command[0] = state;
    if (hciCommand(OGF_LE_CONTROL, 0x0a, command, 1))
        print("setLEadvertenable failed\n");
}

void setLEadvertparameters(byte type, byte linterval_min, byte hinterval_min, byte linterval_max, byte hinterval_max, byte own_address_type, byte filter_policy)
{
    volatile byte command[15] = {0};

    command[0] = linterval_min;
    command[1] = hinterval_min;
    command[2] = linterval_max;
    command[3] = hinterval_max;
    command[4] = type;
    command[5] = own_address_type;
    command[13] = 0x07;
    command[14] = filter_policy;

    if (hciCommand(OGF_LE_CONTROL, 0x06, command, 15))
        print("setLEadvertparameters failed\n");
}

void setLEadvertdata()
{
    static byte command[32] = {
        0x19,
        0x02, 0x01, 0x06,
        0x03, 0x03, 0xAA, 0xFE,
        0x11, 0x16, 0xAA, 0xFE, 0x10, 0x00, 0x03,
        0x69, 0x73, 0x6f, 0x6d, 0x65, 0x74, 0x69, 0x6d,
        0x2e, 0x65, 0x73,
        0, 0, 0, 0, 0, 0};

    if (hciCommand(OGF_LE_CONTROL, 0x08, command, 32))
        print("setLEadvertdata failed\n");
}

void stopScanning()
{
    setLEscanenable(0, 0);
}

void stopAdvertising()
{
    setLEadvertenable(0);
}

void startActiveScanning()
{
    float BleScanInterval = 60; // every 60ms
    float BleScanWindow = 60;
    float BleScanDivisor = 0.625;

    uint p = BleScanInterval / BleScanDivisor;
    uint q = BleScanWindow / BleScanDivisor;

    setLEscanparameters(LL_SCAN_ACTIVE, lowByte(p), highByte(p), lowByte(q), highByte(q), 0, 0);
    setLEscanenable(1, 0);
}

void startActiveAdvertising()
{
    float advertMinFreq = 100; // every 100ms
    float advertMaxFreq = 100; // every 100ms
    float bleGranularity = 0.625;

    uint min_interval = advertMinFreq / bleGranularity;
    uint max_interval = advertMaxFreq / bleGranularity;

    setLEadvertparameters(LL_ADV_NONCONN_IND, lowByte(min_interval), highByte(min_interval), lowByte(max_interval), highByte(max_interval), 0, 0);
    setLEadvertdata();
    setLEadvertenable(1);
}

void connect(byte *addr)
{
    float BleScanInterval = 60; // every 60ms
    float BleScanWindow = 60;
    float BleScanDivisor = 0.625;

    float connMinFreq = 30; // every 30ms
    float connMaxFreq = 50; // every 50ms
    float BleGranularity = 1.25;

    uint p = BleScanInterval / BleScanDivisor;
    uint q = BleScanWindow / BleScanDivisor;

    uint min_interval = connMinFreq / BleGranularity;
    uint max_interval = connMaxFreq / BleGranularity;

    volatile byte command[25] = {0};

    command[0] = lowByte(p);
    command[2] = lowByte(q);
    command[6] = *(addr + 5);
    command[7] = *(addr + 4);
    command[8] = *(addr + 3);
    command[9] = *(addr + 2);
    command[10] = *(addr + 1);
    command[11] = *addr;
    command[13] = lowByte(min_interval);
    command[15] = lowByte(max_interval);
    command[19] = 0x2a;
    command[20] = 0x00;

    if (hciCommand(OGF_LE_CONTROL, 0x0d, command, 25))
        print("createLEconnection failed\n");
}