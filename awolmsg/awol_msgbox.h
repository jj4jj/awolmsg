#pragma  once
#include <vector>
#include "awol_msg.h"
#include "awol_error.h"

namespace awolmsg {
    template<class ValueT>	
	struct MsgFuture  {
        typedef std::function<void(int ret, ValueT &)> ResultNotify;
		MsgFuture & result(ResultNotify notify){
			this->notify = notify;
			s_pendding_result[this->transaction] = notify;
			return *this;
		}
		int ret{ 0 };
		uint64_t transaction{ 0 };
		MsgFuture(int ret, uint64_t t_) :ret(ret), transaction(t_){}
	private:
		ResultNotify notify{ nullptr };
		static std::unordered_map<uint64_t, ResultNotify>	s_pendding_result;
	public:
		static void invoke(uint64_t transid, int ret, ValueT & v){
			auto it = s_pendding_result.find(transid);
			if (it != s_pendding_result.end()){
				s_pendding_result[transid](ret, v);
			}
		}
    };

    struct MsgBox { //an actor msgbox
		MsgBox(const MsgActor & act, int t_) :actor_(act), type_(t_){}
		int type()const { return type_; }
    public:
		MsgFuture<std::vector<MsgT*> > list(){
			int ret = 0;
			//send list , option.from = self, to->
			uint64_t tranaction = 0;
			return MsgFuture<std::vector<MsgT*> >(ret, tranaction);
		}
		MsgFuture<int>	send(MsgBox & mbto, const MsgT & m){ //send msg to
			int ret = 0;
			//send add , option.from = self, to->
			uint64_t tranaction = 0;
			return MsgFuture<int>(ret, tranaction);
		}
		MsgFuture<int>	put(const MsgT & m){ //send msg to
			int ret = 0;
			//send add , cb = , to = self, from = none
			uint64_t tranaction = 0;
			return MsgFuture<int>(ret, tranaction);
		}
		MsgFuture<int>	remove(const MsgT & m){ //remove one msg
			int ret = 0;
			//send remove , cb = 
			uint64_t tranaction = 0;
			return MsgFuture<int>(ret, tranaction);
		}
		MsgFuture<int>	update(uint64_t id, const MsgT & m){
			int ret = 0;
			//send update , cb = 
			uint64_t tranaction = 0;
			return MsgFuture<int>(ret, tranaction);
		}
		MsgFuture<int>	clean(){
			int ret = 0;
			//send clean , cb = 
			uint64_t tranaction = 0;
			return MsgFuture<int>(ret, tranaction);
		}
		MsgFuture<MsgT> recv(){ //notify a new message
			int ret = 0;
			//subscribe
			uint64_t tranaction = 0;
			return MsgFuture<MsgT>(ret, tranaction);
		}
	private:
		MsgActor actor_;
		int		 type_;
    };






}

