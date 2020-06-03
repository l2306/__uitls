
#include "_TimerEvent.h"
#include <unistd.h>


//﻿https://blog.csdn.net/lu_embedded/article/details/53080496 
/*
    int  timeout;   //到期时间        ms
    int  interval;  //间隔时间
    堆顶按照timeout排序，小顶堆
    1，插入时，timeout = getnow + interval;
    2，更新时，timeout += interval;  timeout = timeout<每天上限? timeout:timeout-每天上限;
    =======================================================================
    start@  sleep(堆顶时间-now)          ... @     sleep(堆顶时间-now)                                    
         @  处理事件(timout<now)         ... @
         @  修改timeout，保持堆属性      ... @
*/

//通过开启线程 
//  1，根据堆顶时间设置睡眠,下次唤醒后处理超时事件 
//      优劣：定时资源消耗与事件数量相关 
//  2，设置固定时间睡眠，定时到达时，进行事件处理 
//      优劣：消耗固定的事件资源 

//alarm sleep 只能精确到秒 

#define	__DEBUG__  
#ifdef	__DEBUG__  
	#define DEBUG(format,...) \
	printf("File: "__FILE__", Line: %05d: %s "format, __LINE__, __FUNCTION__, ##__VA_ARGS__)  
#else  
	#define DEBUG(format,...)  
#endif 

int getKey(TimerEvent* const& data){
	return data->evid;
}
bool cmpVal(TimerEvent* const& data_P, TimerEvent* const& data_S){
	return data_P->timeout < data_S->timeout;
}	

int ms_sleep(int s, long ms) {
    struct timeval  tv;
    tv.tv_sec       = s;         //秒 
    tv.tv_usec      = ms*1000;   //微妙 
    return select(0, NULL, NULL, NULL, &tv);
}

//  获得今天的第多少毫秒 
//  注：int_max 2*2147483647      1天(d)=86400秒(s)  
int get_cur_ms(){
    struct timeval      tv;  
    struct timezone     tz;   
    gettimeofday(&tv, &tz);  
	//DEBUG(">>> >>>tv_sec %d tv_usec %d \n", tv.tv_sec, tv.tv_usec);
    return tv.tv_sec*1000+ tv.tv_usec/1000;
}

void TimeLoop::insert(TimerEvent* e) 
{                   
    e->timeout    = get_cur_ms()+ e->interval;
    heapTmEv.insert(e);
	//continues();
	DEBUG("%s >>>> timeout %d interval %d \n",__FUNCTION__, e->timeout, e->interval);
}
void TimeLoop::update(TimerEvent* e)
{                
    e->timeout    = get_cur_ms()+ e->interval;
    heapTmEv.update(e); 
}   
void TimeLoop::loopon(TimerEvent* e) 
{                   
    e->timeout    += e->interval;
    //e->timeout = e->timeout<86400000? e->timeout:e->timeout-86400000;
    heapTmEv.update(e);
}                       
void TimeLoop::remove(TimerEvent* e)
{
    heapTmEv.remove(e);
}

void TimeLoop::Run()
{
	process_timeout_events();
}

void TimeLoop::process_timeout_events()
{
    while(1){
		const int     now = get_cur_ms();
		bool          hasTop=false;
		TimerEvent    *tmev = heapTmEv.top(&hasTop);
        while ( hasTop && tmev->timeout <= now) 
		{
			printf("--,%d, \n",heapTmEv.size());
			switch (tmev->eTmEv){
			case TimerEvent::eTmEv_ONCE :			//单次 
				tmev->funTmEv( tmev->argTmEv);
				remove(tmev); 
				break;
			case TimerEvent::eTmEv_LOOP :		 	//周期
				tmev->funTmEv( tmev->argTmEv);
				loopon(tmev); 
				break;
			default:
				break;
			}
			//DEBUG("%s >>>> timeout %d interval %d \n",__FUNCTION__, tmev->timeout, tmev->interval);
           tmev = heapTmEv.top(&hasTop);
		}
		if(hasTop){
			DEBUG("sleeping..%d.. out %d  new %d \n",tmev->timeout-get_cur_ms(), tmev->timeout, get_cur_ms());
			ms_sleep(0, tmev->timeout - get_cur_ms());      //判断隔天时候的，是否合理 
		}else{
			suspend();
			DEBUG("suspend.......  %d \n", get_cur_ms());		
		}
	}
}


//#ifdef  DBG_timerevent

#include <stdio.h>

void print1(void*){
	DEBUG("xx1111 %d\n", get_cur_ms());
}
void print2(void*){
	DEBUG("xx2222 %d\n", get_cur_ms());
}

//g++ *.cpp -O2 -lpthread -g  -o  t

int test_timerevent();

int main()
{	
	test_timerevent();
	return 0;
}

int test_timerevent(){

	TimeLoop tt;
	tt.start();
	TimerEvent* 	t= new TimerEvent();
	t->evid		= 1;
	t->eTmEv 		= TimerEvent::eTmEv_LOOP;
	t->funTmEv		= print1;
	t->argTmEv		= NULL;
	t->interval	= 10;
	DEBUG("xx11   %d %d \n",t->timeout, get_cur_ms());
	tt.insert(t);
	DEBUG("xx11   %d %d \n",t->timeout, get_cur_ms());
	if (0){
		TimerEvent* 	t= new TimerEvent();
		t->evid		    = 2;
		t->eTmEv 		= TimerEvent::eTmEv_LOOP;
		t->interval	    = 1000;
		t->funTmEv		= print2;
		t->argTmEv		= NULL;
		tt.insert(t);
		DEBUG("xx22   %d %d \n",t->timeout, get_cur_ms());
	}


	while(1){
		sleep(1);
	}

    return 0;
}

//#endif

//--------------------------------------
//void TimeLoop::push_timer_heap(Event *e) 
//{                   
//  e->index      = timer_heap_.size();
//  e->timeout    = get_milliseconds()+ e->interval;
//  timer_heap_.push_back(e);
//  adjust_timer_heap(e->index, timer_heap_.size(), timer_heap_);
//}
//
//void TimeLoop::popbak_timer_heap(void)
//{                
//  swap_timer_event(0, timer_heap_.size() - 1);
//  adjust_timer_heap(0, timer_heap_.size() - 1, timer_heap_);
//  timer_heap_.pop_back();
//}                          
//void TimeLoop::remove_timer_heap(const Event *e)
//{                
//  timer_heap_[e->index]->timeout = -1;
//  adjust_timer_heap(e->index, timer_heap_.size(), timer_heap_);
//  popbak_timer_heap();
//}
//
//void TimeLoop::process_timeout_events(void)
//{ 
//  const int64_t now = get_cur_ms();
//  Event *e = top_timer_heap();
//  while (e != nullptr && e->timeout <= now) {
//      e->timer_cb(e->fd, Event::kEventTimer, e->args);
//      if(1)      //单次
//          pop_timer_heap();
//      if(0)      //周期
//          pop_timer_heap();
//      e = top_timer_heap();
//  }
//  //根据堆顶时间，设置睡眠
//  //也可  固定时间睡眠
//}


