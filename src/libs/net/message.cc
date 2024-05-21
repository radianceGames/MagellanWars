#include "common.h"
#include "util.h"
#include "message.h"

TZone gMessageZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CMessage),
	0,
	0,
	NULL,   
	"Zone CMessage"
};

unsigned short int 
	CMessage::mTransactionCount = 0,
	CMessage::mServerID = 0;

CMessage::CMessage()
{
	mSendOrReceive = true;

	mMessage = NULL;
	mReadPosition = 0;
	mCount = 0;
}

CMessage::~CMessage()
{
}

bool
CMessage::set_packet(void *aPacket, int aAction)
{
	TMessage 
		*Message;

	Message = (TMessage*)aPacket;

	if (aAction == MESSAGE_SEND)
	{
		Message->size = MESSAGE_HEADER_SIZE;
		Message->header.header_bytes[0] = Message->size % 256;
		Message->header.header_bytes[1] = Message->size / 256;
		Message->header.header_struct.type = 0;
		Message->header.header_struct.server = mServerID;
		Message->header.header_struct.counter = ++mTransactionCount;
	}
	if (Message->size > MAX_MESSAGE_SIZE) 
		return false;

//	system_log("set packet to %s", 
//				(aAction == MESSAGE_SEND) ? "send":"receive");

	mSendOrReceive = (aAction == MESSAGE_SEND) ? true:false;

	mMessage = (TMessage*)aPacket;

	if (aAction == MESSAGE_RECEIVE)
		return analysis();

	return true;
}

int 
CMessage::type(int aType)
{
	if (!mMessage) 
		return -1;

	if (aType < 0) 
		return mMessage->header.header_struct.type;

	if (!send_or_receive()) 
		return mMessage->header.header_struct.type;

	mMessage->header.header_struct.type = aType;
//	mMessage->header.header_struct.size = MESSAGE_HEADER_SIZE;
	
	return aType;
}

void*
CMessage::get_data()
{
	if (!mMessage) 
		return NULL;

	if (mMessage->size <= MESSAGE_HEADER_SIZE) 
		return NULL;

	return (void*)mMessage->data;
}

int
CMessage::get_data_size()
{
	if (!mMessage) 
		return -1;

	if (mMessage->size <= MESSAGE_HEADER_SIZE) 
		return 0;

	if (mCount == 0) 
		return 0;

	return mMessage->size-MESSAGE_HEADER_SIZE;
}

int
CMessage::get_packet_size()
{
	if (!mMessage) 
		return -1;

	return mMessage->size;
}

void*
CMessage::get_header()
{
	if (!mMessage) 
		return NULL;

	if (!send_or_receive())
		return (void*)&mMessage->header.header_struct;

	mMessage->header.header_struct.server = mServerID;
	mMessage->header.header_struct.counter = mTransactionCount;

	return (void*)&mMessage->header.header_struct;
}


bool 
CMessage::analysis()
{
	if (send_or_receive()) return false;

	if (!mMessage) return false;

	if ((mMessage->header.header_struct.type & 0xFF) == 0) return false;

	int 
		DataLength,
		Position = 0,
		ItemCount = 0;
	
	DataLength = mMessage->size - MESSAGE_HEADER_SIZE;

	while(Position < DataLength)
	{
		unsigned char 
			Type = mMessage->data[Position] >> 2;
		unsigned char
			ByteOfCount = mMessage->data[Position] & 0x03;
		short int 
			Count;

		switch(ByteOfCount)
		{
			case 0:
				return false;
			case 1:
				Count = mMessage->data[Position+1];
				break;
			case 2:
				Count = mMessage->data[Position+1]
						+mMessage->data[Position+2]*256;
				break;
			default:
				return false;
		}

	//	system_log("type: %d, byte of count: %d, count:%d",
	//			Type, ByteOfCount, Count);

		if (Type == MESSAGE_ITEM_LIST)
			Position += sizeof(Type)+ByteOfCount;
		else
			Position += sizeof(Type)+ByteOfCount+Count;

		ItemCount++;
	}

	mCount = ItemCount;

//	system_log("total item %d", ItemCount);

	return true;	
}

int 
CMessage::server_ID()
{
	if (!mMessage) return -1;

	return mMessage->header.header_struct.server;
}

int 
CMessage::host_server_ID(int aServerID)
{
	if (aServerID > 0) mServerID = aServerID;
	return mServerID;
}

int 
CMessage::transaction_count()
{
	if (!mMessage) return -1;

	return mMessage->header.header_struct.counter;
}

