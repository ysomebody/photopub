#include "stdafx.h"
#include <vector>
#include <errno.h>
using namespace std;

CString GetExcelDriver()
{
    char szBuf[2001];
    WORD cbBufMax = 2000;
    WORD cbBufOut;
    char *pszBuf = szBuf;
    CString sDriver;

    // ��ȡ�Ѱ�װ����������(������odbcinst.h��)
    if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
        return "";

    // �����Ѱ�װ�������Ƿ���Excel...
    do
    {
        if (strstr(pszBuf, "Excel") != 0)
        {
            //���� !
            sDriver = CString(pszBuf);
            break;
        }
        pszBuf = strchr(pszBuf, '\0') + 1;
    }

    while (pszBuf[1] != '\0');
    return sDriver;
}


bool ReadFromExcel(const CString &filename, vector< vector<CString> > &fieldlist) 
{
    CDatabase database;
    CString sSql;
    CString sItem1, sItem2;
    CString sDriver;
    CString sDsn;
	 bool ret = false;

    // �����Ƿ�װ��Excel���� "Microsoft Excel Driver (*.xls)" 
    sDriver = GetExcelDriver();
    if (sDriver.IsEmpty())
    {
        // û�з���Excel����
        AfxMessageBox("û�а�װExcel����!");
        return ret;
    }

    // �������д�ȡ���ַ���
    sDsn.Format("ODBC;DRIVER={%s};DSN='';DBQ=%s", sDriver, filename);
    TRY
    {
        // �����ݿ�(��Excel�ļ�)
       database.Open(NULL, false, false, sDsn);
		
		CRecordset recset(&database);

        // ���ö�ȡ�Ĳ�ѯ���.
		  sSql = "SELECT * FROM [Sheet1$]";

        // ִ�в�ѯ���
        recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly);

        // ��ȡ��ѯ���
        while (!recset.IsEOF())
        {
            //��ȡExcel�ڲ���ֵ
			  size_t size=fieldlist.size();
			  for (size_t i = 0;i<size;++i)
			  {
	            recset.GetFieldValue(fieldlist[i][0], sItem1);
					int dotpos=sItem1.ReverseFind('.');
					if (dotpos!=-1)
					{
						sItem1 = sItem1.Left(dotpos);
					}
					fieldlist[i].push_back(sItem1);
			  }
            // �Ƶ���һ��
            recset.MoveNext();
        }

        // �ر����ݿ�
        database.Close();
		  ret = true;
    }

    CATCH(CDBException, e)
    {
        // ���ݿ���������쳣ʱ...
        AfxMessageBox("Excel��ʽ����: " + e->m_strError);
		  if (database.IsOpen())
			  database.Close();
    }
    END_CATCH;
	 return ret;
}

void RenameDirectory(const CString &dir, const CString &xlsfile)
{
	vector< vector<CString> > fieldlist;
	fieldlist.push_back(vector<CString>());
	fieldlist.push_back(vector<CString>());
	fieldlist[0].push_back("����");
	fieldlist[1].push_back("ѧ��");
	ReadFromExcel(xlsfile,fieldlist);

	size_t fsize=fieldlist.size();
	size_t rsize=fieldlist[0].size();
	for (size_t i=1;i<rsize;++i)
	{
		CString orgfilename = dir + fieldlist[1][i] + ".jpg";
		CString newfilename = dir + fieldlist[1][i] +"_" + fieldlist[0][i] + ".jpg";
		if (rename(orgfilename.GetString(),newfilename.GetString()))
		{
			AfxMessageBox("���������滻�ļ���\n"+orgfilename+"\n��\n"+newfilename+"\n������ɣ�");
		}
	}
	AfxMessageBox("�滻������");
}