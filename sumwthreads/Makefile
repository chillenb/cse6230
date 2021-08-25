CC = gcc

CFLAGS = -g -Wall

% : %.c
	$(CC) $(CFLAGS) $< -o $@ $(LIBS)

ALL=sum

all: $(ALL)

clean:
	rm -f $(ALL)

