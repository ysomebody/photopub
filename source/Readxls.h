#pragma once
#include <vector>

CString GetExcelDriver();
bool ReadFromExcel(const CString &filename, std::vector< std::vector<CString> > &fieldlist) ;

void RenameDirectory(const CString &dir, const CString &xlsfile);