import redis
import json


class MsgSvr():
    def __init__(self):
        self.r=redis.Redis(host='localhost',port=6379, db=0)
        self.peers=[]
        #connect to redis server

    def set(self, actor, type, qid, msg):
        #hset
        hset=self.mergekey(actor, type)
        if self.r.hlen(hset) > 100:
            return None
        return self.r.hset(hset, qid, msg)

    def alive(self, actor):
        pass

    def mergekey(self, actor, type):
        return 'u1$msg$%i$%i$%i' % (actor.type, actor.id, type)

    def list(self, actor, type):
        key=self.mergekey(actor, type)
        #print key
        return self.r.hvals(key)

    def rm(self, actor, type, id):
        return self.r.hdel(self.mergekey(actor, type), id)
    def length(self, actor, type):
        return self.r.hlen(self.mergekey(actor,type))

    def on_actor_msg_add():
        pass

    def register(self, peer):
        self.peers.append(peer)

class MsgMgr():
    def __init__(self, svr, selfinfo):
        self.msgsvr=MsgSvr()
    def init(self, options, processors):
        self.processors=[]
        self.processors.extend(processors)
        #send(register_msg)
        self.msgsvr.register(self)

    def update(self):
        #listall >
        pass

import datetime
import time
class MsgProcessor():
    def __init__(self, type, msgsvr):
        self.type=type
        self.msgsvr=msgsvr
        self.starttime=time.time()
        self.seq=0

    def msg(self, type, expire, data):
        return dict(type=type, id=self.nextid(), time=time.time(), expire=expire, data=data)

    def nextid(self):
        self.seq+=1
        return self.starttime*(1<<31)+self.seq

    def sendm(self, toers, type, data, expire=-1):
        [self.send(actor, type, data, expire) for actor in toers]

    def send(self, actor, type, data, expire=-1):
        msg=self.msg(type, expire, data)
        if self.msgsvr.set(actor, self.type, msg['id'], msg) is None:
            print "full"
        else:
            print "send msg success"

class MailProcessor(MsgProcessor):
    def update(self, actor, id, op):
        pass

    def list(self, actor):
        return self.msgsvr.list(actor, self.type)

    def remove(self, actor, id):
        pass
    @staticmethod
    def mail(sender, title, content, attachements=None):
        return json.dumps(dict(sender=sender.id, title=title, content=content, attachments=attachements))

    def send(self, toers, mail):
        if isinstance(toers, list):
            MsgProcessor.sendm(self, toers, 1 , mail)
        else:
            MsgProcessor.send(self, toers, 1 , mail)
        pass

    def onNotify(self, actor, mail):
        print 'actor receive mail'
        pass

class Actor():
    def __init__(self):
        self.id=0
        self.type=0
        self.

def main():
    msgsvr=MsgSvr()
    actor=Actor()
    actor.type=1
    actor.id=23556
    mail=MailProcessor(1, msgsvr)
    mail.send(actor, MailProcessor.mail(actor, 'echo', 'hello,world from myself'))
    print 'list:'+str(mail.list(actor))



if __name__ == '__main__':
    main()


