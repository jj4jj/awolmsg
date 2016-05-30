libawolproto.a: awolmsg.pb.cc awolapp.pb.cc
	g++ --std=c++11 -c $^ -I../../ -g ${protoi} -O2
	ar -rcs $@ *.o

awolmsg.pb.cc: awolmsg.proto
	${protoc} awolmsg.proto --cpp_out=.
awolapp.pb.cc: awolapp.proto
	${protoc} awolapp.proto --cpp_out=.

clean:
	rm -f *.o *.pb.*
install: libawolproto.a
	mkdir -p ../lib
	cp -f libawolproto.a ../lib/
