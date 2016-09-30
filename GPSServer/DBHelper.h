#include "stdafx.h"
#include  "winsock.h"
#include "GPSServer.h"
#include "mysql.h" 
#include <string>
using namespace std;

class DBHelper
{
	MYSQL mysql;
	MYSQL_RES *resultset;  
public:
	/************************************************************************/
	/* �������ݿ����                                                                     */
	/************************************************************************/
     bool ConnectDatabase(){
		 try 
		 {  
			 mysql_init(&mysql);  //����mysql�����ݿ�

			 //����false������ʧ�ܣ�����true�����ӳɹ�
			 if (!(mysql_real_connect(&mysql,"localhost", "root", "", "gps",0,NULL,0))) //�м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ
			 {
				 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"���ݿ�����ʧ�ܣ�");
				 return FALSE;  

			 }
			 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"���ݿ����ӳɹ���");
			 return TRUE;
		 }  
		 catch (...)  
		 {  
			 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUSINFO,0,(LPARAM)"���ݿ�����ʧ�ܣ�");
			 return FALSE;  
		 } 

	 }
	 /************************************************************************/
	 /*             ���ݿ�������*/
	 /************************************************************************/
	 void InsertData(CString Latitude,CString Longitude,CString diviceIMEI,CString Data )
	 {
		 CString sql;

		 sql.Format(("insert into locationdata(Latitude,Longitude,DiviceIMEI,Date)values(%s,%s,%s,%s)"), Latitude,Longitude,diviceIMEI,Data);

		 if(mysql_query(&mysql, sql))        //ִ��SQL���
		 {
			 printf("��������ʧ��");
		 }
		 else
		 {
			 printf("�������ݳɹ�");
		 }
	 }
	 /************************************************************************/
	 /* �û���¼��֤����                                                                     */
	 /************************************************************************/
	 void LoginCheck(SOCKET  wParam,CString UserName,CString Password)
	 {
		 CString sql;

		 sql.Format(("select * from user where UserName='%s' and UserPassword = %s"), UserName,Password);

		 //select * from user where UserName='admin' and UserPassword = 123456]"	ATL::CStringT<char,StrTraitMFC_DLL<char,ATL::ChTraitsCRT<char> > >

		 if(mysql_query(&mysql,sql))        //ִ��SQL���
		 {
			 printf("���ݿ��ѯʧ��");
		 }
		 else
		 {  
			 resultset = mysql_store_result(&mysql);// ��ý����           
			 if (mysql_num_rows(resultset) != NULL)  
			 {//��ѯ�����û� ,�������¼���ֻ����ƶ˷��ص�¼�ɹ���Ϣ
				 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"[ST*Login*OK]");
			 }  
			 else  
			 {  
				 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"[ST*Login*Failure]");
			 }   
			 mysql_free_result(resultset);  // �ͷŽ����  
		 }  
	 }
	 /************************************************************************/
	 /* ��ѯ�����豸 ����ѯȺ�����IMEI��Ϊ��IMEI�ŵ����ű��豸                                                               */
	 /************************************************************************/
	 void GetDivice(SOCKET  wParam,CString IMEI)
	 {

		 CString sql;

		 CString diviceinfo;

		 MYSQL_ROW row;    

		 string diviceName,diviceNum,online,diviceIMEI;

		 string n_str = "[ST*SetDivice*OK,";

		 sql.Format(("select * from divicegroup where IMEI = %s"), IMEI);

	      if(mysql_query(&mysql,sql))  
			 {  
		
			 }  
			 else  
			 {                      
				 //��������           
				 resultset = mysql_store_result(&mysql);// ��ý����           
				 if (mysql_num_rows(resultset) != NULL)  
				 {  
					 int numRows = mysql_num_rows(resultset); // ��ý�����еļ�¼��  
					 int numFields = mysql_num_fields(resultset);// ��ñ����ֶ���  
				  
					 while (row = mysql_fetch_row(resultset))  
					 {  
							 diviceName = row[2];
							 diviceNum = row[3];
							 diviceIMEI = row[4];
							 online   = row[5];
							 n_str += diviceName+"#"+diviceNum+"#"+diviceIMEI+"#"+online+",";
				
					 }  
					 n_str+="]";
			
					 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)n_str.c_str());
				
				 }  
				 else  
				 {  
					 n_str+="]";
					 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)n_str.c_str());

				 }   
				 mysql_free_result(resultset);  // �ͷŽ����  
		 }

	 }

	 /************************************************************************/
	 /*      ��ȡ�켣����                                                                */
	 /************************************************************************/
	 void GetLocus(SOCKET  wParam,CString diviceIMEI)
	 {
		 CString sql;

		  MYSQL_ROW row;   

		   string Locus_str = "[ST*GetLocus*OK";

		  string Date;

		   sql.Format(("select * from locationdata where DiviceIMEI=%s"), diviceIMEI);

		   if(mysql_query(&mysql,sql))  
		   {  
			   //��ѯʧ��
		   }  
		   else  
		   {                
		      //��������
			   resultset = mysql_store_result(&mysql);// ��ý����           
			   if (mysql_num_rows(resultset) != NULL)  
			   {  
				   while (row = mysql_fetch_row(resultset))  
				   {  
				       Date = row[4];

					   string::size_type idx = Locus_str.find( Date);

					   if(idx==string::npos){

						    Locus_str+=","+Date;
					   }
				   }
				   Locus_str+="]";
				    ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)Locus_str.c_str());
			   }else{
				   Locus_str+="]";
				   ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)Locus_str.c_str());
			   }
				 
			    mysql_free_result(resultset);  // �ͷŽ����  

		   }
	 }
	 /************************************************************************/
	 /*                                                                      */
	 /************************************************************************/
	 void GetLocusDetails(SOCKET  wParam,CString diviceIMEI,CString date)
	 {
		 CString sql;

		 MYSQL_ROW row;   

		 string LocusDetails_str = "[ST*LocusDetails*OK";

		 string Longitude,Latitude;

		 sql.Format(("select * from locationdata where DiviceIMEI=%s and Date = %s"), diviceIMEI,date);

		 if(mysql_query(&mysql,sql))  
		 {  
			 //��ѯʧ��
		 }  
		 else  
		 {
			 //��ʼ��������
			 //��������
			 resultset = mysql_store_result(&mysql);// ��ý����           
			 if (mysql_num_rows(resultset) != NULL)  
			 {  
				 while (row = mysql_fetch_row(resultset))  
				 { 
			    	 Longitude=row[1];
			    	 Latitude = row[2];
					 LocusDetails_str+=","+Longitude+"&"+Latitude;
				 }
				 LocusDetails_str+="]";
	
				 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)LocusDetails_str.c_str());
			 }
		 }
		 mysql_free_result(resultset);  // �ͷŽ����  
	 }


	 /************************************************************************/
	 /* �ֻ��ͻ�����ӿɿص��豸��                                                                     */
	 /************************************************************************/
	 void ADDDivice(SOCKET  wParam,CString IMEI,CString DiviceName,CString DiviceNum,CString DiviceIMEI)
	 {
		 CString sql;

		 sql.Format(("insert into divicegroup(IMEI,DiviceName,DiviceNum,DiviceIMEI)values(%s,'%s',%s,%s)"), IMEI,DiviceName,DiviceNum,DiviceIMEI);

		 if(mysql_query(&mysql, sql))        //ִ��SQL���
		 {
			 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"ST*ADDDivice*Failure");
			 printf("��������ʧ��");
		 }
		 else
		 {
			  ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"ST*ADDDivice*OK");
			 printf("�������ݳɹ�");
		 }
	 }
	 /************************************************************************/
	 /* �ֻ��ͻ���ɾ���ɿص��豸                                                                     */
	 /************************************************************************/
	 void DeleteDivice(SOCKET  wParam,CString IMEI,CString DiviceIMEI)
	 {
		 CString sql;

		 sql.Format(("delete  from divicegroup where IMEI=%s and DiviceIMEI=%s"), IMEI,DiviceIMEI);
		 if(mysql_query(&mysql, sql))        //ִ��SQL���
		 {
			 ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"ST*DeleteDivice*Failure");
			  printf("ɾ������ʧ��");
		 }else {
			  ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)"ST*DeleteDivice*OK");
			  printf("ɾ�����ݳɹ�");
		 }

	 }
	 /************************************************************************/
	 /*       �ֻ����ƶ�����ָ���豸��ʵʱ��λ����
	          ��ָ�����豸��������ʵʱ��λ������
	 */
	 /************************************************************************/
	 void GetlatLng(SOCKET  wParam,string IMEI)
	 {

	::SendMessage(AfxGetMainWnd()->m_hWnd,WM_SENDINFO,(SOCKET)wParam,(LPARAM)IMEI.c_str());
	 }
	  /************************************************************************/
	 /*     �������ݿ��ű��豸����״̬
	 */
	 /************************************************************************/
	 void UpdateOnLine(CString IMEI){
		 CString sql;

		 sql.Format(("UPDATE divicegroup set DiviceOnLine=0 where DiviceIMEI = %s"), IMEI);

		 if(mysql_query(&mysql, sql))        //ִ��SQL���
		 {
		 
		 }else{

		 }
	
	 }
	   /************************************************************************/
	 /*     �������ݿ��ű��豸����״̬
	 */
	 /************************************************************************/
	 void UpdateOnLine1(CString IMEI){
		 CString sql;

		 sql.Format(("UPDATE divicegroup set DiviceOnLine=1 where DiviceIMEI = %s"), IMEI);

		 if(mysql_query(&mysql, sql))        //ִ��SQL���
		 {
			

		 }else{


		 }

	 }

};