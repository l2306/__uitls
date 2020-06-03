#ifndef UTILS_TIMEREVENT_H
#define UTILS_TIMEREVENT_H

#include <sys/time.h>
#include "_HeapT.h"
#include "_thread.h"
// windows下 因该自己定义线程替换 （暂时没有支持windows）

typedef  void (*funTmEv_t)(void*);   // 

class TimerEvent{
public:
    typedef enum enumTmEv{
        eTmEv_ONCE=1,           //单次 
        eTmEv_LOOP=2            //周期 
    }enumTmEv;
	TimerEvent()		{}
	~TimerEvent()		{}
	enumTmEv		eTmEv;
	int				evid;
	int				timeout;             //时间        ms 
	int				interval;            //间隔时间 
	funTmEv_t		funTmEv;             // 
	void*			argTmEv;             //超时回调 
    
	friend int getKey(TimerEvent* const& data);
	friend bool cmpVal(TimerEvent* const& data_P, TimerEvent* const& data_S);
};
//int getKey(TimerEvent* const& data);
//bool cmpVal(TimerEvent* const& data_P, TimerEvent* const& data_S);


class TimeLoop :public z_Thread
{
public:
	TimeLoop()		{}
	~TimeLoop()	{}
   	void Run();

	void clear();
	void reset();
	void insert(TimerEvent* e);     //插入 
	void update(TimerEvent* e);     //更新    //从此刻开始 
   	void loopon(TimerEvent* e);     //继续    //timeout精确 
	void remove(TimerEvent* e);     //删除 
	void process_timeout_events();

	TimerEvent* top(bool* pbOK = NULL) const;
private:
    //HeapT <TimerEvent*, int>  heapTmEv;
	HeapT2<TimerEvent*, int, HeapTrait<TimerEvent*, int> > heapTmEv;
};

//#define DBG_timerevent
#ifdef  DBG_timerevent

int test_timerevent();

#endif

#endif
