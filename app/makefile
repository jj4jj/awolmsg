libawolapp.a: ./*.cpp
	g++ --std=c++11 -c $^ -I../..  -I../awolmsg/ ${protoi}
	ar -rcs $@ *.o

install: libawolapp.a
	mkdir ../lib/ -p
	mv libawolapp.a ../lib/

clean:
	rm -f *.o


