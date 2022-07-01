#include <sys/types.h>
#include <sys/sysctl.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <util.h>

/*
 * openbsd memory display util
 */

static struct uvmexp uvm;
static u_int8_t uflag, mflag, fflag;
static u_int64_t phy_mem, used_mem, free_mem;

void fprint(u_int64_t size);
static void usage(void);

int
main(int argc, char **argv)
{
	int ch;
	int mib[2];
	size_t len;

#ifdef __OpenBSD__
	if (pledge("stdio ps vminfo", NULL) == -1)
		err(1, "pledge");
#endif

	while ((ch = getopt(argc, argv, "umf")) != -1) {
		switch (ch) {
		case 'u':
			uflag = 1;
			break;
		case 'm':
			mflag = 1;
			break;
		case 'f':
			fflag = 1;
			break;
		default:
			usage();
		}
	}
    /*
    if(argc >= 2)
      usage();
    else{
      uflag = 1;
      mflag = 1;
      fflag = 1;
    }
    */

    /*
     * XXX If argc is 2 their wont be anything printed
     */

	if (argc == 1) {
		uflag = 1;
		mflag = 1;
		fflag = 1;
	}

	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM64;
	len = sizeof(phy_mem);

	if (sysctl(mib, 2, &phy_mem, &len, NULL, 0) == -1)
		err(1, "sysctl");

	mib[0] = CTL_VM;
	mib[1] = VM_UVMEXP;
	len = sizeof(uvm);

	if (sysctl(mib, 2, &uvm, &len, NULL, 0) == -1)
		err(1, "sysctl");

	free_mem = (u_int64_t)uvm.pagesize * uvm.free;
	used_mem = phy_mem - free_mem;

	if (uflag) {
		printf("USED: ");
		fprint(used_mem);
	}
	if (mflag) {
		printf("MEM: ");
		fprint(phy_mem);
	}
	if (fflag) {
		printf("FREE: ");
		fprint(free_mem);
	}

	printf("\n");
	return 0;
}

void
fprint(u_int64_t size)
{
	char *human_size;
	human_size = malloc(FMT_SCALED_STRSIZE);
	fmt_scaled(size, human_size);
	printf("%s ", human_size);
	free(human_size);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-umf]\n",
	    getprogname());
	exit(1);
}
