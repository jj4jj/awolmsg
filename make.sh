#!/bin/bash
protoc=protoc
protol="-lprotobuf"
if [ "x$1" = "xu1" ];then
    protoc="../../protobuf/bin/protoc"
    protol="../../protobuf/lib/libprotobuf.a"
    protoi="-I../../protobuf/include"
fi

cd ./awolmsg && make install protoc="${protoc}" protol="${protol}" protoi="${protoi}"
cd -
cd ./app/ && make install protoc="${protoc}" protol="${protol}" protoi="${protoi}"
cd -
cd ./test/ && make install protoc="${protoc}" protol="${protol}" protoi="${protoi}"
cd -
cd ./msgsvr/ && make install protoc="${protoc}" protol="${protol}" protoi="${protoi}"
