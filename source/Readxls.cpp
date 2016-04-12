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

    // 获取已安装驱动的名称(涵数在odbcinst.h里)
    if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
        return "";

    // 检索已安装的驱动是否有Excel...
    do
    {
        if (strstr(pszBuf, "Excel") != 0)
        {
            //发现 !
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

    // 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
    sDriver = GetExcelDriver();
    if (sDriver.IsEmpty())
    {
        // 没有发现Excel驱动
        AfxMessageBox("没有安装Excel驱动!");
        return ret;
    }

    // 创建进行存取的字符串
    sDsn.Format("ODBC;DRIVER={%s};DSN='';DBQ=%s", sDriver, filename);
    TRY
    {
        // 打开数据库(既Excel文件)
       database.Open(NULL, false, false, sDsn);
		
		CRecordset recset(&database);

        // 设置读取的查询语句.
		  sSql = "SELECT * FROM [Sheet1$]";

        // 执行查询语句
        recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly);

        // 获取查询结果
        while (!recset.IsEOF())
        {
            //读取Excel内部数值
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
            // 移到下一行
            recset.MoveNext();
        }

        // 关闭数据库
        database.Close();
		  ret = true;
    }

    CATCH(CDBException, e)
    {
        // 数据库操作产生异常时...
        AfxMessageBox("Excel格式错误: " + e->m_strError);
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
	fieldlist[0].push_back("姓名");
	fieldlist[1].push_back("学号");
	ReadFromExcel(xlsfile,fieldlist);

	size_t fsize=fieldlist.size();
	size_t rsize=fieldlist[0].size();
	for (size_t i=1;i<rsize;++i)
	{
		CString orgfilename = dir + fieldlist[1][i] + ".jpg";
		CString newfilename = dir + fieldlist[1][i] +"_" + fieldlist[0][i] + ".jpg";
		if (rename(orgfilename.GetString(),newfilename.GetString()))
		{
			AfxMessageBox("发生错误：替换文件名\n"+orgfilename+"\n到\n"+newfilename+"\n不能完成！");
		}
	}
	AfxMessageBox("替换结束！");
}