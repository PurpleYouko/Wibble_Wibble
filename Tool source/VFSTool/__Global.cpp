#include "stdafx.h"
#include "__Global.h"
#include "Dlg_Main.h"
#include <string>
#include <vector>
#include "TriggerVFS.h"

bool __SetCurrendDirectory (char * Path)
{
	SetCurrentDirectory ((LPCTSTR)Path);

	return true;
}


bool __SetAbsPath (char * Path)
{

	return true;
}


/// 현재 기준 경로를 가져옴
CString __GetAbsPath (void)
{
	return ::g_pDlg_Main->GetAbsPath ();
}


/// 문자열을 lStart부터 잘라서 리턴한다
CString __SubString (int lStart, CString strOrg)
{
	std::string str = strOrg.GetString ();
	std::string strSub;

	strSub = str.substr (lStart, strOrg.GetLength () - lStart);

	CString strString = strSub.c_str ();

	return strString;
}


/// 제외 시킬 확장자를 검색한다
bool __GetExcExt (std::vector<std::string> *vec_exc_ext)
{
	CString strExc = ::g_pDlg_Main->GetExcExt ();
	if(strExc.GetLength () <= 0) { return true; }
	char * buff = new char [strExc.GetLength ()];

	if(buff)
	{
		strcpy (buff, strExc.GetString ());
		char * token = strtok (buff, ";");
		while (token)
		{
			vec_exc_ext->push_back (token);
			token = strtok (NULL, ";");
		}

		return true;
	}
	
	return false;
}


/// 제외시킬 폴더를 검색한다
bool __GetExcDir (std::vector<std::string> *vec_exc_dir)
{
	CString strExc = ::g_pDlg_Main->GetExcPrefix ();
	if(strExc.GetLength () <= 0) { return true; }
	char * buff = new char [strExc.GetLength ()];

	if(buff)
	{
		strcpy (buff, strExc.GetString ());
		char * token = strtok (buff, ";");
		while (token)
		{
			vec_exc_dir->push_back (token);
			token = strtok (NULL, ";");
		}

		return true;
	}
	
	return false;
}


//bool RemoveFileFromVF (VFile *pVFile , const char * FileName)
//{
//	::vf_removefile2 (FileName, pVFile);
//}