#if !defined(__ARCHSPACE_KEY_H__)
#define __ARCHSPACE_KEY_H__

#include "net.h"
#include "cgi.h"

extern TZone gKeyServerZone;
extern TZone gCryptConnectionZone;

#define MAX_KEY_RECEIVE_BUFFER 1000
typedef struct
{
	char
		name[256];
	int
		port;
	int
		pipe[2];
} SKeyServerInfo;

/**
*/
class CKeyServer: public CClient
{
	public:
		CKeyServer();
		virtual ~CKeyServer();

		virtual int read();
		virtual int write();

		const char* get_buffer();
	protected:
		char mBuffer[256];

	RECYCLE(gKeyServerZone);
}; 

#define CRYPT_CURRENT_KEY		1
#define CRYPT_OLD_KEY			2
#define CRYPT_FUTURE_KEY		3

/**
*/
class CCryptConnection: public CConnection
{
	public:
		CCryptConnection();
		virtual ~CCryptConnection();

		static void set_key(const char *aCurrentKey, 
				const char *aOldKey, const char* aFutureKey);
	protected:
		void encrypt(int aKeyType, const char *aSource,
								CString &aDest);
		void decrypt(int aKeyType, const char *aSource,
								CString &aDest);
		static char 
				mCurrentKey[10],
				mOldKey[10],
				mFutureKey[10];

	RECYCLE(gCryptConnectionZone);
};

#endif
