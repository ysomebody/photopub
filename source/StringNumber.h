#pragma once

class CStringNumber
{
	CString number;

	CStringNumber();
public:
	CStringNumber(const CString& str)
	{
		for (int i=0;i<str.GetLength();++i){
			if (str[i]>='0' && str[i]<='9' ) {
				number.AppendChar(str[i]);
			}
		}
	}
	void ToString(CString& str)
	{
		int idx=0;
		for (int i=0;i<str.GetLength();++i)
			if (str[i]>='0' && str[i]<='9') {
				str.SetAt(i,number[idx++]);
			}
	}
	void Increase()
	{
		for (int i=number.GetLength()-1;i>=0;--i){
			if (number[i]!='9'){
				number.SetAt(i,number[i]+1);
				break;
			}else {
				number.SetAt(i,'0');
			}
		}
	}
	~CStringNumber();
};
