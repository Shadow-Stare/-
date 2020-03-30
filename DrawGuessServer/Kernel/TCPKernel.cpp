#include "TCPKernel.h"

#include "TCPNet.h"

static long long roomid_auto = 0;

TCPKernel::TCPKernel()
{
	m_pNet = new TCPNet(this);
	m_hNullEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

TCPKernel::~TCPKernel()
{
	delete m_pNet;
	m_pNet = NULL;
}

_BEGIN_PROTOCOL_MAP
	PM(_DEF_PROTOCOL_REGISTER_RQ,&TCPKernel::RegisterRq)
	PM(_DEF_PROTOCOL_LOGIN_RQ,&TCPKernel::LoginRq)
	PM(_DEF_PROTOCOL_LOGOUT_RQ,&TCPKernel::LogoutRq)

	//���ڷ��������ԣ�����Ϣͷ������һ��ת�����ʰ�ͬһ������
	PM(_DEF_PROTOCOL_STREAMINFO_RQ,&TCPKernel::StreamInfoRq)
	PM(_DEF_PROTOCOL_STREAMCONTENT_RQ,&TCPKernel::StreamInfoRq)

	PM(_DEF_PROTOCOL_ENTERROOM_RQ,&TCPKernel::SelectRoomRq)
	PM(_DEF_PROTOCOL_CREATEROOM_RQ,&TCPKernel::CreateRoomRq)
	PM(_DEF_PROTOCOL_GAMESTART_RQ,&TCPKernel::GameStartRq)
	PM(_DEF_PROTOCOL_PLAYERMESSAG_RQ,&TCPKernel::PlayerMessageRq)
	PM(_DEF_PROTOCOL_LEAVEROOM_RQ,&TCPKernel::PlayerLeaveRoomRq)

_END_PROTOCOL_MAP

void StreamItask::RunItask(){
	//�ȴ��ź�\�¼�
	HANDLE hary[] = {m_pRoom->m_hSemaphore,m_pAudience->m_hEvent};
	DWORD  dwIndex;
	while(1)
	{
		dwIndex = WaitForMultipleObjects(2,hary,FALSE,INFINITE);
		if(dwIndex ==0)
		{
			//����
			m_pKernel->m_pNet->SendData(m_pAudience->m_sock,m_pRoom->m_szbuffer,_DEF_STREAMPACKDEF);
		}
		else if(dwIndex == 1)
			//�˳�����
			break;
	}
}

void TCPKernel::StreamInfoRq(SOCKET sock,char* szbuf){
	StreamInfo *psi = (StreamInfo*)szbuf;
	Room_Info *pRoom = NULL;
	
	cout << "���յ�ͼƬת�����󣬴�С" << psi->m_nLen << endl;

	//���ݷ��������ݵ�socket�����ж����ĸ����䲢��¼
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(sock== (*iteRoom)->m_sock)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	//�����Ƹ�����map<������list<����>>
	int nSize = m_mapRoomToAudience[pRoom].size();
	//�������û�й��ڣ�������
	//����й��ڣ����ͷ���Ӧ�������ź���
	if(nSize >0)
	{
		memcpy(pRoom->m_szbuffer,szbuf,_DEF_STREAMPACKDEF);
		ReleaseSemaphore(pRoom->m_hSemaphore,nSize,NULL);
	}
}

//ѡ�񷿼�
void TCPKernel::SelectRoomRq(SOCKET sock,char* szbuf)
{
	STRU_ENTERROOM_RQ *pssr = (STRU_ENTERROOM_RQ*)szbuf;
	Room_Info *pRoom = NULL;
	Audience_Info *pAudienceif = new Audience_Info;
	//ѡ���Ӧ����ŵ���������������Ϣ
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(pssr->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	if(pRoom == NULL){
		//û�ҵ�����
		STRU_ENTERROOMFAIL_RS erfrs;
		erfrs.m_nType = _DEF_PROTOCOL_ENTEREOOMRES_RS;
		erfrs.result = 0;
		m_pNet->SendData(sock,(char*)&erfrs,sizeof(erfrs));
		return;
	}

	pAudienceif->m_uid = pssr->m_uid;
	pAudienceif->m_sock = sock;
	pAudienceif->m_hEvent = WSACreateEvent();
	//map<������list<����>>
	m_mapRoomToAudience[pRoom].push_back(pAudienceif);

	STRU_PLAYERNUMRENEW_RS pnnrs;
	pnnrs.m_nType = _DEF_PROTOCOL_PLAYERNUMRENEW_RS;
	pnnrs.m_roomid = pRoom->m_roomid;
	pnnrs.m_playernum = m_mapRoomToAudience[pRoom].size() + 1;
	pnnrs.m_uid = pRoom->m_ownerid;

	//����������Ϣ
	m_pNet->SendData(pRoom->m_sock,(char*)&pnnrs,sizeof(pnnrs));

	//���������ÿ�����ڷ���Ϣ
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&pnnrs,sizeof(pnnrs));
		pAudience++;
	}

	//���̳߳�Ͷ������--����һ���߳�
	Itask *pItask = new StreamItask(pRoom,pAudienceif,this);
	m_threadpool.Push(pItask);
}

