msgsvr: msgsvr.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  -lprotobuf ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-redis.a  -I../.. --std=c++11 /usr/lib/x86_64-linux-gnu/libhiredis.a /usr/lib/x86_64-linux-gnu/libev.a

test: test.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  -lprotobuf ../../dcpots/lib/libdcbase.a -I../../ --std=c++11
