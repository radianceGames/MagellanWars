#if !defined(__ARCHSPACE_LIB_MESSAGE_H__)
#define __ARCHSPACE_LIB_MESSAGE_H__

/*
	Header structure
		
		SIZE: 2 Bytes
		TYPE: 2 Bytes
		SERVER_ID: 2 Bytes
		COUNTER: 2 Bytes

	Total 8 Bytes
*/

#define MESSAGE_HEADER_SIZE 8
#define MAX_MESSAGE_DATA_SIZE 3992
#define MAX_MESSAGE_SIZE  \
			MESSAGE_HEADER_SIZE + MAX_MESSAGE_DATA_SIZE

#define MESSAGE_ITEM_LIST		000
#define MESSAGE_ITEM_ASCII		001
#define MESSAGE_ITEM_UINT8		010
#define MESSAGE_ITEM_UINT4		014
#define MESSAGE_ITEM_UINT2		012
#define MESSAGE_ITEM_UINT1		011
#define MESSAGE_ITEM_INT8		020
#define MESSAGE_ITEM_INT4		024
#define MESSAGE_ITEM_INT2		022
#define MESSAGE_ITEM_INT1		021
#define MESSAGE_ITEM_FLOAT8		030
#define MESSAGE_ITEM_FLOAT4		034
#define MESSAGE_ITEM_BOOLEAN	040

typedef struct 
{
	unsigned short int 
		size,
		type, 
		server,
		counter;
} TMessageHeader;

typedef struct 
{
	union
	{
		TMessageHeader
			header_struct;
		unsigned char 
			header_bytes[MESSAGE_HEADER_SIZE];
	} header;

	unsigned char 
		data[MAX_MESSAGE_DATA_SIZE];
	unsigned short int
		size;
} TMessage;
/* 
	서버간 프로토콜 정의에 따르는 패킷 생성 및 해석 루틴
*/

#define MESSAGE_SEND 			1
#define MESSAGE_RECEIVE 		2

extern TZone gMessageZone;

/**
*/
class CMessage
{
	public:
		CMessage();
		~CMessage();

		bool set_packet(void *aPacket, int aAction);
		int type(int aType=-1);		

		void *get_data();

		void *get_header();
		int get_data_size();
		int get_packet_size();

		int server_ID();
		static int host_server_ID(int aServerID = -1);

		int transaction_count();
		int host_transaction_count() { return mTransactionCount; }
		int item_count() { return mCount; }

		bool send_or_receive() { return mSendOrReceive; }

		bool set_list(int aCount);

		bool set_item(unsigned char aItem);
		bool set_item(unsigned short int aItem);
		bool set_item(unsigned int aItem);
		bool set_item(unsigned long long aItem);

		bool set_item(char aItem);
		bool set_item(short int aItem);
		bool set_item(int aItem);
		bool set_item(long long aItem);
		
		bool set_item(float aItem);
		bool set_item(double aItem);

		bool set_item(bool aItem);


		bool set_item(const unsigned char *aItem, int aSize = -1);
		bool set_item(unsigned char *aItem, int aSize = -1);
		bool set_item(unsigned short int *aItem, int aSize);
		bool set_item(unsigned int *aItem, int aSize);
		bool set_item(unsigned long long *aItem, int aSize);

		bool set_item(const char *aItem, int aSize = -1);
		bool set_item(char *aItem, int aSize = -1);
		bool set_item(short int *aItem, int aSize);
		bool set_item(int *aItem, int aSize);
		bool set_item(long long *aItem, int aSize);
		
		bool set_item(float *aItem, int aSize);
		bool set_item(double *aItem, int aSize);

		bool set_item(bool *aItem, int aSize);

		bool get_start();
		int  get_item_type();

		bool get_list(int &aCount);
		bool get_item(unsigned char &aItem);
		bool get_item(unsigned short int &aItem);
		bool get_item(unsigned int &aItem);
		bool get_item(unsigned long long &aItem);

		bool get_item(char &aItem);
		bool get_item(short int &aItem);
		bool get_item(int &aItem);
		bool get_item(long long &aItem);

		bool get_item(float &aItem);
		bool get_item(double &aItem);

		bool get_item(bool &aItem);

		bool get_item(unsigned char* &aItem, int &aSize, 
				bool aString=false);
		bool get_item(unsigned short int* &aItem, int &aSize);
		bool get_item(unsigned int* &aItem, int &aSize);
		bool get_item(unsigned long long*& aItem, int &aSize);

		bool get_item(char* &aItem, int &aSize, 
				bool aString=false);
		bool get_item(short int* &aItem, int &aSize);
		bool get_item(int* &aItem, int &aSize);
		bool get_item(long long* &aItem, int &aSize);

		bool get_item(float* &aItem, int &aSize);
		bool get_item(double* &aItem, int &aSize);

		bool get_item(bool* &aItem, int &aSize);

	protected:
		bool set_item(int aType, void* aData, int aDataSize);
		bool get_item(int &aType, void *&aData, int &aDataSize);

		bool
			mSendOrReceive;

		TMessage
			*mMessage;

		int
			mCount;

		static unsigned short int 
			mServerID;
		static unsigned short int 
			mTransactionCount;

		int 
			mReadPosition;

		bool analysis();

	RECYCLE(gMessageZone);
};

#endif
