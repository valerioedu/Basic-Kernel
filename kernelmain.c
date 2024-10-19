#include "../Libs/BasicLibrary/basic_library.h"

typedef struct {
    unsigned int ttbr0;
    unsigned int ttbr1;
} ttbr_entry;

ttbr_entry ttbr[2];

void init_ttbr() {
    ttbr[0].ttbr0 = 0x80000000;
}

int main() {
    init_ttbr();
    printf("ttbr0: %x\n", ttbr[0].ttbr0);
    return 0;
}