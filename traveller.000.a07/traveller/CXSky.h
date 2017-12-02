
/******************************************************************************************************************/

#include "TxtFilePro.h"
#include "file2buf.h"
#include "errstr.h"

typedef struct tagStar
{
	/* Stores the parameter of the stars... */
	unsigned int 	nRank;
	char			szBayer_Name[32-6+1];		/* place for '\0' */
	char			szName[50-32+1];			/* place for '\0' */
	unsigned int	nRA;
	unsigned int	nM;
	float			fB;
	float			fLGAL;
	float			fBGAL;
	char			szSpectralType[89-76+1];	/* place for '\0' */
	float			fMag;
	float			fA_Mag;
	float			fPrllx;
	float			fErr;
	float			fDLY;
	unsigned int	nRes;
	float			fRes;
	unsigned char	szRes[0x80];

} STAR, *PSTAR;

/** Function that gets the star data from a designated text file into a structure containing all data of the star */
int getStarData(STAR* pStar);














////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

