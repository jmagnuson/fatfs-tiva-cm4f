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
- `USE_DMA_RX` - Use DMA-based read functions.
- `USE_SCATTERGATHER` - Use scatter-gather (DMA subset functionality) for DMA-based operations.

It should be noted that for simplicity, the driver initializes the uDMAControlTable itself.  If the application already does this, then the two lines:
```c
static uint8_t ui8ControlTable[1024] __attribute__ ((aligned(1024)));
```
```c
ROM_uDMAControlBaseSet(ui8ControlTable);
```
should be commented out in the driver code.

## Benchmarks

Micro-benchmark tests were performed with various modifications to the driver to determine how much
throughput can be achieved for MMC/SD.  Unless otherwise noted, `ARB_SIZE`=4, `BIT_WIDTH`=8, and `SSI_FRF_MOTO_MODE_0` are used.
SPI was set to 8 MHz and the transfer size was 8192 bytes.

Times are given in milliseconds (lower = better) and the corresponding ratios (higher = better) are given as normalized
inverse to _"no dma"_, which was the slowest of the set.

|  mode         | t_write (ms) | t_read (ms) | *_write | *_read |
|  ----         | ------------ | ----------- | ------- | ------ | 
|  no dma       |  47.12       |  47.40      |  1.00   |  1.00  |
|  dma          |  13.56       |  11.22      |  3.47   |  4.23  |
|  dma-sg       |  13.50       |  11.13      |  3.49   |  4.26  |
|  no dma 16    |  33.47       |  31.33      |  1.41   |  1.51  |
|  dma 16       |  21.09       |  18.83      |  2.23   |  2.52  |
|  dma 16, arb8 |  21.17       |  18.83      |  2.23   |  2.52  |
|  dma, moto3   |  12.36       |  9.67       |  3.81   |  4.90  |

![FatFS benchmarks chart](https://raw.githubusercontent.com/jmagnuson/fatfs-tiva-cm4f/gh-pages/img/benchmarks01.png "FatFS benchmarks")

## To-do

- Modify DMA interrupt to re-initialize itself for multi-block transfers.
- Add sample project for testing.
