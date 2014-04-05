CFLAGS = -O3 -Wall -Wextra -std=c99
DEFINES = -D_GNU_SOURCE

all:
	gcc nvesabios.c -o nvesabios ${CFLAGS} ${DEFINES}
clean:
	rm -rf nvesabios
