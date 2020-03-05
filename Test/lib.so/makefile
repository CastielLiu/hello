lib:
	g++ lib.cpp -I . -fPIC -shared -o libtest.so

	g++ -o test1 a.cpp libtest.so -ltest -L. -Wl,-rpath=.
	
	g++ -o test2 b.cpp libtest.so -ltest -L. -Wl,-rpath=.
	
clean:
	rm test*
