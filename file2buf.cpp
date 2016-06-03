

//////////////////////////////////////////////////////////////////////////////////////////////

#include "file2buf.h"

#include "errstr.h"


int file2Buf(const char* szFNm, char* szBuf)
{
	int nLines = 0;
	char* szLin = new char[0x100];
	
	std::ifstream fil(szFNm, std::ios::in);
	if(fil.is_open())
	{
		while(!fil.eof())
		{
			fil.getline(szLin, 0x100);
			strcat_s(szBuf, 0x100, szLin);
			strcat_s(szBuf, 0x100, "\n");
			nLines++;
		}
		fil.close();
	}
	return (--nLines);
}

DWORD cxFile2BufA(LPCSTR szFNm, LPSTR szBuf)
{
	DWORD dwBytesRead = 0;
	
	HANDLE hFile = CreateFileA(szFNm, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE!=hFile)
	{
		DWORD dwBytesToRead = GetFileSize(hFile, NULL)+1;
		std::cout<<dwBytesToRead<<" bytes large is the input file...\r\n";
		if(dwBytesToRead>0x10)
		{
			if(ReadFile(hFile, szBuf, dwBytesToRead, &dwBytesRead, (LPOVERLAPPED)NULL))
			{
				CloseHandle(hFile);
				return dwBytesRead;
			}
			else
			{
				std::cout<<"cannot read the file \""<<szFNm<<"\"\r\n\t"<<getLastErrorAsString().c_str()<<"\r\n";
			}
		}
		else
		{
			std::cout<<"the file: \""<<szFNm<<"\" is empty\r\n";
		}
	}
	else
	{
		std::cout<<"unable to open the file: \""<<szFNm<<"\"\r\n";
	}
	CloseHandle(hFile);
	return dwBytesRead;
}

DWORD cxFile2BufA(LPSTR szBuf)
{
	LPOPENFILENAMEA 	lpofn = (LPOPENFILENAMEA)GlobalAlloc(GPTR, sizeof(OPENFILENAMEA));
	LPSTR				lpszFNm = (LPSTR)GlobalAlloc(GPTR, MAX_PATH*2);
	DWORD				dwBytesRead = 0;
	
	lpofn->lStructSize     = sizeof(OPENFILENAME);
	lpofn->hwndOwner       = NULL;
	lpofn->lpstrFile       = lpszFNm;
	lpofn->lpstrFilter     = "csv files (*.csv)\0*.csv\0";
	lpofn->nMaxFile        = MAX_PATH;
	lpofn->nFilterIndex    = 0;
	lpofn->lpstrFileTitle  = NULL;
	lpofn->nMaxFileTitle   = 0;
	lpofn->lpstrInitialDir = "";
	lpofn->lpstrDefExt     = "";
	lpofn->Flags           = OFN_EXPLORER | OFN_ENABLESIZING | OFN_HIDEREADONLY;
	lpofn->lpstrTitle      = "Select input data file.";
	
	GetOpenFileNameA(lpofn);
	lpszFNm = lpofn->lpstrFile;
	dwBytesRead = cxFile2BufA(lpszFNm, szBuf);
	GlobalFree(lpszFNm);
	GlobalFree(lpofn);
	
	return dwBytesRead;
}


//////////////////////////////////////////////////////////////////////////////////////////////

