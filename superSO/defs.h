#ifndef _DEFS_H
#define	_DEFS_H

#define DEBUG 1

#if DEBUG == 1
	#include <stdio.h>
	#define _DEBUG(c ...) printf("DEBUG: "); printf(c); 
#else
	#define _DEBUG(x)
#endif

#define PIPE_NAME "/tmp/super_so.pipe"
#define STDIN 0
#define STDOUT 1

#endif	/* _DEFS_H */

