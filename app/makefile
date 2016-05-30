libawolappi.a: ./*.cpp
	g++ --std=c++11 -c $^ -I../..  -I../awolmsg/ ${protoi}
	ar -rcs $@ *.o

install: libawolappi.a
	mkdir ../lib/ -p
	cp -f libawolappi.a ../lib/
	cd ../lib && rm *.o -f && ar x libawolappi.a && ar x libawolproto.a && ar x libawolmsg.a && rm libawolapp.a -f && ar -rcs libawolapp.a *.o && rm *.o -f

clean:
	rm -f *.o


