CC ?= gcc
CFLAGS += -Wall -Werror -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith  -Wsystem-headers     -Wa,--fatal-warnings  -Wreturn-type -Wswitch -Wshadow -Wcast-qual -Wwrite-strings -Wextra -Wno-sign-compare -Wold-style-definition -Wconversion -Wsign-compare -Wformat=2   -Wno-discarded-qualifiers -Wno-unused-function

OS != uname | tr '[:upper:]' '[:lower:]'

all: mem 
#power

mem:
	$(CC) $(CFLAGS) mem.c mem_$(OS).c -o mem 

#power:
#	$(CC) $(CFLAGS) power.c power_$(OS).c -o power -lprop

clean:
	@rm -f mem power



.PHONY: all clean
