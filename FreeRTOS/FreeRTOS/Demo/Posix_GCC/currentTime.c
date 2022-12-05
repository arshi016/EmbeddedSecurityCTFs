#include "currentTime.h"

long getNanotime() {
	long int ns;
    uint64_t all;
    time_t sec;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    sec = spec.tv_sec;
    ns = spec.tv_nsec;
    all = (uint64_t) sec * 1000000000L + (uint64_t) ns;
    return all;
}

uint64_t getEpochSeconds() {
    uint64_t all;
    uint64_t sec;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    sec = spec.tv_sec;
    return sec;
}