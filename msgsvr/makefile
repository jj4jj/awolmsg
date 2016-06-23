msgsvr: msgsvr.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  ${protol} ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-redis.a ../../dcpots/lib/libdcutil-mysql.a -I../.. --std=c++11 ${redisl}  ${evl} ../lib/libawolproto.a -pthread -lmysqlclient ${protoi} -ldl

test: test.cpp
	g++ -g -o $@ $^ ../lib/libawolmsg.a ../../dcpots/lib/libdcrpc.a  ${protol} ../../dcpots/lib/libdcbase.a -I../../ --std=c++11 ${protoi} -ldl

install: msgsvr
	mkdir ../bin/ -p
	cp -f  msgsvr ../bin/
	cp ./init_db.sh ../bin/

clean:
	rm -f msgsvr test
