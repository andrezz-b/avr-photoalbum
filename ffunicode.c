/*------------------------------------------------------------------------*/
/* Unicode Handling Functions for FatFs R0.13 and Later                   */
/*------------------------------------------------------------------------*/
/* This module will occupy a huge memory in the .rodata section when the  */
/* FatFs is configured for LFN with DBCS. If the system has a Unicode     */
/* library for the code conversion, this module should be modified to use */
/* it to avoid silly memory consumption.                                  */
/*------------------------------------------------------------------------*/
/*
/ Copyright (C) 2022, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
*/


#include "ff.h"

#if FF_USE_LFN != 0	/* This module will be blanked if in non-LFN configuration */

#endif /* #if FF_USE_LFN != 0 */
