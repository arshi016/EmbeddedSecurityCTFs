#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "FreeRTOS.h" 
#include "timers.h"

long getNanotime();
uint64_t getEpochSeconds();