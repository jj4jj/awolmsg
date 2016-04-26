


class MsgSvr():
    def __init__(self):
        self.redis=None
        #connect to redis server

    def set(self, actor, qtype, qid, msg):
        #hset 
        hset='u1$msg$%d$%i$%i' % (actor.type, actor.id, qtype)
        pass

    def alive(actor):
        pass

    def list(actor, type):
        pass

    def rm(actor, type, id):
        pass

    def on_actor_msg_add():
        pass




class MsgMgr():
    pass

class MsgProcessor():
    pass


def main():
    msgsvr=MsgSvr()
    #prefix$msg$actor.type$actor.id$type
    actor=object()
    actor.type=1
    actor.id=23556
    msgsvr.set(actor, 1, 23563452356346, 'hello')


if __name__ == '__main__':
    main()


