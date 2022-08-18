#include "brrtools/brrapi.h"

BRR_cppstart

static int i_brrapi_error = 0;

void BRRCALL
brrapi_sete(int e) { i_brrapi_error = e; }
int BRRCALL
brrapi_gete(void) { return i_brrapi_error; }

const char *BRRCALL
brrapi_err(void)
{
	switch (i_brrapi_error) {
		case BRRAPI_E_SUCCESS: return "Success";
		case BRRAPI_E_MEMERR:  return "Memory Allocation Failure";
		case BRRAPI_E_ARGERR:  return "Invalid Arguments";
		case BRRAPI_E_ARRFULL: return "Array is Full";
		default: return "Invalid Error";
	}
}

BRR_cppend
