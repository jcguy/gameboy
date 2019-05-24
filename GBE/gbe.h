#ifndef __GBE_H__
#define __GBE_H__

/* System Specific Compiliation Issue Resolver */
#if defined(SONYPSX)

	#include "psx.h"

#elif defined(DREAMCAST)

#elif defined (WIN32)

	/* Win32 */
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <setjmp.h>

	#define inline __inline

#else

/* Linux */
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <setjmp.h>

#endif /* End of SSCIR */

jmp_buf EmuJmp;

#endif /* __GBE_H__ */
