/*
 * utools - power_netbsd.c
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

/* The NetBSD specific functions were copied from htop's netbsd/Platform.c, and was modified to do what I wanted it to do */

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
