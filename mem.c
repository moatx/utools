#include "mem.h"
#include "utools.h"

/*
static void __dead
usage(int v)
{
	(void)fprintf(stderr, "usage: %s [-utf]\n", getprogname());
	exit(v);
}
*/
char *USAGE = "[-utf]";


int
main(int argc, char **argv)
{
	int ch;
	setprogname(argv[0]);
	if(argc == 1)
		usage(EXIT_SUCCESS);
	while ((ch = getopt(argc, argv, "utf")) != -1) {
		switch (ch) {
		case 'u':
			rputs(getused());
			break;
		case 't':
			rputs(getphy());
			break;
		case 'f':
			rputs(getfree());
			break;
		default:
			usage(EXIT_FAILURE);
		}
	}
}