//����������������
void TCPKernel::CreateRoomRq(SOCKET sock,char* szbuf){
	STRU_CREATEROOM_RQ *crrq = (STRU_CREATEROOM_RQ *)szbuf;
	Room_Info* rif = new Room_Info;
	rif->m_good = NULL;
	rif->m_hThreadGameTimer = NULL;
	rif->m_ownerid = crrq->m_uid;
	ZeroMemory(rif->m_szbuffer,sizeof(rif->m_szbuffer));
	rif->m_sock = sock;
	rif->m_roomid = roomid_auto;
	rif->m_hSemaphore = CreateSemaphore(NULL,0,_DEF_SEMAPHORENUM,NULL);
	m_lstRoomInfo.push_back(rif);

	STRU_CREATEROOM_RS crrs;
	crrs.m_nType = _DEF_PROTOCOL_CREATEROOM_RS;
	strcpy_s(crrs.m_PlayerName,_DEF_SIZE,crrq->m_PlayerName);
	crrs.m_roomid = rif->m_roomid;
	crrs.m_uid = crrq->m_uid;

	m_pNet->SendData(sock,(char*)&crrs,sizeof(crrs));

	roomid_auto++;
}

//��Ϸ��ʼ����
void TCPKernel::GameStartRq(SOCKET sock,char* szbuf){
	STRU_GAMESTART_RQ* gsrq = (STRU_GAMESTART_RQ *)szbuf;
	STRU_GOODSINFO_RS gsrs;
	Room_Info *pRoom = NULL;
	goodinfo* gifo = new goodinfo;

	//�����ݿ���ȡ����Ʒ��Ϣ
	char szsql[_DEF_SQLLEN] = {0};
	list<string> lststr;
	
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	int gid = GetRandomGoodsID();
	sprintf_s(szsql,"SELECT * FROM goods_name_list WHERE id = %d;",gid);
	m_sql.SelectMySql(szsql,7,lststr);
	m_sql.DisConnect();
	
	if(lststr.size() >0){
		gifo->m_gid = atoi(lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_ThingName,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_FirstHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_SecondHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_ThirdHint,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
		gifo->m_GuessTime = atoi(lststr.front().c_str());
		lststr.pop_front();
		strcpy_s(gifo->m_AuthorName,_DEF_SIZE,lststr.front().c_str());
		lststr.pop_front();
	}


	//ѡ���Ӧ���䲢д�뷿����Ϣ�ṹ���goods��Ա����
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(gsrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	//����Ѿ�������ɾ����Ӧ�ڴ�
	if(pRoom->m_good != NULL){
		delete pRoom->m_good;
	}

	pRoom->m_good = gifo;
	pRoom->m_passnum = 0;

	//׼�����Ͱ�
	gsrs.m_nType = _DEF_PROTOCOL_GOODSINFO_RS;
	gsrs.m_roomid = gsrq->m_roomid;
	gsrs.m_uid = gsrq->m_uid;
	gsrs.m_GuessTime = gifo->m_GuessTime;
	strcpy_s(gsrs.m_ThingName,_DEF_SIZE,gifo->m_ThingName);
	strcpy_s(gsrs.m_FirstHint,_DEF_SIZE,gifo->m_FirstHint);
	strcpy_s(gsrs.m_SecondHint,_DEF_SIZE,gifo->m_SecondHint);
	strcpy_s(gsrs.m_ThirdHint,_DEF_SIZE,gifo->m_ThirdHint);
	strcpy_s(gsrs.m_AuthorName,_DEF_SIZE,gifo->m_AuthorName);

	//����������Ϣ
	m_pNet->SendData(pRoom->m_sock,(char*)&gsrs,sizeof(gsrs));

	//���������ÿ�����ڷ���Ϣ
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&gsrs,sizeof(gsrs));
		pAudience++;
	}

	pRoom->isGameon = true;
	//������ʱ�߳�
	Timer_Thread_Info *tti = new Timer_Thread_Info;
	tti->m_pKernel = this;
	tti->pRoom = pRoom;
	pRoom->m_hThreadGameTimer = (HANDLE)_beginthreadex(NULL,0,&ThreadGameTimer,tti,0,0);
}

