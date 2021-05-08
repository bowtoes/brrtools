#include <stdio.h>
#include <stdlib.h>

#include <brrtools/brrlog.h>
#include <brrtools/brrlib.h>

int main(void)
{
	brrlog_setlogmax(0);
	brrlog_styleon = true;
	brrlog_debugon = true;
	while (1) {
		BRRLOG_NOR("Time: %f", (double)brrlib_mutime()/1000000);
	}
}
