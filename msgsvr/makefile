msgsvr: msgsvr.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  -lprotobuf ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-redis.a ../../dcpots/lib/libdcutil-mysql.a -I../.. --std=c++11 /usr/lib/x86_64-linux-gnu/libhiredis.a /usr/lib/x86_64-linux-gnu/libev.a ../awolmsg/awolmsg.pb.cc -pthread -lmysqlclient

test: test.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  -lprotobuf ../../dcpots/lib/libdcbase.a -I../../ --std=c++11 ../proto/awolmsg.pb.cc

clean:
	rm -f msgsvr test
