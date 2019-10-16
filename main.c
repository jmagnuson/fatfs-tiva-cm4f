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
#include <stdlib.h>

/* Application function prototypes. */
extern int main_simple(void);

extern int main_rtos(void);

/* Application switches. */
#define MAIN_SIMPLE /* Use simple test (or RTOS-based if commented out) */

int main(void)
{
  /* Run main app */
#if defined(MAIN_SIMPLE)
  main_simple();
#else
  main_rtos();
#endif

  return 0;

}
