#!/bin/bash
protoc=protoc
protol="-lprotobuf"
if [ "x$1" = "xu1" ];then
    protoc="../protobuf/bin/protoc"
    protol="../protobuf/lib/libprotobuf.a"
fi

cd ./awolmsg && make install protoc="${protoc}" protol="${protol}"
cd -
cd ./app/ && make install protoc="${protoc}" protol="${protol}"
cd -
cd ./test/ && make install protoc="${protoc}" protol="${protol}"
cd -
cd ./msgsvr/ && make install protoc="${protoc}" protol="${protol}"
