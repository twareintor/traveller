
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string.h>
#include <windows.h>
#include <fstream>
#include <sstream>



int getFileName(char* szFNm);

int getL(char** pszOut, const char* szIn, unsigned int* nPos, unsigned int nCols);
int getT(char*** ppszOut, const char* szInpBuf, unsigned int* nPos, unsigned int nRows, unsigned int nCols);

DWORD cxGetLineCount(LPCSTR szBuf);

char* lTrim(char* szX);
char* rTrim(char* szX);
char* trim(char* szX);
























///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
