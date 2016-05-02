libawolmsg.a: *.cpp
	g++ --std=c++11 -c $^ -I../../
	ar -rcs $@ *.o
../proto/awolmsg.pb.cc: ../proto/awolmsg.proto
	cd ../proto/ && protoc awolmsg.proto --cpp_out=.
clean:
	rm -f *.o
install:
	mv *.a ../lib/
	cp *.h ../include/
