msgsvr: msgsvr.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  ${protol} ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-redis.a ../../dcpots/lib/libdcutil-mysql.a -I../.. --std=c++11 -lhiredis ${evl} ../lib/libawolproto.a -pthread -lmysqlclient ${protoi}

test: test.cpp
	g++ -g -o $@ $^ ../lib/libawolmsg.a ../../dcpots/lib/libdcrpc.a  ${protol} ../../dcpots/lib/libdcbase.a -I../../ --std=c++11 ${protoi}

install: msgsvr
	mkdir ../bin/ -p
	mv msgsvr ../bin/
	cp ./init_db.sh ../bin/

clean:
	rm -f msgsvr test
