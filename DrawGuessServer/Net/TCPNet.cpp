
#include "TCPNet.h"

bool  TCPNet::m_bFlagQuit = true;
IKernel *TCPNet::m_pKernel = NULL;

//���캯��
TCPNet::TCPNet(IKernel *pKernel)
{
	m_sockListen = NULL;
	m_pKernel = pKernel;
}

//������
TCPNet::~TCPNet(void)
{
}

//�����ʼ��
bool TCPNet::InitNetWork()
{
	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        UnInitNetWork();
        return false;
    }

	m_sockListen = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP );
	if(m_sockListen == INVALID_SOCKET )
	{
		UnInitNetWork();
		 return false;
	}

	sockaddr_in  addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
	addrServer.sin_port = htons(_DEFPORT);
	if(SOCKET_ERROR ==bind(m_sockListen,(const sockaddr*)&addrServer,sizeof(addrServer)))
	{
		 UnInitNetWork();
		 return false;
	}

	if(SOCKET_ERROR  == listen(m_sockListen,20))
	{
		 UnInitNetWork();
		 return false;
	}

	m_hThreadAccept = (HANDLE)_beginthreadex(NULL,0,&ThreadAccept,this,0,0);
	
	return true;
}

//����������̣߳�ÿһ��sock����accept�����list
unsigned _stdcall TCPNet::ThreadAccept( void * lpvoid)
{
	TCPNet *pthis = (TCPNet*)lpvoid;
	
	while(pthis->m_bFlagQuit)
	{
		  SOCKET sockWaiter =  accept(pthis->m_sockListen,NULL,NULL);
		  if(sockWaiter == INVALID_SOCKET)
			  continue;
		  HANDLE m_hThreadRecv = (HANDLE)_beginthreadex(NULL,0,&ThreadRecv,(void * )sockWaiter,0,0);
		  if(m_hThreadRecv)
		  {
			   pthis->m_lsthThreadRecv.push_back(m_hThreadRecv);
		  }
		 
		
		
		//�������� 
	}
	return 0;
}

//�������ݵ��̣߳��Ƚ��ܰ����ȣ��ٽ������ݣ�֮�󽻸�kernel����
unsigned _stdcall TCPNet::ThreadRecv( void * lpvoid)
{
	SOCKET sockWaiter = (SOCKET)lpvoid;
	char* pszbuf = NULL;
	int nPackSize;
	int nRelReadNum;
	while(TCPNet::m_bFlagQuit)
	{
		
		//���հ�ͷ
	 nRelReadNum =  recv(sockWaiter,(char*)&nPackSize,sizeof(int),0);
	  if(nRelReadNum <=0)
	  {
		  //�ǲ��ǿͻ�������
		  if(WSAECONNRESET  == GetLastError())
		  {
			  closesocket(sockWaiter);
			  sockWaiter  =NULL;
			  break;
		  }
		  continue;
	  }	
	 //������
	pszbuf = new char[nPackSize];
	int noffset = 0;
	while(nPackSize)
	{
		 nRelReadNum =  recv(sockWaiter,pszbuf+noffset,nPackSize,0);
		 noffset += nRelReadNum;
		 nPackSize -= nRelReadNum;
	}
	
	 
	 //����
	  m_pKernel->DealData(sockWaiter,pszbuf);
		
	//
	delete []pszbuf;
	pszbuf = NULL;

	}
	return 0;
}

//ж������
void TCPNet::UnInitNetWork()
{
	m_bFlagQuit = false;
	if(m_hThreadAccept )
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(m_hThreadAccept,100))
			TerminateThread(m_hThreadAccept,-1);
		
		CloseHandle(m_hThreadAccept);
		m_hThreadAccept = NULL;

	
	}
	auto ite = m_lsthThreadRecv.begin();
	while(ite != m_lsthThreadRecv.end())
	{
		if(WAIT_TIMEOUT == WaitForSingleObject(*ite,100))
		TerminateThread(*ite,-1);
		
		CloseHandle(*ite);
		*ite = NULL;
		ite++;
	}
	m_lsthThreadRecv.clear();
	WSACleanup();
	if(m_sockListen)
	{
		closesocket(m_sockListen);
		m_sockListen = NULL;
	}
}

//�������ݣ��ȷ��Ͱ����ȣ��ٷ��Ͱ����ݣ���ͬ��ͼƬ������㷨��
bool TCPNet::SendData(SOCKET sock,char* szbuf,int nlen)
{
	if(sock == INVALID_SOCKET || !szbuf || nlen <=0)
		return false;
	//������
	if(send(sock,(const char*)&nlen,sizeof(int),0)<0)
		return false;
	//������
	if(send(sock,szbuf,nlen,0)<0)
		return false;
	
	return true;
}

//����ͼƬ���ݣ��ȷ��Ͱ����ȣ��ٷ��Ͱ����ݣ�ͼƬ������㷨��
bool TCPNet::SendPicData(SOCKET sock,char* szbuf,int nlen){
	if(sock == INVALID_SOCKET || !szbuf || nlen <=0)
		return false;
	//������
	if(send(sock,(const char*)&nlen,sizeof(int),0)<0)
		return false;
	//������,�ֿ鴫��
	char a[4096] = {0};
	char *pto = szbuf;
	while(1){
		if(nlen > 4096){
			send(sock,pto,4096,0);
		}else{
			send(sock,pto,nlen,0);
			break;
		}
		pto = pto + 4096;
		nlen = nlen - 4096;
	}
	return true;
}