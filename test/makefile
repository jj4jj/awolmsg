test: demo_cli demo_svr


demo_cli: ./demo_cli.cpp
	g++ -o $@ $^ -lprotobuf --std=c++11

demo_svr: ./demo_svr.cpp  ../proto/awolmsg.pb.cc  ../lib/libawolmsg.a ../../dcpots/lib/libdcrpc.a ../../dcpots/lib/libdcbase.a
	g++ -o $@ $^ -lprotobuf --std=c++11 -I ../../
test.pb.cc: test.proto
	protoc test.proto --cpp_out=.

../proto/awolmsg.pb.cc: ../proto/awolmsg.proto
	cd ../proto/ && protoc awolmsg.proto --cpp_out=.