bool
CMessage::set_item(int aType, void* aData, int aDataSize)
{
	unsigned char 
		ItemSize[3],
		ItemHeader;
	unsigned char  
		ByteOfSize = 0;

	if (!mMessage) return false;

	if (!send_or_receive()) return false;

	if (mMessage->size < MESSAGE_HEADER_SIZE) 
		return false;

	if (aDataSize < 0) return false;

	if (aDataSize < 256) 
	{
		ItemSize[0] = (unsigned char)aDataSize;
		ByteOfSize = 1;
	} else if (aDataSize < 256*256)
	{
		ItemSize[0] = aDataSize%256;
		ItemSize[1] = aDataSize/256;
		ByteOfSize = 2;
	} else return false;

	if (get_data_size() + sizeof(ItemHeader) + ByteOfSize + aDataSize >
			MAX_MESSAGE_DATA_SIZE) return false;

	ItemHeader = (unsigned char)((aType & 0x3F) << 2) + ByteOfSize;

	if (aType == MESSAGE_ITEM_LIST)
	{
		memcpy(mMessage->data+get_data_size(),
				&ItemHeader, sizeof(ItemHeader));
		memcpy(mMessage->data+get_data_size()+sizeof(ItemHeader),
				ItemSize, ByteOfSize);

		mMessage->size += sizeof(ItemHeader) + ByteOfSize;
		mMessage->header.header_bytes[0] = mMessage->size % 256;
		mMessage->header.header_bytes[1] = mMessage->size / 256;
		mCount++;

		return true;
	}
	if (!aData) return false;

	memcpy(mMessage->data+get_data_size(), 
			&ItemHeader, sizeof(ItemHeader));
	memcpy(mMessage->data+get_data_size()+sizeof(ItemHeader),
			ItemSize, ByteOfSize);
	if (aDataSize)
		memcpy(mMessage->data+get_data_size()+sizeof(ItemHeader)+
				ByteOfSize, aData, aDataSize);

	mMessage->size += sizeof(ItemHeader) + ByteOfSize + aDataSize;
	mMessage->header.header_bytes[0] = mMessage->size % 256;
	mMessage->header.header_bytes[1] = mMessage->size / 256;
	mCount++;

	return true;
}

bool
CMessage::set_list(int aCount)
{
	return set_item(MESSAGE_ITEM_LIST, NULL, aCount);	
}

bool
CMessage::set_item(unsigned char aItem)
{
	return set_item(MESSAGE_ITEM_UINT1, 
			(void*)&aItem, sizeof(unsigned char));
}

bool
CMessage::set_item(unsigned short int aItem)
{
	return set_item(MESSAGE_ITEM_UINT2, 
			(void*)&aItem, sizeof(unsigned short int));
}

bool
CMessage::set_item(unsigned int aItem)
{
	return set_item(MESSAGE_ITEM_UINT4, 
			(void*)&aItem, sizeof(unsigned int));
}

bool
CMessage::set_item(unsigned long long aItem)
{
	return set_item(MESSAGE_ITEM_UINT8, 
			(void*)&aItem, sizeof(unsigned long long));
}

bool
CMessage::set_item(char aItem)
{
	return set_item(MESSAGE_ITEM_INT1, 
			(void*)&aItem, sizeof(char));
}

bool
CMessage::set_item(short int aItem)
{
	return set_item(MESSAGE_ITEM_INT2, 
			(void*)&aItem, sizeof(short int));
}

bool
CMessage::set_item(int aItem)
{
	return set_item(MESSAGE_ITEM_INT4, 
			(void*)&aItem, sizeof(int));
}

bool
CMessage::set_item(long long aItem)
{
	return set_item(MESSAGE_ITEM_INT8, 
			(void*)&aItem, sizeof(long long));
}

bool
CMessage::set_item(float aItem)
{
	return set_item(MESSAGE_ITEM_FLOAT4, 
			(void*)&aItem, sizeof(float));
}

bool
CMessage::set_item(double aItem)
{
	return set_item(MESSAGE_ITEM_FLOAT8, 
			(void*)&aItem, sizeof(double));
}

bool
CMessage::set_item(bool aItem)
{
	return set_item(MESSAGE_ITEM_BOOLEAN, 
			(void*)&aItem, sizeof(bool));
}

