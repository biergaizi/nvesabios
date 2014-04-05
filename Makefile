CFLAGS = -O3 -Wall -Wextra -std=c99

all:
	gcc nvesabios.c -o nvesabios ${CFLAGS}
clean:
	rm -rf nvesabios
