#ifndef _PACKDEF_H
#define _PACKDEF_H

#define	MYSQL_HOST	"59.110.174.158"		//Mysql������ַ
#define	MYSQL_USER	"root"					//Mysql�û���
#define	MYSQL_PWD	"sgzx123456"			//Mysql����
#define	MYSQL_DB	"draw_guess"			//Mysql���ݿ�

#define SERVER_IP	"127.0.0.1"//�ͻ���ר�ã����ӵķ�������ַ
#define _DEFPORT    23356	//TCP�����˿�
#define _DEFSIZE	1024	//��������С

#define _DEF_PLAYERMESSAG_SIZE 500		//�û�������Ϣ��󳤶�

#define _DEF_STREAMPACKDEF   sizeof(StreamInfo)

#define _DEF_THREADPOOLNUM    1000		//�̳߳��߳�����

#define _DEF_SEMAPHORENUM    10000		//�ź�����Ŀ

#define _BEGIN_PROTOCOL_MAP ProtocolMap   m_aryProtocolMapEntries[]=\
{

#define _END_PROTOCOL_MAP 	{0,0} \
};

#define PM(x,y)  {x,y},

#define _DEF_PROTOCOL_BASE    10

//ע�����󡢻ظ�
#define _DEF_PROTOCOL_REGISTER_RQ    _DEF_PROTOCOL_BASE +1
#define _DEF_PROTOCOL_REGISTER_RS    _DEF_PROTOCOL_BASE +2

//��¼���󡢻ظ�
#define _DEF_PROTOCOL_LOGIN_RQ    _DEF_PROTOCOL_BASE +3
#define _DEF_PROTOCOL_LOGIN_RS    _DEF_PROTOCOL_BASE +4

#define _DEF_PROTOCOL_CREATEROOM_RQ    _DEF_PROTOCOL_BASE +5
#define _DEF_PROTOCOL_CREATEROOM_RS    _DEF_PROTOCOL_BASE +6
#define _DEF_PROTOCOL_ENTERROOM_RQ    _DEF_PROTOCOL_BASE +7

#define _DEF_PROTOCOL_PLAYERNUMRENEW_RS    _DEF_PROTOCOL_BASE +8
#define _DEF_PROTOCOL_GAMESTART_RQ    _DEF_PROTOCOL_BASE +9

#define _DEF_PROTOCOL_GOODSINFO_RS    _DEF_PROTOCOL_BASE +10

#define _DEF_PROTOCOL_PLAYERMESSAG_RQ    _DEF_PROTOCOL_BASE +11
#define _DEF_PROTOCOL_PLAYERMESSAG_RS    _DEF_PROTOCOL_BASE +12

#define _DEF_PROTOCOL_STREAMINFO_RQ    _DEF_PROTOCOL_BASE +13
#define _DEF_PROTOCOL_STREAMINFO_RS    _DEF_PROTOCOL_BASE +14

#define _DEF_PROTOCOL_STREAMCONTENT_RQ    _DEF_PROTOCOL_BASE +15
#define _DEF_PROTOCOL_STREAMCONTENT_RS    _DEF_PROTOCOL_BASE +16

#define _DEF_PROTOCOL_TIMEOVER_RS    _DEF_PROTOCOL_BASE +17

#define _DEF_PROTOCOL_LEAVEROOM_RQ    _DEF_PROTOCOL_BASE +18
#define _DEF_PROTOCOL_CLOSEROOM_RS    _DEF_PROTOCOL_BASE +19

#define _DEF_PROTOCOL_LOGOUT_RQ    _DEF_PROTOCOL_BASE +20

#define _DEF_PROTOCOL_ENTEREOOMRES_RS    _DEF_PROTOCOL_BASE +21


typedef char PackType;

#define _DEF_SIZE   45				//�û�����������󳤶�
#define _DEF_STREAMSIZE   4096		//������
#define _DEF_AUTHORLISTNUM   10
#define _DEF_SQLLEN       300
#define _role_audience   0
#define _role_author     1

#define _register_fail     0
#define _register_success   1
#define _login_fail     0
#define _login_success   1

#define UM_LOGINMSG				WM_USER + 1
#define UM_CREATROOMMSG			WM_USER + 2
#define UM_PLAYERNUMRENEWMSG	WM_USER + 3
#define UM_GETGOODSINFOMSG		WM_USER + 4
#define UM_TIMEREVENTMSG		WM_USER + 5
#define UM_CLOSEROOMMSG			WM_USER + 6
#define UM_ENTERFAILMSG			WM_USER + 7

