/*
 * utools - power.c
 * (C) 2022 moatx
 * Released under the GNU GPLv2+, see the COPYING file
 * in the source distribution for its full text.
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>
#include <fcntl.h>
#include <paths.h>
#include <unistd.h>
#include <prop/proplib.h>
#include <sys/envsys.h>
#include <signal.h>
#include <sys/stat.h>
#include "power.h"
#include "utools.h"


/* daemon flag for low power capacity notifier is a w.i.p */
/*
static void daemonize();
*/

static const char *version = "1.0.1";

char *USAGE = "[-cvph]";

/*
static void __dead
usage(int v)
{
	(void)fprintf(stderr, "usage: %s [-cvph]\n", getprogname());
	exit(v);
}
*/

static void
daemonize(void)
{
	/* int fd; */


	puts("initializing daemon...");
	switch (fork()) {
	case -1:
		exit(EXIT_FAILURE);
	case 0:
		break;
	default:
		_exit(0);
	}

	/* On success: The child process becomes session leader */
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* Ignore signal sent from child to parent process */
	signal(SIGCHLD, SIG_IGN);

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");
/*
        if ((fd = open(_PATH_DEVNULL, O_RDWR, 0)) != -1) {
                (void)dup2(fd, STDIN_FILENO);
                (void)dup2(fd, STDOUT_FILENO);
                (void)dup2(fd, STDERR_FILENO);
                if (fd > STDERR_FILENO)
                        (void)close(fd);
        }
	*/


	for (;;) {
		puts("AAA");
		sleep(2);
	}

}



int
main(int argc, char **argv)
{
	int ch;
	setprogname(argv[0]);

	if (argc == 1) {
		printf("%.2f%%\n", getPercent());
		/* printf("%li\n", isConnected()); */
		/* usage(EXIT_SUCCESS); */
	}
	while ((ch = getopt(argc, argv, "pcdhv")) != -1) {
		switch (ch) {
		case 'p':
			printf("%.2f%%\n", getPercent());
			/* rputs(getPercent()); */
			break;
		case 'c':
			printf("is the power cable connected?: %s\n", (isConnected()) ? "yes" : "no");
			break;
		case 'd':
			daemonize();
			break;
		case 'h':
			usage(EXIT_SUCCESS);
		case 'v':
			(void)fprintf(stderr, "%s\n", version);
			exit(EXIT_SUCCESS);
		default:
		case '?':
			usage(EXIT_FAILURE);
		}
	}
	return EXIT_SUCCESS;
}
