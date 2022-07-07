/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrcon.h"

#include <stdio.h>
#if defined(_WIN32)
# include <windows.h>
# include <conio.h>
#else
# include <termios.h>
#endif

int BRRCALL
brrcon_pause(void)
{
#if defined(_WIN32)
	getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	struct termios new, before;
	/* https://stackoverflow.com/a/18806671/13528679 */
	tcgetattr(0, &before);       /* get current terminal attirbutes; 0 is the file descriptor for stdin */
	new = before;
	new.c_lflag &= ~ICANON;      /* disable canonical mode */
	new.c_lflag &= ~ECHO;        /* don't echo */
	new.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
	new.c_cc[VTIME] = 0;         /* no timeout */
	tcsetattr(0, TCSANOW, &new); /* set immediately */
	getc(stdin);
	tcsetattr(0, TCSANOW, &before);
#else
	return 1;
#endif
	return 0;
}

int BRRCALL
brrcon_clear(void)
{
#if defined(_WIN32)
	/* https://cboard.cprogramming.com/cplusplus-programming/93806-conio-h-functions-mingw-post672795.html#post672795 */
	COORD a = {0,0};
	DWORD nwrite;
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07, 2000, a, &nwrite);
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	printf("\x1b[1;1H\x1b[2J");
#else
	return 1;
#endif
	return 0;
}