//��������뿪����
void TCPKernel::PlayerLeaveRoomRq(SOCKET sock,char* szbuf){
	STRU_LEAVEROOM_RQ *lrrq = (STRU_LEAVEROOM_RQ *)szbuf;
	STRU_CLOSEROOM_RS lrrs;
	Room_Info *pRoom = NULL;
	lrrs.m_nType = _DEF_PROTOCOL_CLOSEROOM_RS;
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(lrrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}
	lrrs.m_roomid = lrrq->m_roomid;
	lrrs.m_playeruid = lrrq->m_playeruid;
	lrrs.m_isRoomOwner = lrrq->m_isRoomOwner;

	if(lrrq->m_isRoomOwner){
		//����Ƿ����˷���֪ͨ�÷�����������˷�
		
		pRoom->isGameon = false;
		//���������ÿ�����ڷ���Ϣ
		list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			m_pNet->SendData((*pAudience)->m_sock,(char*)&lrrs,sizeof(lrrs));
			delete *pAudience;
			pAudience++;
		}

		m_mapRoomToAudience.erase(pRoom);
		if(pRoom->m_good != NULL){
			delete pRoom->m_good;
		}
		delete pRoom;

	}else{


		list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			if((*pAudience)->m_uid == lrrq->m_playeruid){
				break;
			}
			pAudience++;
		}

		m_mapRoomToAudience[pRoom].erase(pAudience);


		STRU_PLAYERNUMRENEW_RS pnnrs;
		pnnrs.m_nType = _DEF_PROTOCOL_PLAYERNUMRENEW_RS;
		pnnrs.m_roomid = pRoom->m_roomid;
		pnnrs.m_playernum = m_mapRoomToAudience[pRoom].size() + 1;
		pnnrs.m_uid = pRoom->m_ownerid;

		//����������Ϣ
		m_pNet->SendData(pRoom->m_sock,(char*)&pnnrs,sizeof(pnnrs));

		//���������ÿ�����ڷ���Ϣ
		pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			m_pNet->SendData((*pAudience)->m_sock,(char*)&pnnrs,sizeof(pnnrs));
			pAudience++;
		}

	}
}

