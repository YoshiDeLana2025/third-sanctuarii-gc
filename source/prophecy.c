#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ogc/lwp_watchdog.h>

#include "prophecy.h"

#include "prophecy-1_raw.h"
#include "prophecy-2_raw.h"
#include "prophecy-3_raw.h"
#include "prophecy-4_raw.h"
#include "prophecy-5_raw.h"
#include "prophecy-6_raw.h"
#include "prophecy-7_raw.h"

static Prophecy* prophecies = NULL;
static int prophecy_index = 0;

void PROPH_Init() {
	prophecies = malloc(sizeof(Prophecy) * PROPHECY_NUM);
	
	prophecies[0].width = 434;
	prophecies[0].height = 25;
	prophecies[0].data = prophecy_1_raw;

	prophecies[1].width = 496;
	prophecies[1].height = 25;
	prophecies[1].data = prophecy_2_raw;

	prophecies[2].width = 385;
	prophecies[2].height = 48;
	prophecies[2].data = prophecy_3_raw;

	prophecies[3].width = 431;
	prophecies[3].height = 25;
	prophecies[3].data = prophecy_4_raw;

	prophecies[4].width = 468;
	prophecies[4].height = 25;
	prophecies[4].data = prophecy_5_raw;

	prophecies[5].width = 299;
	prophecies[5].height = 48;
	prophecies[5].data = prophecy_6_raw;

	prophecies[6].width = 299;
	prophecies[6].height = 49;
	prophecies[6].data = prophecy_7_raw;

	// Shuffle
	if (PROPHECY_NUM > 1) {
		srand(gettime());
    size_t ii;
    for (ii = 0; ii < PROPHECY_NUM - 1; ii++) {
      size_t jj = ii + rand() / (RAND_MAX / (PROPHECY_NUM - ii) + 1);
      Prophecy p = prophecies[jj];
      prophecies[jj] = prophecies[ii];
      prophecies[ii] = p;
    }
  }
}

void PROPH_Next() {
	prophecy_index = (prophecy_index + 1) % PROPHECY_NUM;
}

Prophecy* PROPH_Get() {
	return &prophecies[prophecy_index];
}