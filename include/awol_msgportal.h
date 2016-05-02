#pragma once
#include <string>
#include "awol_msgbox.h"
namespace awolmsg {
struct MsgPortal {
	virtual const MsgOptions & options() const = 0;
public:
	virtual int sendto(const MsgActor & actor, const std::string & m, bool fromc = true);
	virtual int put(const std::string & m);//server
	virtual int update(uint64_t id, const std::string & m, int op = 0);
	virtual int list(bool fromc = true);
	virtual int remove(uint64_t id, bool fromc = true);//client or server
public:
	virtual void onremove(int ret, uint64_t id, bool fromc = true);
	virtual void onupdate(int ret, uint64_t id, int op);
	virtual void onlist(int ret, bool fromc, const MsgList & vms);
	virtual void onnotify(uint64_t id, const std::string & msg);
	virtual void onsend(int ret, bool fromc);
	virtual void onput(int ret);
public:
	MsgPortal(const MsgActor & actor, int type);
	int type() const { return msgbox_.type(); };
	const MsgActor & actor() const { return msgbox_.actor(); };
	virtual ~MsgPortal();
	MsgBox &  msgbox(){ return msgbox_; }
private:
	MsgBox    msgbox_;
};
}
