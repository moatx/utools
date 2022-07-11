/*-
 * Copyright 2022 moatx. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
