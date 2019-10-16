
// Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Tivaware
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

// Driverlib includes
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"

// SD Includes
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "sd_util.h"

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

volatile uint32_t tick_counter = 0;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

/* DMA control table */
//#pragma DATA_ALIGN(ui8ControlTable, 1024)
//extern uint8_t ui8ControlTable[1024] __attribute__ ((aligned(1024)));

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART0(void)
{
  //
  // Enable the GPIO Peripheral used by the UART.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  //
  // Enable UART0
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  //
  // Configure GPIO Pins for UART mode.
  //
  ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
  ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  //
  // Use the internal 16MHz oscillator as the UART clock source.
  //
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

  //
  // Initialize the UART for console I/O.
  //
  UARTStdioConfig(0, 115200, 16000000);
}

static void
ConfigureuDMA()
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
  ROM_uDMAEnable();
  //ROM_uDMAControlBaseSet(ui8ControlTable);
}

static void
ConfigureSPI(void)
{

  // all this happens in power_on()...
  //
  // Initialize SPI
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_SSI0);

}

unsigned char file_write_buffer[DATA_BUFFER_SIZE];
unsigned char file_read_buffer[DATA_BUFFER_SIZE];

int
main_simple(void)
{

  static SD_Struct sd_struct = {
      NULL,
      NULL,
      NULL,
      NULL,
      "/",
      "00000.log"

  };
  //unsigned char *data_array;
  //unsigned int data_array_length = 0;
  //unsigned char *file_data_buffer;
  //unsigned char *file_write_buffer;
  FRESULT fresult = FR_OK;
  WORD bytesWritten = 0;
  //unsigned int writeCounter=0;
  uint32_t ui32SysClock;


  //
  // Setup the system clock to run at 50 Mhz from PLL with crystal reference
  //
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                 SYSCTL_XTAL_16MHZ);

  //
  // Enable and configure the GPIO port for the LED operation.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);


  //
  // Enable interrupts to the processor.
  //
  ROM_IntMasterEnable();

  //
  // Initialize the UART and configure it for 115,200, 8-N-1 operation.
  //
  ConfigureUART0();

  ConfigureuDMA();

  ConfigureSPI();

  /* Set up SysTick timer */
  ui32SysClock = SysCtlClockGet();
  SysTickPeriodSet(ui32SysClock);
  SysTickIntEnable();
  SysTickEnable();


  volatile uint32_t i = 0;
  for (i = 0; i < DATA_BUFFER_SIZE; i++)
  {
    file_write_buffer[i] = (unsigned char) (i & 0xFF);
  }

  Nop();
  i = tick_counter;

  // Initialize SD
  fresult = ((FRESULT) ConfigureSD(&sd_struct));
  if (fresult != FR_OK)
  {
//		UARTprintf("SD didn't initialize\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, (GPIO_PIN_3));
    while (1)
    {}
  }
  else
  {
//		UARTprintf("SD Initialized.\n");
  }

  i = 50e6;
  while (i--)
  {}

  Nop();

  volatile unsigned int t0r = tick_counter;
  volatile unsigned int t0 = (SysTickValueGet());

#if 1 /* only for testing, no longer needed */
  fresult = f_write(&sd_struct.g_sFileObject, file_write_buffer, DATA_BUFFER_SIZE, &bytesWritten);
  if (fresult != FR_OK)
  {
    //Nop();
//		UARTprintf("File didn't write\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, (GPIO_PIN_3));

    while (1)
    {}
  }
  else
  {
//		UARTprintf("File written\n");
  }
  f_sync(&sd_struct.g_sFileObject);
#endif

  volatile unsigned int t1r = tick_counter;
  volatile unsigned int t1 = (SysTickValueGet());

/* Go back to beginning of file */
  fresult = f_lseek(&sd_struct.g_sFileObject, 0);

  volatile unsigned int t2r = tick_counter;
  volatile unsigned int t2 = (SysTickValueGet());

#if 1 /* only for testing, no longer needed */
  fresult = f_read(&sd_struct.g_sFileObject, file_read_buffer, DATA_BUFFER_SIZE, &bytesWritten);
  if (fresult != FR_OK)
  {
    //Nop();
//        UARTprintf("File didn't read\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, (GPIO_PIN_3));

    while (1)
    {}
  }
  else
  {
//        UARTprintf("File read\n");
  }
#endif

  volatile unsigned int t3r = tick_counter;
  volatile unsigned int t3 = (SysTickValueGet());

  t0r = tick_counter;
  t0 = (SysTickValueGet());

#if 1 /* only for testing, no longer needed */
  fresult = f_write(&sd_struct.g_sFileObject, file_write_buffer, DATA_BUFFER_SIZE, &bytesWritten);
  if (fresult != FR_OK)
  {
    //Nop();
//		UARTprintf("File didn't write\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, (GPIO_PIN_3));

    while (1)
    {}
  }
  else
  {
//		UARTprintf("File written\n");
  }
  f_sync(&sd_struct.g_sFileObject);
#endif

  t1r = tick_counter;
  t1 = (SysTickValueGet());


  f_close(&sd_struct.g_sFileObject);
  f_mount(0, NULL);

  Nop();

  for (; i < DATA_BUFFER_SIZE; i++)
  {
    if (file_write_buffer[i] != file_read_buffer[i])
    {
      GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, (GPIO_PIN_3));

      while (1)
      {}
    }
  }


  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, (GPIO_PIN_2));

  //
  // Loop Forever
  //
  while (1)
  {

  }
}


void
SysTickHandler(void)
{

  tick_counter++;
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));

}
