demo_cli: ./demo_cli.cpp
	g++ -o $@ $^ ${protol} --std=c++11

demo_svr: ./demo_svr.cpp test.pb.cc ../lib/libawolapp.a ../lib/libawolmsg.a ../../dcpots/lib/libdcrpc.a ../../dcpots/lib/libdcbase.a
	g++ -o $@ $^ ${protol} --std=c++11 -I ../../ -g -I../awolmsg ${protoi}

test.pb.cc: test.proto
	${protoc} test.proto --cpp_out=.

clean:
	rm -f demo_cli demo_svr *.pb.*

install: demo_cli demo_svr
