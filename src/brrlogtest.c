#include <brrtools/brrlog.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static inline void a(const char *const msg, ...)
{
	brrsz i = strlen(msg);
	//printf("Checking input of %zu \x1b[33m'%s'\x1b[0m\n", i, msg);
	va_list lptr;
	va_start(lptr, msg);
	brrsz mlen = brrlogv((brrlog_parms_t){0}, msg, lptr);
	va_end(lptr);

	printf("Got output length of %zu\n", mlen);
}

#define err(_msg_) fprintf(stderr, _msg_": %s (%i) (%s (%i))\n", brrapi_err(), brrapi_gete(), strerror(errno), errno)
int main(int argc, char **argv)
{
	if (brrlog_init(brrlog_default_cfg, "(!", "!)")) {
		err("Failed to init brrlog");
		return 1;
	}
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("");
	a("(!Giggity!)");
	a("(!1:Giggity!) and (!3:some more!)");
	a("BEFORE (!1:Giggity!) BETWEEN (!3:some more!) AFTER");
	a("(!0:!)");
	a("(!:!)");
	a("(!!)");
	a("( ! ! )");
	a("(!31:Lots (!1:of (!2:nested (!3:styled (!4:text (!5:sections (!6:dear lord!)!)!)!)!)!) can I get some help please!)");
	a("(!          :              !)");
	brrlog_deinit();
}
