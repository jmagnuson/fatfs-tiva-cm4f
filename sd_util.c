#include <stdlib.h>
#include <string.h>
#include "sd_util.h"

unsigned int ConfigureSD(SD_Struct *);

unsigned int
ConfigureSD(SD_Struct *sd_struct)
{
  FRESULT fresult = FR_OK;

  FATFS *g_sFatFs = &sd_struct->g_sFatFs;
  FIL *g_sFileObject = &sd_struct->g_sFileObject;
  char *log_filename = &(sd_struct->log_filename)[0];

  fresult = f_mount(0, g_sFatFs);
  if (fresult != FR_OK)
  {
    return fresult;
  }

  strncpy(log_filename, "test.log", (FILENAME_BUF_SIZE) - 1);

  fresult = f_open(g_sFileObject, log_filename, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
  if (fresult != FR_OK)
  {
    return fresult;
  }

  return FR_OK;
}
