CC = gcc
CFLAGS := -O2 -Wall -Werror -Wextra -std=c99 -O2 -pipe
LDFLAGS := -lm

render:
	$(CC) $(CFLAGS) -o render main.c $(LDFLAGS)

run: render
	./render
	convert out.ppm out.png
	xdg-open out.ppm

.PHONY: test clean

clean:
	rm render