//�����Ϣ���󣨷�����ת��+�жϣ�
void TCPKernel::PlayerMessageRq(SOCKET sock,char* szbuf){
	STRU_PLAYERMESSAG_RQ* pmrq = (STRU_PLAYERMESSAG_RQ *)szbuf;
	STRU_PLAYERMESSAG_RS pmrs;
	Room_Info *pRoom = NULL;
	pmrs.m_nType = _DEF_PROTOCOL_PLAYERMESSAG_RS;
	strcpy_s(pmrs.m_MessageContent,_DEF_PLAYERMESSAG_SIZE,pmrq->m_MessageContent);
	pmrs.m_playeruid = pmrq->m_playeruid;
	strcpy_s(pmrs.m_playername,_DEF_SIZE,pmrq->m_playername);
	pmrs.m_roomid = pmrq->m_roomid;

	//ѡ���Ӧ����
	auto iteRoom = m_lstRoomInfo.begin();
	while(iteRoom != m_lstRoomInfo.end())
	{
		if(pmrq->m_roomid == (*iteRoom)->m_roomid)
		{
			pRoom = *iteRoom;
			break;
		}
		iteRoom++;
	}

	if(pRoom->isGameon){
		if(strcmp(pmrq->m_MessageContent,pRoom->m_good->m_ThingName) == 0){
			pmrs.m_GuessResult = 1;
			pRoom->m_passnum++;
		}else{
			pmrs.m_GuessResult = 0;
		}
	}else{
		pmrs.m_GuessResult = 2;
	}

	//����������Ϣ
	m_pNet->SendData(pRoom->m_sock,(char*)&pmrs,sizeof(pmrs));

	//���������ÿ�����ڷ���Ϣ
	list<Audience_Info*> lstAudience = m_mapRoomToAudience[pRoom];
	auto pAudience = lstAudience.begin();
	while(pAudience != lstAudience.end())
	{
		m_pNet->SendData((*pAudience)->m_sock,(char*)&pmrs,sizeof(pmrs));
		pAudience++;
	}

}

//�յ��ͻ���ע������Ĵ���
void TCPKernel::RegisterRq(SOCKET sock,char* szbuf)
{
	STRU_REGISTER_RQ *psrr = (STRU_REGISTER_RQ*)szbuf;
	char szsql[_DEF_SQLLEN] = {0};
	STRU_REGISTER_RS srr;
	list<string> lststr;
	srr.m_nType = _DEF_PROTOCOL_REGISTER_RS;
	srr.m_szResult = _register_fail;
	srr.m_userid = -1;
	strcpy_s(srr.m_szUserName,"");
	//���ж��Ƿ��Ѿ�ע���
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	char szcmpsql[_DEF_SQLLEN] = {0};
	sprintf_s(szcmpsql,"SELECT * FROM user_personal_info WHERE username = '%s' OR email = '%s';",psrr->m_szUserName,psrr->m_szEmail);
	m_sql.SelectMySql(szcmpsql,1,lststr);
	if(lststr.size() >0)
	{
		m_pNet->SendData(sock,(char*)&srr,sizeof(srr));
		return ;
	}

	//���û���Ϣд�����ݿ�
	sprintf_s(szsql,"INSERT INTO user_personal_info VALUES(NULL,'%s','%s',%d,'%s');",
		psrr->m_szUserName,psrr->m_szPassWord,psrr->m_szGender,psrr->m_szEmail);
    
	if(m_sql.UpdateMySql(szsql))
	{
		srr.m_szResult = _register_success;
	}
	//�ظ�
	m_pNet->SendData(sock,(char*)&srr,sizeof(srr));
	m_sql.DisConnect();
}

//�յ��ͻ��˵�¼����Ĵ���
void TCPKernel::LoginRq(SOCKET sock,char* szbuf)
{
	STRU_LOGIN_RQ *pslr = (STRU_LOGIN_RQ*)szbuf;
	//ͨ���û����������� У�������Ƿ���ȷ
	char szsql[_DEF_SQLLEN] = {0};
	list<string> lststr;
	STRU_LOGIN_RS slr;
	slr.m_nType = _DEF_PROTOCOL_LOGIN_RS;
	slr.m_szResult = _login_fail;
	sprintf_s(szsql,"SELECT uid,password FROM user_personal_info WHERE username = '%s' OR email = '%s';",pslr->m_szUserName,pslr->m_szEmail);
	m_sql.ConnectMySql(MYSQL_HOST,MYSQL_USER,MYSQL_PWD,MYSQL_DB);
	m_sql.SelectMySql(szsql,2,lststr);
	m_sql.DisConnect();
	if(lststr.size() >0)
	{
		string userid = lststr.front();
		lststr.pop_front();

		string strPassword = lststr.front();
		lststr.pop_front();


		if(!strcmp(pslr->m_szPassWord,strPassword.c_str()))
		{
			slr.m_userid = _atoi64(userid.c_str());
			strcpy_s(slr.m_szUserName,pslr->m_szUserName);
			slr.m_szResult = _login_success;
		}


		//�������Ƿ��Ѿ����ߣ�����Ѿ��������¼ʧ�ܣ������¼��ǰ���
		auto pPlayer = m_OnlinePlayer.begin();
		while(pPlayer != m_OnlinePlayer.end())
		{
			if(_atoi64(userid.c_str()) == (*pPlayer)){
				slr.m_szResult = _login_fail;
				break;
			}
			pPlayer++;
		}
		m_OnlinePlayer.push_back(_atoi64(userid.c_str()));
	}

	//�ظ�
	m_pNet->SendData(sock,(char*)&slr,sizeof(slr));
}

