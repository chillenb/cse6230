CC = gcc

CFLAGS = -g -Wall -fopt-info-all
OPTFLAGS = -O3 -march=native -mtune=native

% : %.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

ALL=sum

all: $(ALL)

clean:
	rm -f $(ALL)

