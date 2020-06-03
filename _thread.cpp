#include "_thread.h"

z_Thread::z_Thread()
{
	maskSIGUSR1();
	sigemptyset(&_waitSig);				//	定义SIGUSR1信号阻塞
	sigaddset(&_waitSig, SIGUSR1);
}

z_Thread::~z_Thread()
{
}

void z_Thread::maskSIGUSR1()
{
	sigset_t sig;
	sigemptyset(&sig);
	sigaddset(&sig , SIGUSR1);
	pthread_sigmask(SIG_BLOCK , &sig , NULL);		//结果集是当前集合参数集的并集
}


pthread_t z_Thread::getThreadId()
{
	return _pid;
}


void *z_Thread::threadFun(void *arg)
{
	z_Thread *pThread = (z_Thread*)arg;
	pThread->Run();			//重写的方法
}

bool z_Thread::start()
{
	int nRet = pthread_create(&_pid , NULL , threadFun , this);
	if(0 != nRet)
		return false;
	else{
		nRet = pthread_detach(_pid);
		if(nRet == 0)
			return true;
	}

	return true;
}

void z_Thread::suspend()
{
	int sig;
	sigwait(&_waitSig , &sig);		//阻塞等待
}

void z_Thread::continues()
{
	pthread_kill(_pid , SIGUSR1);	//发信号，结束阻塞
}
