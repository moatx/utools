/*
 * utools - netbsd.c
 * (C) 2022 moatx
 * Released under the GNU GPLv2+, see the COPYING file
 * in the source distribution for its full text.
*/

/*
 * htop - netbsd/Platform.c
 * (C) 2014 Hisham H. Muhammad
 * (C) 2015 Michael McConville
 * (C) 2021 Santhosh Raju
 * (C) 2021 Nia Alarie
 * (C) 2021 htop dev team
 * Released under the GNU GPLv2+, see the COPYING file
 * in the source distribution for its full text.
*/

/* The NetBSD specific power related functions were copied from htop's netbsd/Platform.c, and was modified to do what I wanted it to do, and the rest of the code was made by me*/

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
	free_mem = (uint64_t)uvmexp.pagesize * (uint64_t)uvmexp.free;
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
		err(1, "sysctl");


	mib[0] = CTL_VM;
	mib[1] = VM_UVMEXP2;
	ssize = sizeof(uvmexp);
	if (sysctl(mib, 2, &uvmexp, &ssize, NULL, 0) < 0) {
		err(EXIT_FAILURE, "sysctl vm.uvmexp2 failed");
	}
	free_mem = (uint64_t)uvmexp.pagesize * (uint64_t)uvmexp.free;
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
		err(1, "sysctl");


	return (double) phy_mem;

}





bool
isConnected(void)
{
	prop_dictionary_t dict, fields;
	prop_object_t device, curValue, descField;
	prop_object_iterator_t devIter, fieldsIter;

	bool isConnected = 0;

	int fd = open(_PATH_SYSMON, O_RDONLY);
	if (fd == -1)
		goto error;



	if (prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict) != 0)
		goto error;

	devIter = prop_dictionary_iterator(dict);
	if (devIter == NULL)
		goto error;

	while ((device = prop_object_iterator_next(devIter)) != NULL) {
		prop_object_t fieldsArray = prop_dictionary_get_keysym(dict, device);
		if (fieldsArray == NULL)
			goto error;

		fieldsIter = prop_array_iterator(fieldsArray);
		if (fieldsIter == NULL)
			goto error;

		while ((fields = prop_object_iterator_next(fieldsIter)) != NULL) {
			curValue = prop_dictionary_get(fields, "cur-value");
			descField = prop_dictionary_get(fields, "description");

			if (descField == NULL || curValue == NULL)
				continue;
			if (prop_string_equals_cstring(descField, "connected")) {
				isConnected = prop_number_integer_value(curValue);
			}
		}
	}
	return isConnected;

error:
	if (fd != -1)
		close(fd);
	exit(EXIT_FAILURE);
}

double
getPercent(void)
{
	double percent = 0;
	prop_dictionary_t dict, fields, props;
	prop_object_t device, class, curValue, maxValue, descField;
	prop_object_iterator_t devIter, fieldsIter;

	intmax_t totalCharge = 0;
	intmax_t totalCapacity = 0;
	int isBattery = 0;
	intmax_t isPresent = 1;
	intmax_t curCharge = 0;
	intmax_t maxCharge = 0;

	int fd = open(_PATH_SYSMON, O_RDONLY);
	if (fd == -1)
		goto error;



	if (prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict) != 0)
		goto error;

	devIter = prop_dictionary_iterator(dict);
	if (devIter == NULL)
		goto error;

	while ((device = prop_object_iterator_next(devIter)) != NULL) {
		prop_object_t fieldsArray = prop_dictionary_get_keysym(dict, device);
		if (fieldsArray == NULL)
			goto error;

		fieldsIter = prop_array_iterator(fieldsArray);
		if (fieldsIter == NULL)
			goto error;

		while ((fields = prop_object_iterator_next(fieldsIter)) != NULL) {
			props = prop_dictionary_get(fields, "device-properties");
			if (props != NULL) {
				class = prop_dictionary_get(props, "device-class");

				if (prop_string_equals_cstring(class, "battery")) {
					isBattery = 1;
				}
				continue;
			}
			curValue = prop_dictionary_get(fields, "cur-value");
			maxValue = prop_dictionary_get(fields, "max-value");
			descField = prop_dictionary_get(fields, "description");

			if (descField == NULL || curValue == NULL)
				continue;
			if (prop_string_equals_cstring(descField, "present")) {
				isPresent = prop_number_integer_value(curValue);
			} else if (prop_string_equals_cstring(descField, "charge")) {
				if (maxValue == NULL)
					continue;
				curCharge = prop_number_integer_value(curValue);
				maxCharge = prop_number_integer_value(maxValue);
			}
		}

		if (isBattery && isPresent == 0)
			goto error;

		totalCharge += curCharge;
		totalCapacity += maxCharge;
	}
	percent = ((double)totalCharge / (double)totalCapacity) * 100.0;
	return percent;

error:
	if (fd != -1)
		close(fd);
	exit(EXIT_FAILURE);
}
