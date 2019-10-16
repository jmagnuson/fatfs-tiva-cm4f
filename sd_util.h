#ifndef SD_UTIL_H_
#define SD_UTIL_H_

#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"

#define PATH_BUF_SIZE 80
#define FILENAME_BUF_SIZE 12

#define DATA_BUFFER_SIZE 8192
#define DATA_BUFFER_SIZE_DIV2 DATA_BUFFER_SIZE/2

#define Nop(); asm(" nop");

typedef struct SD_Struct
{
  FATFS g_sFatFs;
  DIR g_sDirObject;
  FILINFO g_sFileInfo;
  FIL g_sFileObject;
  char g_cCwdBuf[PATH_BUF_SIZE];
  char log_filename[FILENAME_BUF_SIZE];
} SD_Struct;

unsigned int ConfigureSD(SD_Struct *);

#endif /* SD_UTIL_H_ */
