#include "bsdport.h"
#include "key.h"

TZone gCryptConnectionZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCryptConnection),
	0,
	0,
	NULL,   
	"Zone CCryptConnection"
};

char
	CCryptConnection::mCurrentKey[10] = "",
	CCryptConnection::mOldKey[10] = "",
	CCryptConnection::mFutureKey[10] = "";

#define DES_SALT			4096
#define DES_COUNT			8

CCryptConnection::CCryptConnection(): CConnection()
{
}

CCryptConnection::~CCryptConnection()
{
}

void
CCryptConnection::set_key(const char *aCurrentKey,
  	      const char *aOldKey, const char *aFutureKey)
{
	if (strlen(aCurrentKey) < 10)
		strcpy(mCurrentKey, aCurrentKey);
	if (strlen(aOldKey) < 10)
		strcpy(mOldKey, aOldKey);
	if (strlen(aFutureKey) < 10)
		strcpy(mFutureKey, aFutureKey);
}

void
CCryptConnection::encrypt(int aKeyType, 
			const char *aSource, CString &aDest)
{
	int
		Length,
		i;
	unsigned char Temp[5000];
	char Dest[10240];

	Length = strlen(aSource)+1;

	aDest.clear();

	switch(aKeyType)
	{
		case CRYPT_CURRENT_KEY:
			mydes_setkey(mCurrentKey);
			break;
		case CRYPT_OLD_KEY:
			mydes_setkey(mOldKey);
			break;
		case CRYPT_FUTURE_KEY:
			mydes_setkey(mFutureKey);
			break;
		default:
			return;
	}

//	system_log("encrypt source (%s)", aSource);

	for(i=0; i<Length; i+=8)
		mydes_cipher(aSource+i, (char*)Temp+i, DES_SALT, DES_COUNT);
	Length = i;

	for(i=0; i<Length; i++)
	{
		Dest[2*i] = Temp[i]/16 + 'A';
		Dest[2*i+1] = Temp[i]%16 + 'A';
	}
	Dest[2*i] = '\0';
	aDest = Dest;
}

void
CCryptConnection::decrypt(int aKeyType, 
			const char *aSource, CString &aDest)
{
	int
		Length,
		i;

	char Temp[5000];
	char Dest[10240];

	Length = strlen(aSource);

	switch(aKeyType)
	{
		case CRYPT_CURRENT_KEY:
			mydes_setkey(mCurrentKey);
			break;
		case CRYPT_OLD_KEY:
			mydes_setkey(mOldKey);
			break;
		case CRYPT_FUTURE_KEY:
			mydes_setkey(mFutureKey);
			break;
		default:
			return;
	}

//	system_log("decrypt source (%s)", aSource);

	memset((char*)Temp, 0, sizeof(Temp));
	for(i=0; aSource[i]; i+=2)
		Temp[i/2] = (aSource[i]-'A')*16+(aSource[i+1]-'A');
	Temp[i/2] = 0;

	Length /= 2;
	for(i=0; i<Length; i+=8)
		mydes_cipher(Temp+i, Dest+i, DES_SALT, -DES_COUNT);
	Dest[i] = 0;

	aDest = Dest;

//	system_log("decrypt ret (%s)", (char*)aDest );
}