struct goodinfo{
	int		m_gid;//��ƷID
	char 	m_ThingName[_DEF_SIZE];//��Ʒ����
	char 	m_FirstHint[_DEF_SIZE];//��һ����ʾ
	char 	m_SecondHint[_DEF_SIZE];//�ڶ�����ʾ
	char 	m_ThirdHint[_DEF_SIZE];//��������ʾ
	int		m_GuessTime;//��Ʒ��������ʱ��
	char 	m_AuthorName[_DEF_SIZE];//��������
};

//===========================Э����ṹ����==============================

//ע�ᡢ��¼�����
typedef struct STRU_REGISTER_RQ
{
	PackType m_nType;
	char     m_szUserName[_DEF_SIZE];
	char     m_szPassWord[_DEF_SIZE];
	int      m_szGender;//0����Ů��1�����У�2����δ֪�Ա�3�����¼����
	char	 m_szEmail[_DEF_SIZE];
}STRU_LOGIN_RQ;

//ע�ᡢ��¼�ظ���
typedef struct STRU_REGISTER_RS
{
	PackType	m_nType;
	char		m_szResult;
	long long	m_userid;//���ڵ�¼�ظ�
	char		m_szUserName[_DEF_SIZE];//���ڵ�¼�ظ�
}STRU_LOGIN_RS;

//����Ϣ
struct StreamInfo
{
	PackType m_nType;
	long long m_UserId;
	char      m_szContent[_DEF_STREAMSIZE];
	int       m_nLen;
};

//��������������Ϣ�����䴴������ֵ��Ϣ����ҽ��뷿����Ϣ
typedef struct STRU_CREATEROOM_RQ{
	PackType m_nType;
	long long m_uid;//����UID�����UID
	char 	m_PlayerName[_DEF_SIZE];//�����û���������û���
	long long m_roomid;//����id���ڷ��䴴������ֵ��Ϣ����Ч����Ϊ-1����˵������ʧ�ܣ�						����ҽ��뷿����Ϣ����Ч������Ҫ����ķ���
}STRU_CREATEROOM_RS,STRU_ENTERROOM_RQ;

//����ˢ����Ϣ����Ϸ��ʼ��Ϣ
typedef struct STRU_PLAYERNUMRENEW_RS{
	PackType m_nType;
	long long m_uid;//����UID
	long long m_roomid;//����id
	int m_playernum;//��������(��������)������Ϸ��ʼ��Ϣ����Ч����Ϊ-1��
}STRU_GAMESTART_RQ;

//��Ϸ������Ϣ
struct STRU_GOODSINFO_RS{
	PackType m_nType;
	long long m_uid;//����UID
	long long m_roomid;//����id
	char 	m_ThingName[_DEF_SIZE];//��Ʒ����
	char 	m_FirstHint[_DEF_SIZE];//��һ����ʾ
	char 	m_SecondHint[_DEF_SIZE];//�ڶ�����ʾ
	char 	m_ThirdHint[_DEF_SIZE];//��������ʾ
	int		m_GuessTime;//��Ʒ��������ʱ��
	char 	m_AuthorName[_DEF_SIZE];//��������
};

//������Ϣ������������ת���ģ�
typedef struct STRU_PLAYERMESSAG_RQ{
	PackType	m_nType;
	long long	m_playeruid;//����Ϣ��ҵ�UID
	char		m_playername[_DEF_SIZE];//����Ϣ��ҵ��û���
	long long	m_roomid;//����id
	char 		m_MessageContent[_DEF_PLAYERMESSAG_SIZE];//��Ϣ����
	int			m_GuessResult;//�²�����0Ϊδ���У�1Ϊ���У�2Ϊ��ǰ������Ϸ������
}STRU_PLAYERMESSAG_RS;

//��Ϸ��ʱ��Ϣ
struct STRU_TIMEOVER_RS{
	PackType m_nType;
	long long m_roomid;//����id
	int 		m_PassNum;//�¶Ե��������
	//����ṹ����ڿ��Ծ�����£����ݷ������洢�Ľṹ����
};

//�˷���Ϣ�������رշ�����Ϣ
typedef struct STRU_LEAVEROOM_RQ{
	PackType m_nType;
	long long m_roomid;//����id
	long long m_playeruid;//�˷���ҵ�UID
	bool 	m_isRoomOwner;//�Ƿ�Ϊ����
}STRU_CLOSEROOM_RS;
 
//������Ϣ
struct STRU_LOGOUT_RQ{
	PackType m_nType;
	long long	m_userid;//���ߵ����ID
};

//��ҽ���ʧ����Ϣ
struct STRU_ENTERROOMFAIL_RS{
	PackType m_nType;
	int	result;//ʧ��ԭ��
};

#endif