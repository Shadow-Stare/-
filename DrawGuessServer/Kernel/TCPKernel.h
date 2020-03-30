#ifndef  _TCPKERNEL_H
#define _TCPKERNEL_H

#include "IKernel.h"
#include "CMySql.h"
#include "MyThreadPool.h"
#include <iostream>
#include <time.h>
using namespace std;

class TCPKernel;
typedef void (TCPKernel::*PFUN)(SOCKET sock,char* szbuf);

struct ProtocolMap
{
	PackType m_nType;   //Э��
	PFUN     m_npfun;   //������ַ
};

//������Ϣ
struct Audience_Info
{
	long long m_uid;//����UID
	SOCKET m_sock;
	HANDLE m_hEvent;
};

struct Room_Info
{
	long long m_ownerid;//����UID
	long long m_roomid;
	goodinfo* m_good;//�²����Ʒ��Ϣ
	int m_passnum;//��Ե�����
	HANDLE m_hThreadGameTimer;
	char m_szbuffer[_DEF_STREAMPACKDEF];
	SOCKET m_sock;
	HANDLE   m_hSemaphore;
	bool isGameon;
};

struct Timer_Thread_Info{
	TCPKernel* m_pKernel;
	Room_Info* pRoom;
};

class TCPKernel :public IKernel
{
public:
	TCPKernel();

	virtual ~TCPKernel();
public:
	 bool Open();
	 void Close();
	 bool DealData(SOCKET sock,char* szbuf);

	 int GetRandomGoodsID();//ȡ�����ƷID���㷨���ظ���
public:
	void RegisterRq(SOCKET sock,char* szbuf);//�ͻ���ע������
	void LoginRq(SOCKET sock,char* szbuf);//�ͻ��˵�¼����
	void StreamInfoRq(SOCKET sock,char* szbuf);//ת������ͼƬ��ͷ�����������֣�
	void SelectRoomRq(SOCKET sock,char* szbuf);//��ҽ���

	void CreateRoomRq(SOCKET sock,char* szbuf);//����������������
	void GameStartRq(SOCKET sock,char* szbuf);//��Ϸ��ʼ����
	void PlayerMessageRq(SOCKET sock,char* szbuf);//�����Ϣ���󣨷�����ת��+�жϣ�

	void PlayerLeaveRoomRq(SOCKET sock,char* szbuf);//��������뿪����

	void LogoutRq(SOCKET sock,char* szbuf);//�����������
public:
	CMySql m_sql;
	CMyThreadPool m_threadpool;


	std::list<long long> m_OnlinePlayer;
	std::list<Room_Info*> m_lstRoomInfo;
	std::map<Room_Info*,std::list<Audience_Info*>> m_mapRoomToAudience;

	
	HANDLE m_hNullEvent;//���¼���ʹ��WaitForSingleObject�ӳ�ʱ�õ�
	
	static  unsigned _stdcall ThreadGameTimer( void * );

};



class StreamItask : public Itask
{
public:
	StreamItask(Room_Info* pRoom,Audience_Info* pAudience,TCPKernel* pKernel)
	{
		m_pRoom = pRoom;
		m_pAudience = pAudience;
		m_pKernel = pKernel;
	}
	~StreamItask()
	{}
public:
	void RunItask();
private:
	Room_Info* m_pRoom;  //��Ƶ��
	Audience_Info* m_pAudience; //����
	TCPKernel*   m_pKernel; //����

};

#endif