//�����������
void TCPKernel::LogoutRq(SOCKET sock,char* szbuf){
	STRU_LOGOUT_RQ * lorq = (STRU_LOGOUT_RQ *)szbuf;
	auto pPlayer = m_OnlinePlayer.begin();
	while(pPlayer != m_OnlinePlayer.end())
	{
		if(lorq->m_userid == (*pPlayer)){
			break;
		}
		pPlayer++;
	}
	m_OnlinePlayer.erase(pPlayer);
}

//�����̳߳أ���ʼ��TCP����
bool TCPKernel::Open()
{
	if(!m_threadpool.CreateThreadPool(1,_DEF_THREADPOOLNUM))
		return false;

	if(!m_pNet->InitNetWork())
		return false;

	return true;
}

//�����̳߳أ��ر�TCP����
void TCPKernel::Close()
{
	m_threadpool.DestroyThreadPool();

	m_sql.DisConnect();

	m_pNet->UnInitNetWork();
}

//����������Ϣ��������Ϣ���Ͳ�ӳ���ȥʵ�ֶ�Ӧ����
bool TCPKernel::DealData(SOCKET sock,char* szbuf)
{
	PackType *pType =(PackType*)szbuf;
	//����Э��ӳ����ṹ�����飩
	int i = 0;
	while(1)
	{
		if(m_aryProtocolMapEntries[i].m_nType == *pType)
		{
			//���ú���ָ��
			(this->*m_aryProtocolMapEntries[i].m_npfun)(sock,szbuf);
			break;
		}
		else if(m_aryProtocolMapEntries[i].m_nType ==0 
			&& m_aryProtocolMapEntries[i].m_npfun ==0)
			break;

		i++;
	}
	
	return true;
}

//ȡ�����ƷID���㷨���ظ���δʵ�֣�
int TCPKernel::GetRandomGoodsID(){
	srand(time(NULL));

	list<string> lststr;
	char szsql[_DEF_SQLLEN] = {0};
	sprintf_s(szsql,"SELECT COUNT(*) FROM goods_name_list;");
	m_sql.SelectMySql(szsql,1,lststr);
	int k = atoi(lststr.front().c_str());
	lststr.pop_front();

	int id = rand() % k + 1;//����1����ǰ�����б������������


	return id;
}

unsigned _stdcall TCPKernel::ThreadGameTimer(void * lpvoid){
	Timer_Thread_Info *tti = (Timer_Thread_Info*)lpvoid;

	DWORD dwWait = ::WaitForSingleObject(tti->m_pKernel->m_hNullEvent, (tti->pRoom->m_good->m_GuessTime + 2) * 1000);

	if(tti->pRoom->isGameon == true){
		//���뱣֤��Ϸ�ǽ����еģ���ֹ��;�˳�
		tti->pRoom->isGameon = false;
		STRU_TIMEOVER_RS tors;
		tors.m_nType = _DEF_PROTOCOL_TIMEOVER_RS;
		tors.m_roomid = tti->pRoom->m_roomid;
		tors.m_PassNum = tti->pRoom->m_passnum;

		//����������Ϣ
		tti->m_pKernel->m_pNet->SendData(tti->pRoom->m_sock,(char*)&tors,sizeof(tors));

		//���������ÿ�����ڷ���Ϣ
		list<Audience_Info*> lstAudience = tti->m_pKernel->m_mapRoomToAudience[tti->pRoom];
		auto pAudience = lstAudience.begin();
		while(pAudience != lstAudience.end())
		{
			tti->m_pKernel->m_pNet->SendData((*pAudience)->m_sock,(char*)&tors,sizeof(tors));
			pAudience++;
		}

		
	}


	delete tti;
	return 0;
}