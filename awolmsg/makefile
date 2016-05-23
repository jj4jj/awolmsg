libawolmsg.a: *.cpp awolmsg.pb.cc
	g++ --std=c++11 -c $^ -I../../ -g
	ar -rcs $@ *.o
awolmsg.pb.cc: awolmsg.proto
	${protoc} awolmsg.proto --cpp_out=.
clean:
	rm -f *.o awolmsg.pb.cc
install: libawolmsg.a
	mkdir -p ../lib
	mv libawolmsg.a ../lib/
