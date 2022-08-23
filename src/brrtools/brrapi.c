#define _brrapi_keep_gens
#include "brrtools/brrapi.h"

BRR_cppstart

static int i_brrapi_error = 0;

void BRRCALL
brrapi_sete(int e) { i_brrapi_error = e; }
int BRRCALL
brrapi_gete(void) { return i_brrapi_error; }

#define _brrapi_e_op(_n_, _i_, _D_) case BRRAPI_E_##_n_: return _D_;
const char *BRRCALL
brrapi_err(void)
{
	switch (i_brrapi_error) {
		_brrapi_e(_brrapi_e_op)
		default: return "Invalid Error";
	}
}
#undef _brrapi_e_op

BRR_cppend
