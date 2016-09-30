
// GPSServerDlg.h : ͷ�ļ�
//
#pragma once
#include <Windows.h>
#include "afxwin.h"
#include "afxcmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include "DBHelper.h"
#include <cstring>

UINT ThreadFunction_server( LPVOID pParam );
UINT ThreadFunction_listen( LPVOID pParam );
UINT ThreadFunction_accept( LPVOID pParam );
UINT ThreadFunction_recv( LPVOID pParam);
//�߳�ͬ���õ��ı���
static CEvent hEvents_listen;
static CEvent hEvents_accept;
//���������Ϸ��������豸��Ϣ��
static map<SOCKET,string> map_socket;
//���������Ϸ��������ֻ����ƶ���Ϣ
static map<SOCKET,string> STmap_socket;
//����һ��ת�����������ض��豸����ָ���豸�Ķ�λ����ʱ�����������ظ���������ض�
//ʹ��multimap��ʵ�ֶ�Զ�Ĺ�ϵ��
static multimap<SOCKET,SOCKET>Forwarding_map;


// CGPSServerDlg �Ի���
class CGPSServerDlg : public CDialogEx
{
// ����
public:
	CGPSServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GPSSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CWinThread *m_pThread_server;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg  bool ConnectDatabase();//�������ݿ�
    afx_msg vector <string>split(string& str, string separate_character);
	afx_msg LRESULT dealwith_statusmsg(WPARAM wParam,LPARAM lparam);
	afx_msg LRESULT dealwith_parammsg(WPARAM wParam,LPARAM lparam);
	afx_msg LRESULT dealwith_deletemsg(WPARAM wParam,LPARAM lparam);
	afx_msg LRESULT dealwith_sendmsg(WPARAM wParam,LPARAM lparam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	
	 int show_num;
	CString	m_JD;
	CString	m_WD;
	CString    m_date;
	CString    m_time;
	CString    m_DiviceIMEI;
	DBHelper* dbHelper;
	CEdit mshowinfo;
	 void dealwith_showmsg(string user,string show_msg);
};