bool
CMessage::set_item(const unsigned char *aItem, int aSize)
{
	if (aSize < 0)
	{
		return set_item(MESSAGE_ITEM_ASCII, 
				(void*)aItem, strlen((char*)aItem));
	} else {
		return set_item(MESSAGE_ITEM_UINT1,
				(void*)aItem, aSize);
	}
}

bool 
CMessage::set_item(unsigned char *aItem, int aSize)
{
	return set_item((const unsigned char*)aItem, aSize);
}

bool 
CMessage::set_item(unsigned short int *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_UINT2, (void*)aItem, 
			aSize*sizeof(unsigned short int));
}

bool 
CMessage::set_item(unsigned int *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_UINT4, (void*)aItem, 
			aSize*sizeof(unsigned int));
}

bool 
CMessage::set_item(unsigned long long *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_UINT8, (void*)aItem, 
			aSize*sizeof(unsigned long long));
}

bool 
CMessage::set_item(const char *aItem, int aSize)
{
	if (aSize < 0)
	{
		return set_item(MESSAGE_ITEM_ASCII, 
				(void*)aItem, strlen(aItem));
	} else {
		return set_item(MESSAGE_ITEM_INT1,
				(void*)aItem, aSize);
	}
}

bool 
CMessage::set_item(char *aItem, int aSize)
{
	return set_item((const char*)aItem, aSize);
}

bool 
CMessage::set_item(short int *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_INT2, 
			(void*)aItem, aSize*sizeof(short int));
}

bool 
CMessage::set_item(int *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_INT4, 
			(void*)aItem, aSize*sizeof(int));
}

bool 
CMessage::set_item(long long *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_INT8, 
			(void*)aItem, aSize*sizeof(long long));
}

bool 
CMessage::set_item(float *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_FLOAT4, 
			(void*)aItem, aSize*sizeof(float));
}

bool 
CMessage::set_item(double *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_FLOAT8, 
			(void*)aItem, aSize*sizeof(double));
}

bool 
CMessage::set_item(bool *aItem, int aSize)
{
	if (aSize <= 0) return false;
	
	return set_item(MESSAGE_ITEM_BOOLEAN, 
			(void*)aItem, aSize*sizeof(bool));
}


bool
CMessage::get_start()
{
	mReadPosition = 0;
	if (mCount == 0) return false;
	return true;
}

int 
CMessage::get_item_type()
{
	if (!mMessage) 
		return -1;
	if (mReadPosition >= get_data_size())
		return -1;
	
	unsigned char 
		Type = mMessage->data[mReadPosition] >> 2;

	return Type;
}

bool 
CMessage::get_item(int &aType, void* &aData, int &aDataSize)
{
	if (!mMessage) 
		return false;
	if (mReadPosition >= get_data_size()) 
		return false;

	int 
		Position = mReadPosition;

	unsigned char 
		Type = mMessage->data[Position] >> 2;
	unsigned char
		ByteOfCount = mMessage->data[Position] & 0x03;
	int 
		Count;

	switch(ByteOfCount)
	{
		case 0:
			return false;
		case 1:
			Count = mMessage->data[Position+1];
			break;
		case 2:
			Count = mMessage->data[Position+1]+
					mMessage->data[Position+2]*256;
			break;
		default:
			return false;
	}

	aType = Type;
	aDataSize = Count;

	if (Type == MESSAGE_ITEM_LIST)
	{
		mReadPosition += sizeof(Type)+ByteOfCount;
		aData = NULL;	
	} else {
		mReadPosition += sizeof(Type)+ByteOfCount+Count;
		aData = &mMessage->data[Position+sizeof(Type)+ByteOfCount];
	}

	return true;
}

bool
CMessage::get_list(int &aCount)
{
	
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aCount = 0;

	if (!get_item(Type, Data, DataSize)) 
		return false;

	if (Type != MESSAGE_ITEM_LIST) 
		return false;
	if (DataSize <= 0) 
		return false;
	
	aCount = DataSize;

	return true;
}

