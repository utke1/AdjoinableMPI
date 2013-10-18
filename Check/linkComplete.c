#include "ampi/ampi.h"

int main(void) { 
 AMPI_Init_NT(0,0);
 AMPI_Finalize_NT();
 return 0; 
}
