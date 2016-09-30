
// GPSServerDlg.cpp : ʵ���ļ�
#include "stdafx.h"
#include "GPSServer.h"
#include "WebPage.h"
#include "GPSServerDlg.h"
#include "afxdialogex.h"
#include <string>
#include<time.h>
#include <map>
#include  "winsock.h"
#include "mysql.h" 
#include <json.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	


END_MESSAGE_MAP()


// CGPSServerDlg �Ի���




CGPSServerDlg::CGPSServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGPSServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_JD = _T("");
	m_WD = _T("");
}

void CGPSServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_SHOWINFO,mshowinfo);

}

BEGIN_MESSAGE_MAP(CGPSServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	//�����Ϣӳ��
	ON_MESSAGE(WM_STATUSINFO,dealwith_statusmsg)
	ON_MESSAGE(WM_RECEIVEINFO,dealwith_parammsg)
	ON_MESSAGE(WM_DELETEINFO,dealwith_deletemsg)
	ON_MESSAGE(WM_SENDINFO,dealwith_sendmsg)
END_MESSAGE_MAP()


// CGPSServerDlg ��Ϣ�������

BOOL CGPSServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	dbHelper = new DBHelper();
	if(!dbHelper->ConnectDatabase()){
				return FALSE;
	}
	//����socket��ʼ���߳�
	m_pThread_server=AfxBeginThread(ThreadFunction_server,NULL);//�������߳�

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


/************************************************************************/
/* ����չʾ�������Լ����ݿ����е�״̬                                                                     */
/************************************************************************/
LRESULT CGPSServerDlg::dealwith_statusmsg(WPARAM wParam,LPARAM lparam)
{
	CString pstr;
	pstr=(char *)lparam;
	GetDlgItem(IDC_EDIT1)->SetWindowText(pstr);
	return 0;
}

/************************************************************************/
/* ���߳�����socket �����˿� �����պ��·���Ϣ                                                                    */
/************************************************************************/
UINT ThreadFunction_server( LPVOID pParam )
{
	WSADATA wsa;
	SOCKET Listen_Sock;

	SOCKADDR_IN serverAddr;//����һ��SOCKADDR_IN�ṹ�Ķ���
	//��ʼ��socket��Դ
	if (WSAStartup(MAKEWORD(1,1),&wsa) != 0)
	{
		
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"��ʼ��ʧ�ܣ�");
		WSACleanup();
	}
	//����socket
	Listen_Sock = socket(AF_INET, SOCK_STREAM, 0);
	if(Listen_Sock==INVALID_SOCKET)
	{

		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"socket����ʧ�ܣ�");
	}


	//ZeroMemory((char *)&serverAddr,sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(20086); //���ؼ����˿�:20086
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//���׽��ְ󶨵�ַ
	if(bind(Listen_Sock,(sockaddr *)&serverAddr,sizeof(serverAddr))==SOCKET_ERROR)
	{
		//printf("%d",WSAGetLastError()); 
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"��ʧ�ܣ�");
		closesocket(Listen_Sock);

	}else{
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"�������ɹ�");
	}

	AfxBeginThread(ThreadFunction_listen,(LPVOID)Listen_Sock);//���������׽��ֵ��߳�
	WaitForSingleObject(hEvents_listen,INFINITE);//���߳�ִ����֮ǰ�������߳�

     AfxBeginThread(ThreadFunction_accept,(LPVOID)Listen_Sock);//����accept�߳�
	WaitForSingleObject(hEvents_accept,INFINITE);//���߳�ִ����֮ǰ�������߳�
	return 0;
}
/************************************************************************/
/*       ThreadFunction_listen    :�����̺߳���                                              */
/************************************************************************/
UINT ThreadFunction_listen( LPVOID pParam )
{
	if(listen((SOCKET)pParam,50)==SOCKET_ERROR)
	{
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"����ʧ�ܣ�");
		closesocket((SOCKET)pParam);
		exit(0);
	}
	else
	{
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"���ڼ���...");
	}
	SetEvent(hEvents_listen);
	return 0;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
