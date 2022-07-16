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
is_connected(void)
{
	prop_dictionary_t dict, fields;
	prop_object_t device, curvalue, descfield;
	prop_object_iterator_t diter, fiter;

	bool connected = 0;

	int fd = open(_PATH_SYSMON, O_RDONLY);
	if (fd == -1)
		goto error;

	if (prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict) != 0)
		goto error;

	diter = prop_dictionary_iterator(dict);
	if (diter == NULL)
		goto error;

	while ((device = prop_object_iterator_next(diter)) != NULL) {

		if ((fiter = prop_array_iterator(prop_dictionary_get_keysym(dict, device))) == NULL)
			goto error;

		while ((fields = prop_object_iterator_next(fiter)) != NULL) {
			curvalue = prop_dictionary_get(fields, "cur-value");
			descfield = prop_dictionary_get(fields, "description");

			if (descfield == NULL || curvalue == NULL)
				continue;

			if (prop_string_equals_cstring(descfield, "connected")) {
				connected = prop_number_integer_value(curvalue);
			}
		}
	}
	return connected;

error:
	if (fd != -1)
		close(fd);
	exit(EXIT_FAILURE);
}

double
get_percent(void)
{
	double percent = 0;
	prop_dictionary_t dict, fields;
	prop_object_t device, curvalue, maxvalue, desc;
	prop_object_iterator_t diter, fiter;

	intmax_t charge = 0;
	intmax_t capacity = 0;

	int fd = open(_PATH_SYSMON, O_RDONLY);
	if (fd == -1)
		goto error;

	if (prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict) != 0)
		goto error;

	if ((diter = prop_dictionary_iterator(dict)) == NULL)
		goto error;

	while ((device = prop_object_iterator_next(diter)) != NULL) {

		if ((fiter = prop_array_iterator(prop_dictionary_get_keysym(dict, device))) == false)
			goto error;

		while ((fields = prop_object_iterator_next(fiter)) != NULL) {

			curvalue = prop_dictionary_get(fields, "cur-value");
			maxvalue = prop_dictionary_get(fields, "max-value");
			desc = prop_dictionary_get(fields, "description");

			if (desc == NULL || curvalue == NULL)
				continue;
			if (prop_string_equals_cstring(desc, "present")) {
				if(prop_number_integer_value(curvalue) == 0 )
					goto error;
			} else if (prop_string_equals_cstring(desc, "charge")) {
				if (maxvalue == NULL)
					continue;
				charge = prop_number_integer_value(curvalue);
				capacity = prop_number_integer_value(maxvalue);
			}
		}
	}
	percent = ((double)charge / (double)capacity) * 100.0;
	return percent;

error:
	if (fd != -1)
		close(fd);
	exit(EXIT_FAILURE);
}
