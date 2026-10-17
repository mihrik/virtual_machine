#include <stdio.h>
#include "colors.h"

int main(void)
{
    PRINT_COLOR(BLUE, " %X%X", 0xFEDCBA98 >> 28 & 0xF, 0xFEDCBA98 >> 24 & 0xF);
    PRINT_COLOR(BLUE, " %X%X", 0xFEDCBA98 & 0x00F00000, 0xFEDCBA98 & 0x000F0000);
    PRINT_COLOR(BLUE, " %X%X", 0xFEDCBA98 & 0x0000F000, 0xFEDCBA98 & 0x00000F00);
    PRINT_COLOR(BLUE, " %X%X", 0xFEDCBA98 & 0x000000F0, 0xFEDCBA98 & 0x0000000F);

    return 0;
}