UINT ThreadFunction_accept( LPVOID pParam )
{
	SOCKET  sockClient;
	SOCKADDR_IN  clientAddr;
	int sizeclient=sizeof(clientAddr);
	//ѭ��accept���ö���ͻ������� ʹ��while(1)��û����������
	while(1)
	{
		sockClient = accept((SOCKET)pParam,(sockaddr *)&clientAddr,&sizeclient);//ִ��accept���ȴ��ͻ�������
		if(sockClient==INVALID_SOCKET)
		{
			//�ͻ�������ʧ��
			continue;
		}
		//�ͻ������ӳɹ�
		AfxBeginThread(ThreadFunction_recv,(LPVOID)sockClient);//���������߳�
	}
	SetEvent(hEvents_accept);//�߳�ͬ��
	return 0;

}

/************************************************************************/
/*   ThreadFunction_recv������Ϣ�߳�
       ������Ϣ��Ϊ�����ű���Ϣ�ͽ���
	   ���Կ����ֻ�����Ϣ
*/
/************************************************************************/
UINT ThreadFunction_recv( LPVOID pParam )
{
	//ѭ������
	while(1)
	{
		char recvbuf[1024]={0};
		int recvd=recv((SOCKET)pParam,recvbuf,sizeof(recvbuf)+1,0);
		string str(recvbuf);
		if(recvd<=0){
			//����ѭ����
			break;
		}else{
			//�Խ��յ�����Ϣ�ֱ���
			//����ͻ��˷���ʵʱ��������Ϣ
			::SendMessage(AfxGetMainWnd()->m_hWnd,WM_RECEIVEINFO,(SOCKET)pParam,(LPARAM)recvbuf);
		}
	}
	//�ͻ������ӶϿ���ɾ��map_socket���GPSlist�б�ؼ���ĸ���������ӵ��ű��豸���ֻ����ƶˣ�
	
		::SendMessage(AfxGetMainWnd()->m_hWnd,WM_DELETEINFO,(SOCKET)pParam,0);

	return 0;
}

