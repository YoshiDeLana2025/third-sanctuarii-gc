#include <stddef.h>
#include <tuxedo/types.h>

#define PROPHECY_NUM 7

typedef struct {
	int width, height;
	const u8 *data;
} Prophecy;

void PROPH_Init();
void PROPH_Next();
Prophecy* PROPH_Get();