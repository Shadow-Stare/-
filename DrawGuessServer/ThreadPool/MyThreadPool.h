#pragma once
#include <list>
#include <queue>
#include <Windows.h>
#include <process.h>

class Itask
{
public:
	Itask()
	{}

	virtual ~Itask()
	{}

public:
	virtual void RunItask() = 0;
};




class CMyThreadPool
{
public:
	CMyThreadPool(void);
	~CMyThreadPool(void);
public:
	//�����̳߳� (��С�߳�����,����߳�����)
	bool CreateThreadPool(long lMinThreadNum,long lMaxThreadNum);

	//�����̳߳�
	void DestroyThreadPool();
	static  unsigned  __stdcall ThreadProc( void * );

	//����������ύ����
	bool Push(Itask* );
private:
	std::list<HANDLE> m_lstThread;
	std::queue<Itask*> m_qItask;
	bool  m_bFlagQuit;
	HANDLE m_hSemaphore;
	HANDLE m_hMutex;
	long   m_lMaxThreadNum;
	long   m_lCreateThreadNum;
	long   m_lRunThreadNum;
};

