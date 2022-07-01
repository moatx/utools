#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <paths.h>
#include <unistd.h>
#include <kvm.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <net/if.h>
#include <prop/proplib.h>
#include <prop/proplib.h>
#include <sys/envsys.h>
#include <sys/iostat.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>

typedef enum ACPresence_ {
   AC_ABSENT,
   AC_PRESENT,
   AC_ERROR
} ACPresence;

int
main(void)
{
	double *percent;
	prop_dictionary_t dict, fields, props;
	prop_object_t device, class;

	intmax_t totalCharge = 0;
	intmax_t totalCapacity = 0;

	int fd = open(_PATH_SYSMON, O_RDONLY);
	if (fd == -1)
		goto error;



	if (prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict) != 0)
		goto error;

	prop_object_iterator_t devIter = prop_dictionary_iterator(dict);
	if (devIter == NULL)
		goto error;

	while ((device = prop_object_iterator_next(devIter)) != NULL) {
		prop_object_t fieldsArray = prop_dictionary_get_keysym(dict, device);
		if (fieldsArray == NULL)
			goto error;

		prop_object_iterator_t fieldsIter = prop_array_iterator(fieldsArray);
		if (fieldsIter == NULL)
			goto error;
		bool isACAdapter = false;
		bool isBattery = false;

		intmax_t isPresent = 1;
		intmax_t isConnected = 0;
		intmax_t curCharge = 0;
		intmax_t maxCharge = 0;

		while ((fields = prop_object_iterator_next(fieldsIter)) != NULL) {
			props = prop_dictionary_get(fields, "device-properties");
			if (props != NULL) {
				class = prop_dictionary_get(props, "device-class");

				if (prop_string_equals_cstring(class, "ac-adapter")) {
					isACAdapter = true;
				} else if (prop_string_equals_cstring(class, "battery")) {
					isBattery = true;
				}
				continue;
			}
			prop_object_t curValue = prop_dictionary_get(fields, "cur-value");
			prop_object_t maxValue = prop_dictionary_get(fields, "max-value");
			prop_object_t descField = prop_dictionary_get(fields, "description");

			if (descField == NULL || curValue == NULL)
				continue;
      if (prop_string_equals_cstring(descField, "connected")) {
            isConnected = prop_number_integer_value(curValue);
         } else if (prop_string_equals_cstring(descField, "present")) {
            isPresent = prop_number_integer_value(curValue);
         } else if (prop_string_equals_cstring(descField, "charge")) {
            if (maxValue == NULL)
               continue;
            /*curCharge*/ totalCharge = prop_number_integer_value(curValue);
            curCharge = prop_number_integer_value(curValue);
            maxCharge = prop_number_integer_value(maxValue);
         }
      }

      if (isBattery && isPresent) {
	    /*
         totalCharge += curCharge;
	 */
         totalCapacity += maxCharge;
      }

		/*
		if (isACAdapter && *isOnAC != AC_PRESENT) {
			*isOnAC = isConnected ? AC_PRESENT : AC_ABSENT;
		}
		*/
	}

	*percent = ((double)totalCharge / (double)totalCapacity) * 100.0;
	printf("%d %d %d %.2f%% \n", *percent, totalCharge, totalCapacity, (totalCharge / totalCharge) * 100.0 );

error:
	if (fd != -1)
		close(fd);

}
