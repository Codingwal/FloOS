#pragma once

#include "defs.h"

void bt_reset();
void bt_loadfirmware();
void bt_setbaud();
void bt_setbdaddr();
void bt_getbdaddr(byte *bdaddr);
void bt_init();

bool bt_isReadByteReady();
byte bt_readByte();
byte bt_waitReadByte();

void setLEeventmask(byte mask);
void startActiveScanning();
void stopScanning();
void startActiveAdvertising();
void connect(byte *addr);
void sendACLsubscribe(uint handle);