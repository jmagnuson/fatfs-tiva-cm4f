# FatFs Tiva-cm4f port

## Description

This library file modifies an existing driver provided by TI and adds DMA
functionality for sector transmissions.

## Notes

The target architecture is ARM Cortex-M4, and was developed for TI's 
[TM4C123 Launchpad](http://www.ti.com/tool/ek-tm4c123gxl) using the [CCS](http://www.ti.com/tool/ccstudio) compiler.

Currently, there are a number of preprocessor functionality switches in the driver:
- `USE_FREERTOS` - Use semaphore-based blocking for read and write operations.
- `USE_DMA_TX` - Use DMA-based write functions.
- `USE_DMA-RX` - Use DMA-based read functions.
- `USE_SCATTERGATHER` - Use scatter-gather (DMA subset functionality) for DMA-based operations.

It should be noted that for simplicity, the driver initializes the uDMAControlTable itself.  If the application already does this, then the two lines:
```c
static uint8_t ui8ControlTable[1024] __attribute__ ((aligned(1024)));
```
```c
ROM_uDMAControlBaseSet(ui8ControlTable);
```
should be commented out in the driver code.

## To-do

- Modify DMA interrupt to re-initialize itself for multi-block transfers.
- Add sample project for testing.
