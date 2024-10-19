#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint64_t ttbr0;
    uint64_t ttbr1;
} ttbr_entry;

ttbr_entry ttbr[2];

void init_ttbr() {
    ttbr[0].ttbr0 = 0x80000000;
}