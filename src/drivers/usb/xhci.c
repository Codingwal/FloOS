#include "defs.h"
#include "error.h"

/*
typedef struct TransferRequestBlock
{
    uint64 dataBufPtr;
    uint32 status;
    uint32 control;
} __attribute__((packed)) TransferRequestBlock;
REQUIRE_SIZE(TransferRequestBlock, 16);

void transferRing_enqueue(TransferRequestBlock trb)
{
}
bool transferRing_isEmpty(void)
{
}

void xhci_controlTransfer(void)
{
    assert(transferRing_isEmpty(), "transfer ring should be empty");

    uint64 setupData;

    TransferRequestBlock setupStageTD = {
        .dataBufPtr = setupData,
        .status = 8,
        .control = STAGE_SETUP | IMMEDIATE,
    };
    transferRing_enqueue(setupStageTD);

    TransferRequestBlock statusStageTD = {
        .dataBufPtr = NULL,
        .status = 8,
        .control = STAGE_STATUS | OUT,
    };
    transferRing_enqueue(statusStageTD);

    xhci_ringDoorbell();
}
*/