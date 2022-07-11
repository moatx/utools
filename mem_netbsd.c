#include <stdio.h>
#include <stdlib.h>
#include <uvm/uvm_extern.h>
#include <sys/sysctl.h>
#include <err.h>
#include "mem.h"

/* get free memory */
double
getfree(void)
{
	size_t ssize;
	int mib[2];
	struct uvmexp_sysctl uvmexp;
	uint64_t phy_mem, free_mem;


	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM64;
	ssize = sizeof(phy_mem);

	if (sysctl(mib, 2, &phy_mem, &ssize, NULL, 0) == -1)
		err(1, "sysctl");


	mib[0] = CTL_VM;
	mib[1] = VM_UVMEXP2;
	ssize = sizeof(uvmexp);
	if (sysctl(mib, 2, &uvmexp, &ssize, NULL, 0) < 0) {
		err(EXIT_FAILURE, "sysctl vm.uvmexp2 failed");
	}
	free_mem = (uint64_t) uvmexp.pagesize * (uint64_t) uvmexp.free;
	return (double)free_mem;

}
/* get used memory */
double
getused(void)
{
	size_t ssize;
	int mib[2];
	struct uvmexp_sysctl uvmexp;
	uint64_t phy_mem, used_mem, free_mem;


	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM64;
	ssize = sizeof(phy_mem);

	if (sysctl(mib, 2, &phy_mem, &ssize, NULL, 0) == -1)
		err(EXIT_FAILURE, "sysctl");


	mib[0] = CTL_VM;
	mib[1] = VM_UVMEXP2;
	ssize = sizeof(uvmexp);
	if (sysctl(mib, 2, &uvmexp, &ssize, NULL, 0) < 0) {
		err(EXIT_FAILURE, "sysctl vm.uvmexp2 failed");
	}
	free_mem = (uint64_t) uvmexp.pagesize * (uint64_t) uvmexp.free;
	used_mem = phy_mem - free_mem;
	return (double)used_mem;

}
/* get total physical memory */
double
getphy(void)
{
	size_t ssize;
	int mib[2];
	uint64_t phy_mem;


	mib[0] = CTL_HW;
	mib[1] = HW_PHYSMEM64;
	ssize = sizeof(phy_mem);

	if (sysctl(mib, 2, &phy_mem, &ssize, NULL, 0) == -1)
		err(EXIT_FAILURE, "sysctl");


	return (double)phy_mem;

}
