#ifndef UTOOLS_H
#define UTOOLS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char* USAGE;

static void __dead
usage(int v)
{
	(void)fprintf(stderr, "usage: %s %s\n", getprogname(), USAGE);
	exit(v);
}

/* human readable puts W.I.P */
static void
rputs(double size)
{
	int i = 0;
	const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (size > 1024) {
		size /= 1024;
		i++;
	}
	printf("%.*f %s\n", i, size, units[i]);
}
#endif
