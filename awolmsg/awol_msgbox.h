#pragma  once
#include <functional>
#include "awol_msg.h"
namespace awolmsg {
struct MsgBox { //an actor msgbox
	MsgBox(const MsgActor & act, int t_);
	int type()const { return type_; }
	const MsgActor & actor() const { return actor_;}
public:
	typedef std::function<void(int ret, const MsgList &)> MsgListCallBack;
	typedef std::function<void(int ret, uint64_t id, const string & msg)> MsgOPCallBack;
	int list(MsgListCallBack lcb);
	int	send(const MsgActor & sendto, const std::string & m_, MsgOPCallBack cb=nullptr);
	int	put(const std::string & m, MsgOPCallBack cb = nullptr);
	int	remove(uint64_t uid, MsgOPCallBack cb = nullptr);//remove one msg
	int	sync(uint64_t uid, const std::string & m_, MsgOPCallBack cb = nullptr);
	int	clean(MsgOPCallBack cb = nullptr);
    int	get(uint64_t uid, MsgOPCallBack cb = nullptr);

private:
	MsgActor		actor_;
	int				type_{ 0 };
};

}

