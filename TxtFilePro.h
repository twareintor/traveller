
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string.h>
#include <windows.h>
#include <fstream>
#include <sstream>


typedef struct tagStar
{
	// Stores the parameter of the stars...
	unsigned int 	nRank;
	char			szBayer_Name[32-6+1];		// place for '\0'
	char			szName[50-32+1];			// place for '\0'
	unsigned int	nRA;
	unsigned int	nM;
	float			fB;
	float			fLGAL;
	float			fBGAL;
	char			szSpectralType[89-76+1];	// place for '\0'
	float			fMag;
	float			fA_Mag;
	float			fPrllx;
	float			fErr;
	float			fDLY;
	unsigned int	nRes;
	float			fRes;
	unsigned char	szRes[0x80];
	
} STAR, *PSTAR;

int getStarData(STAR* pStar);

int getFileName(char* szFNm);

int getL(char** pszOut, const char* szIn, unsigned int* nPos, unsigned int nCols);
int getT(char*** ppszOut, const char* szInpBuf, unsigned int* nPos, unsigned int nRows, unsigned int nCols);



DWORD cxGetLineCount(LPCSTR szBuf);

char* lTrim(char* szX);
char* rTrim(char* szX);
char* trim(char* szX);
























///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
