CFLAGS = -std=c++17

a.out: *.cpp
	g++ $(CFLAGS) -o a.out *.cpp

.PHONY: test clean

test: a.out
	./a.out

clean: 
	rm -f a.out