libawolmsg.a: *.cpp
	g++ --std=c++11 -c $^ -I../../ -g ${protoi}
	ar -rcs $@ *.o
clean:
	rm -f *.o
install: libawolmsg.a
	mkdir -p ../lib
	cp -f libawolmsg.a ../lib/
