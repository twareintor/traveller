
/************************************************************************************************************/

#include "CXSky.h"


int getStarData(STAR* pStar)
{
	std::ostringstream ossLog;
	unsigned int nPos[] = {6, 32, 50, 53, 56, 62, 69, 76, 89, 96, 102, 110, 117, 120, 0};
	char* szBuf = new char[1200*512];	/* WARNING!!! THE SIZE! */
	int nRows = 0;

	if(cxFile2BufA("../../../../media/StarCatalog.prn", szBuf)>0xff)
	{
		nRows = cxGetLineCount(szBuf);			/* */ 	
	}
	else
	{
		delete[] szBuf;
		return 0;
	}
	std::cout<<nRows<<" Rows;"<<std::endl;
	int nCols = sizeof(nPos)/sizeof(int)-1;
	char*** ppszOut = new char**[nRows];
	for(unsigned int i = 0; i<nRows; i++)
	{
		ppszOut[i] = new char*[nCols];
		for(unsigned int j = 0; j<nCols; j++)
		{
			ppszOut[i][j] = new char[nPos[j]-(j>0)*nPos[j-(j>0)]+1];	/***/ 
			memset(ppszOut[i][j], 0, nPos[j]-(j>0)*nPos[j-(j>0)]+1);	/***/ 
		}
	}
	getT(ppszOut, szBuf, nPos, nRows, nCols);
	for(unsigned int i = 1; i<nRows; i++)		// nRows - 1 stars, because the first row is the header of the table...
	{
		for(unsigned int j = 0; j<nCols-1; j++)
		{
			ppszOut[i][j] = trim(ppszOut[i][j]);
		}
		(pStar+i-1)->nRank = 					atoi(ppszOut[i][ 0]);
		strcpy_s((pStar+i-1)->szBayer_Name,		ppszOut[i][ 1]);
		strcpy_s((pStar+i-1)->szName,			ppszOut[i][ 2]);
		(pStar+i-1)->nRA = 						atoi(ppszOut[i][ 3]);
		(pStar+i-1)->nM = 						atoi(ppszOut[i][ 4]);
		(pStar+i-1)->fB = 						atof(ppszOut[i][ 5]);
		(pStar+i-1)->fLGAL = 					atof(ppszOut[i][ 6]);
		(pStar+i-1)->fBGAL = 					atof(ppszOut[i][ 7]);			
		strcpy_s((pStar+i-1)->szSpectralType,	ppszOut[i][ 8]);				
		(pStar+i-1)->fMag = 					atof(ppszOut[i][ 9]);			
		(pStar+i-1)->fA_Mag = 					atof(ppszOut[i][10]);			
		(pStar+i-1)->fPrllx = 					atof(ppszOut[i][11]);			
		(pStar+i-1)->fErr = 					atof(ppszOut[i][12]);			
		(pStar+i-1)->fDLY = 					atof(ppszOut[i][13]);			

	}
	/***/
	std::ofstream fib("out_deb.txt");
	if(fib.is_open())
	{
		for(int i=1; i<=nRows; i++)
		{
			fib<<
				(pStar+i)->nRank				<<";"<<
				(pStar+i)->szBayer_Name			<<";"<<
				(pStar+i)->szName				<<";"<<
				(pStar+i)->nRA					<<";"<<
				(pStar+i)->nM					<<";"<<
				(pStar+i)->fB					<<";"<<
				(pStar+i)->fLGAL				<<";"<<
				(pStar+i)->fBGAL				<<";"<<
				(pStar+i)->szSpectralType		<<";"<<
				(pStar+i)->fMag					<<";"<<
				(pStar+i)->fA_Mag				<<";"<<
				(pStar+i)->fPrllx				<<";"<<
				(pStar+i)->fErr					<<";"<<
				(pStar+i)->fDLY					<<";"<<
				std::endl;
		}
	}
	else
	{
		std::cout<<"cannot open test file for parameter output; error: "<<getLastErrorAsString().c_str()<<std::endl;
	}
	fib.close();
	delete[] ppszOut;
	delete[] szBuf;
	return nRows;
}
















/******************************************************************************************************/


