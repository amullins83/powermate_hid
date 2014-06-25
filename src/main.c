#include <stdio.h>
#include <stdlib.h>
#include "powermate_hid.h"


int main(int argc, char* argv[])
{
	PowermateHid *powermate = powermate_hid_new();
	printf("Powermate device opened\n");


	powermate_hid_delete(powermate);
	printf("Powermate device closed\n");
	
	return 0;
}
