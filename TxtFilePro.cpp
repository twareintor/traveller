
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Title of this code: 

#include "TxtFilePro.h"

#include "file2buf.h"

#include "errstr.h"


int getStarData(STAR* pStar)
{
	std::ostringstream ossLog;
    unsigned int nPos[] = {6, 32, 50, 53, 56, 62, 69, 76, 89, 96, 102, 110, 117, 120, 0};
	char* szBuf = new char[1200*512];	// WARNING!!! THE SIZE! 
	int nRows = 0;
	
	if(cxFile2BufA("C:\\media\\StarCatalog.prn", szBuf)>0xff)
	{
		nRows = cxGetLineCount(szBuf);			// 	
	}
	else
	{
		delete[] szBuf;
		return 0;
	}
    std::cout<<nRows<<" Rows;"<<std::endl;
	// char* szTmp = szBuf;
    // std::cout<<szBuf<<std::endl;
    // std::cout<<"//////////////////////////////////////////////////////////////////////////////"<<std::endl;
    int nCols = sizeof(nPos)/sizeof(int)-1;
	// std::cout<<"nRows = "<<nRows<<std::endl;     
    // std::cout<<"nCols = "<<nCols<<std::endl;     
    char*** ppszOut = new char**[nRows];
    for(unsigned int i = 0; i<nRows; i++)
    {
        ppszOut[i] = new char*[nCols];
        for(unsigned int j = 0; j<nCols; j++)
        {
            ppszOut[i][j] = new char[nPos[j]-(j>0)*nPos[j-(j>0)]+1];	// 
            memset(ppszOut[i][j], 0, nPos[j]-(j>0)*nPos[j-(j>0)]+1);	// 
        }
    }
    getT(ppszOut, szBuf, nPos, nRows, nCols);
    // std::cout<<"//////////////////////////////////////////////////////////////////////////////"<<std::endl;
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

		// std::cout<<ppszOut[i][ 1]<<", ";	// DEBUG
		// std::cout<<ppszOut[i][13]<<", ";	// DEBUG
		// std::cout<<std::endl;

	}
	//////////
	std::ofstream fib("d:\\tmp\\out_deb.txt");
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
	
    std::cout<<"/////////////////////////////////////////"<<std::endl;
    delete[] ppszOut;
	// std::cout<<"after delete[] ppszOut"<<std::endl;
    // std::cout<<"##DEBUG3889##"<<std::endl;
    delete[] szBuf;
    // std::cout<<"##DEBUG3323##"<<std::endl;
    // std::cout<<ossLog.str().c_str()<<std::endl;
    // std::cout << "Hello, world!\n";
    return nRows;
}

int getT(char*** ppszOut, const char* szInpBuf, unsigned int* nPos, unsigned int nRows, unsigned int nCols)
{
    char* szBuf = new char[strlen(szInpBuf)]; 
    memcpy(szBuf, szInpBuf, strlen(szInpBuf)); 
    char* szTmp = szBuf;
    char* szNew = strchr(szTmp+1, '\n')+1;

    for(unsigned int i = 0; i<nRows; i++)
    {
        // std::cout<<"##DEBUG3111##"<<std::endl;
        char* szInp = new char[nPos[nCols-1]];      
        memset(szInp, 0, nPos[nCols-1]);			// empty the space reserved for the temporary string (for Windows)
        // std::cout<<"Columns: "<<nPos[nCols-1]<<std::endl;
        // std::cout<<szNew<<std::endl;
        // std::cout<<"##DE_LEN = "<<szNew-szTmp<<std::endl;
        memcpy(szInp, szTmp, szNew-szTmp-1);		// load the next line in the temporary string...
        // std::cout<<"##DEBUG3121##"<<std::endl;
        // std::cout<<szInp<<std::endl;
        char** pszOut = new char*[nCols]; 		// nCols columns maximum...
        for(unsigned int j = 0; j<nCols; j++)
        {
            if(nPos[j]>=nPos[j-(j>0)])
            {
                pszOut[j] = new char[nPos[j]-(j>0)*nPos[j-(j>0)]+1]; // reserve size according to the field size... 
				memset(pszOut[j], 0, nPos[j]-(j>0)*nPos[j-(j>0)]+1);
            }
        }
        getL(ppszOut[i], szInp, nPos, nCols);
        // ppszOut[i] = pszOut; // load the input table of char*-strings
        for(unsigned int j = 0; j<nCols; j++)
        {
            // std::cout<<"here!!!!!!! "<<j<<"\t"<<nPos[j]<<std::endl;
            if(nPos[j]>=nPos[j-(j>0)])
            {
                delete[] pszOut[j];
            }
        }
        delete[] pszOut;
        delete[] szInp;
        szTmp = szNew;
        szNew = strchr(szTmp+1, '\n')+1;
    }
    // std::cout<<"##DEBUG2122##"<<std::endl;
    delete[] szBuf;
    // std::cout<<"##DEBUG2722##"<<std::endl;
    return 0;
}


int getL(char** pszOut, const char* szIn, unsigned int* nPos, unsigned int nCols)
{
	// assumes pszOut is an array of arrays of '\0'-s
	// assumes szIn is a non-empty array of chars with a size at least equal to the biggest value
	// 		stored in the integer-array nPos (list of indexes of the fixed-widht fields)
	// assumes nCols = (number of elements of nPos) - 1
    int poo = 0; 		// position of the previous field 
    int pos = 1000; 	// catches the arguments, then pos-poo is the lenght of a field
    
    for(int i=0; i<nCols; i++)
    {
        pos = nPos[i]-1;	// transforming position in base "1" index
        if(pos>poo)
        {
	        // std::cout<<"X\t"<<pos<<"\t"<<pos-poo<<"\t";
	        memcpy(pszOut[i], szIn+poo, pos-poo);      
		}
        // std::cout<<"#######"<<pszOut[i];
        poo = pos;
        // std::cout<<std::endl;
    }
    return 0;
}


//////////////////////////////////////////////////////////////////////////

char* lTrim(char* szX)
{
	if(' '==szX[0]) while(' '==(++szX)[0]);
	return szX;
}

char* rTrim(char* szX)
{
	size_t i = strlen(szX);
	while(' '==szX[--i]) szX[i] = 0;
	return szX;
}

char* trim(char* szX)
{
	szX = lTrim(szX);
	szX = rTrim(szX);
	return szX;
}


DWORD cxGetLineCount(LPCSTR szBuf)
{
	// 
	DWORD nLines = 0;
	// std::cout<<"##DEBUG19004##"<<std::endl;
	// if(NULL==szBuf)
		// std::cout<<"is null"<<std::endl;
	char* szCR = strchr(const_cast<char*>(szBuf), '\n');	// 
	while(szCR)
	{
		szCR = strchr(szCR+1, '\n');
		nLines++;
	}
	return nLines;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


