system :=
ifeq ($(OS),Windows_NT) 
    system := Windows
else
    system := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

all:
	gcc MandelbrotSet.c $(system)/thread.c $(system)/listenKey.c -o viewer
clean:
	rm viewer -f