bool
CMessage::get_item(unsigned char &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize)) 
		return false;

	if (Type != MESSAGE_ITEM_UINT1) 
		return false;
	if (DataSize != 1*sizeof(unsigned char)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(unsigned short int &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize)) 
		return false;

	if (Type != MESSAGE_ITEM_UINT2) 
		return false;
	if (DataSize != 1*sizeof(unsigned short int)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(unsigned int &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_UINT4) 
		return false;
	if (DataSize != 1*sizeof(unsigned int)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(unsigned long long &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_UINT8) 
		return false;
	if (DataSize != 1*sizeof(unsigned long long)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(char &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize)) 
		return false;

	if (Type != MESSAGE_ITEM_INT1) 
		return false;
	if (DataSize != 1*sizeof(char)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(short int &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize)) 
		return false;

	if (Type != MESSAGE_ITEM_INT2) 
		return false;
	if (DataSize != 1*sizeof(short int)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(int &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_INT4) 
		return false;
	if (DataSize != 1*sizeof(int)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(long long &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_INT8) 
		return false;
	if (DataSize != 1*sizeof(long long)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(float &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0.0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_FLOAT4) 
		return false;
	if (DataSize != 1*sizeof(float)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(double &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = 0.0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_FLOAT8) 
		return false;
	if (DataSize != 1*sizeof(double)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(bool &aItem)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = false;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_BOOLEAN) 
		return false;
	if (DataSize != 1*sizeof(bool)) 
		return false;
	if (Data == NULL) 
		return false;

	memcpy(&aItem, Data, DataSize);

	return true;
}

bool
CMessage::get_item(unsigned char* &aItem, int &aSize, bool aString)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = NULL;
	aSize = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (aString)
	{
		if (Type != MESSAGE_ITEM_ASCII) 
			return false;
		if (DataSize > 0 && DataSize%sizeof(unsigned char) != 0) 
			return false;
		if (Data == NULL) 
			return false;

		aItem = (unsigned char*)Data;
		aSize = DataSize / sizeof(unsigned char);
	} else {
		if (Type != MESSAGE_ITEM_UINT1) 
			return false;
		if (DataSize > 0 && DataSize%sizeof(unsigned char) != 0) 
			return false;
		if (Data == NULL) 
			return false;

		aItem = (unsigned char*)Data;
		aSize = DataSize / sizeof(unsigned char);
	}

	return true;
}

bool
CMessage::get_item(unsigned short int* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_UINT2) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(unsigned short int) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (unsigned short int*)Data;
	aSize = DataSize / sizeof(unsigned short int);

	return true;
}

bool
CMessage::get_item(unsigned int* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_UINT4) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(unsigned int) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (unsigned int*)Data;
	aSize = DataSize / sizeof(unsigned int);

	return true;
}

bool
CMessage::get_item(unsigned long long* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_UINT8) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(unsigned long long) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (unsigned long long*)Data;
	aSize = DataSize / sizeof(unsigned long long);

	return true;
}

bool
CMessage::get_item(char* &aItem, int &aSize, bool aString)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aItem = NULL;
	aSize = 0;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (aString)
	{
		if (Type != MESSAGE_ITEM_ASCII) 
			return false;
		if (DataSize > 0 && DataSize%sizeof(char) != 0) 
			return false;
		if (Data == NULL) 
			return false;

		aItem = (char*)Data;
		aSize = DataSize / sizeof(char);
	} else {
		if (Type != MESSAGE_ITEM_INT1) 
			return false;
		if (DataSize > 0 && DataSize%sizeof(char) != 0) 
			return false;
		if (Data == NULL) 
			return false;

		aItem = (char*)Data;
		aSize = DataSize / sizeof(char);
	}

	return true;
}

bool
CMessage::get_item(short int* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_INT2) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(short int) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (short int*)Data;
	aSize = DataSize / sizeof(short int);

	return true;
}

bool
CMessage::get_item(int* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_INT4) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(int) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (int*)Data;
	aSize = DataSize / sizeof(int);

	return true;
}

bool
CMessage::get_item(long long* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_INT8) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(long long) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (long long*)Data;
	aSize = DataSize / sizeof(long long);

	return true;
}

bool
CMessage::get_item(float* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_FLOAT4) 
		return false;
	if (DataSize%sizeof(float) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (float*)Data;
	aSize = DataSize / sizeof(float);

	return true;
}

bool
CMessage::get_item(double* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_FLOAT8) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(double) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (double*)Data;
	aSize = DataSize / sizeof(double);

	return true;
}

bool
CMessage::get_item(bool* &aItem, int &aSize)
{
	int 
		Type;
	void 
		*Data;
	int 
		DataSize;

	aSize = 0;
	aItem = NULL;

	if (!get_item(Type, Data, DataSize))
		return false;

	if (Type != MESSAGE_ITEM_BOOLEAN) 
		return false;
	if (DataSize > 0 && DataSize%sizeof(bool) != 0) 
		return false;
	if (Data == NULL) 
		return false;

	aItem = (bool*)Data;
	aSize = DataSize / sizeof(bool);

	return true;
}
