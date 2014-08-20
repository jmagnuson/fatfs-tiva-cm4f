# FatFs Tiva-cm4f port

## Description

This library file modifies an existing driver provided by TI and adds DMA
functionality for sector transmissions.

## Notes

The target architecture is ARM Cortex-M4, and was developed for TI's 
[TM4C123 Launchpad](http://www.ti.com/tool/ek-tm4c123gxl).

## To-do

- Add FreeRTOS process-yielding support.
- Add DMA read functionality.
- Modify DMA interrupt to re-initialize itself for multi-block transfers.
- Add sample project for testing.