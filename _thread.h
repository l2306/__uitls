//

#ifndef __THREAD__
#define __THREAD__

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <pthread.h>

class z_Thread
{
	public:
		z_Thread();					//	构造函数
		~z_Thread();				//	析构函数
		pthread_t getThreadId();	//	获得线程id
		bool start();				//	启动线程
		void suspend();				//	线程停止
		void continues();			//	线程继续运行

	private:
		// 屏蔽SIGUSR1信号 , POSIX标准建议在调用sigwait()等待信号以前，
		//		进程中所有线程都应屏蔽该信号，以保证仅有sigwait()的调用者获得该信号 ,
		//	 如果不屏蔽该信号，在sigwait()之前调用pthread_kill()就会出现User defined signal 1.
		void maskSIGUSR1();
		pthread_t _pid;						//	线程pid
		sigset_t _waitSig;					//	信号
		static void *threadFun(void *arg);	//	线程运行主函数

	protected:
		virtual void Run() = 0;				//功能接口

};

#endif
