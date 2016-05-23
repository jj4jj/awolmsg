awolapp.pb.cc: awolapp.proto
	${protoc} $^ --cpp_out=. -I. -I../awolmsg

libawolapp.a: ./*.cpp awolapp.pb.cc
	g++ --std=c++11 -c $^ -I../..  -I../awolmsg/ ${protoi}
	ar -rcs $@ *.o

install: libawolapp.a
	mkdir ../lib/ -p
	mv libawolapp.a ../lib/

clean:
	rm -f awolapp.pb.* *.o


