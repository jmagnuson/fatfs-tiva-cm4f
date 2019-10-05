/****************************************************************************
*
* Copyright (C) 2014, Jon Magnuson <my.name at google's mail service>
* All Rights Reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*
****************************************************************************/


/* Standard includes. */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Platform includes. */
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/udma.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* Application includes. */
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "sd_util.h"

#define RED_LED   GPIO_PIN_1
#define BLUE_LED  GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#if 0 &&/*defined(ccs) || */defined ( __TMS470__ )
#include "arm_atomic.h"
#endif

/* Size of memcpy buffer */
#define MEM_BUFFER_SIZE         1024

typedef struct TaskParameters_t
{
  SemaphoreHandle_t *pcSemaphores;
  SD_Struct *sdParams;
  uint32_t buffer[MEM_BUFFER_SIZE]; // dst buffer

} TaskParameters;

/* Application task prototypes. */
void prvProducerTask(void *pvParameters);

void prvConsumerTask(void *pvParameters);

void prvWriteTask(void *pvParameters);

/* FreeRTOS function/hook prototypes. */
void vApplicationMallocFailedHook(void);

void vApplicationIdleHook(void);

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

void vApplicationTickHook(void);

static void
ConfigureUART0(void)
{
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
  ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

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
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
  ROM_SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_SSI0);

}

void ConfigureLED()
{

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | BLUE_LED | GREEN_LED);

}


static SemaphoreHandle_t isrSemaphore;

static void set_udma_txfer_done(int status)
{
  if (status == 0)
  {
    /* Success */
    xSemaphoreGive(isrSemaphore);
  }
  else
  {
    /* Failed */
    while (1)
    {}
  }
}

int main_rtos(void)
{
  /* Variable declarations */
  static TaskParameters taskParams = {NULL, NULL, NULL};
  static SemaphoreHandle_t pcSemaphores[2] = {NULL, NULL};
  static SD_Struct sd_params = {
      NULL,
      NULL,
      NULL,
      NULL,
      "/",
      "00000.log"

  };

  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                 SYSCTL_OSC_MAIN);

  ConfigureLED();

  ConfigureUART0();

  ConfigureuDMA();

  ConfigureSPI();

  pcSemaphores[0] = xSemaphoreCreateBinary();
  pcSemaphores[1] = xSemaphoreCreateBinary();

  taskParams.pcSemaphores = &pcSemaphores[0];


  if (pcSemaphores[0] == NULL || pcSemaphores[1] == NULL)
  {
    return 1;
  }

  static uint32_t task_result = NULL;


  if (task_result =
          xTaskCreate(
              prvWriteTask,
              (portCHAR *) "prvWriteTask",
              configMINIMAL_STACK_SIZE,
              (void *) &taskParams,
              (tskIDLE_PRIORITY + 1),
              NULL
          )
          != pdTRUE)
  {
    /* Task not created.  Stop here for debug. */
    while (1)
    {}
  }

  vTaskStartScheduler();

  for (;;)
  {}
  return 0;

}


void prvWriteTask(void *pvParameters)
{

  static SD_Struct sd_params = {
      NULL,
      NULL,
      NULL,
      NULL,
      "00000.log",
      "/"
  };

  unsigned char *file_write_buffer = (uint32_t *) (((TaskParameters *) pvParameters)->buffer);
  uint32_t i = 0;
  unsigned short bytesWritten = 0;
  FRESULT fresult = FR_OK;

  for (; i < DATA_BUFFER_SIZE_DIV2; i++)
  {
    file_write_buffer[i] = (unsigned char) (i & 0xFF);
  }
  if ((fresult = ConfigureSD(&sd_params)) != FR_OK)
  {
    while (1)
    {}
  }

  fresult = f_write(&sd_params.g_sFileObject, file_write_buffer, MEM_BUFFER_SIZE, &bytesWritten);
  if (fresult != FR_OK)
  {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
    while (1)
    {}
  }
  f_sync(&sd_params.g_sFileObject);

  f_close(&sd_params.g_sFileObject);
  f_mount(0, NULL);

  while (1)
  {

    vTaskDelay(500 / portTICK_RATE_MS);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ~ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1));

  }


}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{

}

/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
  static unsigned int led_once = 1;

  if (led_once)
  {
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, (GPIO_PIN_2));
    led_once = 0;
  }Nop();

}

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
  (void) pcTaskName;
  (void) pxTask;

}

/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
  static unsigned int fatfs_timer = 10;

  fatfs_timer--;

  if (fatfs_timer == 0)
  {
    disk_timerproc();
    fatfs_timer = 10;
  }

}

/*-----------------------------------------------------------*/

void vAssertCalled(unsigned long ulLine, const char *const pcFileName)
{


}

