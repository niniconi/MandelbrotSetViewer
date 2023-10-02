system := Linux

all:
	gcc MandelbrotSet.c $(system)/thread.c $(system)/listenKey.c -o viewer
clean:
	rm viewer -f
