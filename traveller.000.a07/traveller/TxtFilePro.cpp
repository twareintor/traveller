
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Title of this code: 

#include "TxtFilePro.h"
#include "file2buf.h"
#include "errstr.h"



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