/************************************************************************/
/* ������յ�����Ϣ��     
[3G*3916377609*0173*UD2,240916,044545,V,31.200533,N,121.4615883,E,0.00,0.0,0.0,0,100,98,0,0,00000010,7,0,460,0,6243,53825,160,6243,53826,167,6243,55377,154,6243,53827,150,6243,54178,146,6243,53362,144,6243,53363,143,5,jamestplink,c0:61:18:b:95:ce,-45,STWIFI005,0:c:1b:2d:1e:5a,-49,HP-Print-99-LaserJet 1025,d8:5d:e2:2b:1b:99,-61,GLEXER_004C88,cc:a3:74:0:4c:88,-63,,ca:d7:19:f0:ad:9a,-82,41.5]

*/
/************************************************************************/
LRESULT  CGPSServerDlg::dealwith_parammsg(WPARAM wParam,LPARAM lparam)
{
	string receivemsg="";
	receivemsg = (char *)lparam;
	string ordertype=receivemsg.substr(1,2);//��������"ST"��ʾ��ʱ�ֻ����ƶ˵����"3G"��ʾ�����ն˶�λ�豸������
	if(ordertype=="ST"){//[ST*358584059950478*Connect]
		string imei = receivemsg.substr(4,15);
		map<SOCKET,string>::iterator iter=STmap_socket.find((SOCKET)wParam);
		  vector <string> master_msg = split(receivemsg,"*,");//�����ֻ����ƶ˷��͵���Ϣ����
		if(iter!=STmap_socket.end()){//��STmap_socket�д��ڸ�����,���������ʽ��������
		  	if(master_msg[2]=="GetDivice]"){
				//��ѯ���ݿ����豸�б�
				dbHelper->GetDivice((SOCKET)wParam,master_msg[1].c_str());
			}else if(master_msg[2]=="ADDDivice"){
				dbHelper->ADDDivice((SOCKET)wParam,master_msg[1].c_str(),master_msg[3].c_str(),master_msg[4].c_str(),master_msg[5].c_str());
			}else if(master_msg[2]=="DeleteDivice"){
				dbHelper->DeleteDivice((SOCKET)wParam,master_msg[1].c_str(),master_msg[3].c_str());
			}else if(master_msg[3]=="GetlatLng]"){
				string getLatlng = "";
				//�ֻ����ƶ�����ĳһ�豸��ʵʱλ�á���Ҫ�ҳ����豸������map_socket�е����ӡ�
				//������豸��������ʵʱ��λ���
				for(map <SOCKET,string>::iterator it=map_socket.begin();it!=map_socket.end();it++)
				{
					if(it->second==master_msg[2]){

						multimap<SOCKET,SOCKET>::iterator Forwarding_iter=Forwarding_map.find((SOCKET)wParam);
						
						if(Forwarding_iter!=Forwarding_map.end()){

						}else{

							Forwarding_map.insert(pair<SOCKET,SOCKET>(wParam,it->first));
						}

						getLatlng = "[3G*"+master_msg[2]+"*0002*CR]";
						dbHelper->GetlatLng((SOCKET)it->first,getLatlng);
					}
				}
			}else if(master_msg[3]=="GetLocus]"){
				//�ֻ����ƶ��������ݿ��й켣���ݡ�
				dbHelper->GetLocus((SOCKET)wParam,master_msg[2].c_str());
			}else if(master_msg[4]=="LocusDetails]"){
				//�������Ĺ켣����
				dbHelper->GetLocusDetails((SOCKET)wParam,master_msg[2].c_str(),master_msg[3].c_str());
			}
		}else{//��STmap_socket�в����ڸ�����
		     STmap_socket[(SOCKET)wParam] = imei;
			 if(master_msg[2]=="Connect]"){
				 //��ѯ���ݿ���֤��¼��Ϣ [ST*123547896354781*Login,admin,123456]
				 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"[ST*Login*OK]");
			 }
	}
	}else{
		string mimei = receivemsg.substr(4,10);
		string ordermsg = receivemsg.substr(20,2);
		map<SOCKET,string>::iterator iter=map_socket.find((SOCKET)wParam);
		if(iter!=map_socket.end()){//��map_socket�д��ڸ�����,���������ʽ��������
			if(ordermsg=="UD"){//�ն��豸����λ���ϱ���Ϣ������λ����Ϣ�����ݿ⣬�����µ�ǰλ����Ϣm_JD��mm_WD;
			string n_str="";
			 n_str = "[ST*Divice*GetLatLng,";
			vector <string> location_msg = split(receivemsg,",");	
			m_JD = location_msg[6].c_str();
	    	m_WD = location_msg[4].c_str();
			m_date= location_msg[1].c_str();
			m_DiviceIMEI = mimei.c_str();
			dbHelper->InsertData(m_WD,m_JD,m_DiviceIMEI,m_date);
	
			//���ת�������Ƿ��и��豸ת�������Ӵ���������ھͽ���ת�����ֻ����ƶ˵Ĳ���
			for(multimap <SOCKET,SOCKET>::iterator it=Forwarding_map.begin();it!=Forwarding_map.end();it++)
			{
		
				if(it->second==((SOCKET)wParam))
				{
					
					n_str+=receivemsg;
					 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,it->first,(LPARAM)n_str.c_str());
					
				}
			}
		  }else if(ordermsg=="LK"){
			  //��·����
			  ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)receivemsg.c_str());

			
			}
		}else{//��map_socket�в����ڸ�����
		
			map_socket[(SOCKET)wParam] = mimei;
			dbHelper->UpdateOnLine1(mimei.c_str());
			::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)receivemsg.c_str());
		}
	}
	return 0;
}
/************************************************************************/
/* �ͻ������ӶϿ������������ݿ������ű��豸�������롢�ߴ��� */
/************************************************************************/
LRESULT CGPSServerDlg::dealwith_deletemsg(WPARAM wParam,LPARAM lparam)
{
	string delete_IMEI="";
	//ɾ���豸���ӱ�
	for(map <SOCKET,string>::iterator it=map_socket.begin();it!=map_socket.end();)
	{
		if(it->first==((SOCKET)wParam))
		{
			delete_IMEI = it->second;		
			map_socket.erase(it);
			break;
		}else 
			it++;
	}
	dbHelper->UpdateOnLine(delete_IMEI.c_str());
   //ɾ��ת�����еļ�¼
	for(multimap <SOCKET,SOCKET>::iterator it=Forwarding_map.begin();it!=Forwarding_map.end();)
	{
		if(it->first==((SOCKET)wParam))
		{
			Forwarding_map.erase(it);
			break;
		}else 
			it++;
	}
	//ɾ���ֻ����ƶ����ӱ�
	for(map <SOCKET,string>::iterator it=STmap_socket.begin();it!=STmap_socket.end();)
	{

		if(it->first==((SOCKET)wParam))
		{
			STmap_socket.erase(it);
			break;
		}else 
			it++;
	}
	dealwith_showmsg("","�豸"+delete_IMEI+"�Ͽ�����");
	return 0;
}
/************************************************************************/
/*     ��������Ϣ   
       wParam ��socket����
	   lparam  ����Ҫ���͵���Ϣ
*/                             
/************************************************************************/
LRESULT CGPSServerDlg::dealwith_sendmsg(WPARAM wParam,LPARAM lparam)
{

	char sendBuf[1024*10]={0};
	strcpy(sendBuf,(char *)lparam);
	dealwith_showmsg("",sendBuf);
	int byte = 0;
	byte= send((SOCKET)wParam,sendBuf,strlen(sendBuf),0);
	if(byte<=0){

	}

	return 0;
}
/************************************************************************/
/* �ָ��ַ���.c++û���ֳɵ�split�������Լ�ʵ��     */
/************************************************************************/
	vector<string> CGPSServerDlg::split(string& s, string seperator)  
	{  
		vector<string> result;
		typedef string::size_type string_size;
		string_size i = 0;

		while(i != s.size()){
			//�ҵ��ַ������׸������ڷָ�������ĸ��
			int flag = 0;
			while(i != s.size() && flag == 0){
				flag = 1;
				for(string_size x = 0; x < seperator.size(); ++x)
					if(s[i] == seperator[x]){
						++i;
						flag = 0;
						break;
					}
			}

			//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
			flag = 0;
			string_size j = i;
			while(j != s.size() && flag == 0){
				for(string_size x = 0; x < seperator.size(); ++x)
					if(s[j] == seperator[x]){
						flag = 1;
						break;
					}
					if(flag == 0) 
						++j;
			}
			if(i != j){
				result.push_back(s.substr(i, j-i));
				i = j;
			}
		}
		return result;
	}  
	/************************************************************************/
	/* ��Ϣ��ʾ����                                                                     */
	/************************************************************************/
	void CGPSServerDlg:: dealwith_showmsg(string user,string  show_msg )
	{
	
		show_num++;
		if(show_num>20){
			show_num=0;
			mshowinfo.SetWindowText("");
		}
		time_t t = time(0); 
		char tmp[64]; 
		strftime( tmp, sizeof(tmp), "%Y-%m-%d %X",localtime(&t) ); 
		string str_time(tmp);
		string str =str_time+":"+show_msg+"\r\n";
		 mshowinfo.ReplaceSel(str.c_str());
	}
   void CGPSServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
   {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
  }

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGPSServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��  121.464325  31.19897
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGPSServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

