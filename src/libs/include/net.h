#if !defined(__ARCHSPACE_LIB_NET_H__)
#define __ARCHSPACE_LIB_NET_H__

#include "common.h"
#include "util.h"
#include "message.h"

#define MAX_PACKET_SIZE		4096

#define SOCKET_TYPE_SERVER				0x0000
#define SOCKET_TYPE_LOOP_SERVER			0x0001
#define SOCKET_TYPE_FIFO				0x1000
#define SOCKET_TYPE_CLIENT				0x0100

extern TZone gPacketZone;
/**
*/
class CPacket : public CNode
{
	public:
		CPacket();
		CPacket(CPacket &aPacket);
		~CPacket();
		bool set(void *aData, int aSize);
		void *get() { return (void*)&mPacket; }
		int count() { return mPacket.size; }
		int sent(int aSent = -1);
		CPacket* clone();

	protected:
		TMessage
			mPacket;
		int
			mSent;

	RECYCLE(gPacketZone);
};  

// ----------------------------------------------------------------
/**
	소켓을 다루는 모든 클레스의 공통적인 내용을 담고있는 클레스
*/
class CSocket: virtual public CBase
{
	public:
		CSocket();
		virtual ~CSocket();

		void close();

		int get() 
			{ return mSocket; }
		void set(int aSocket)
			{ mSocket = aSocket; }

		virtual int read() = 0;
		virtual int write() = 0;

		virtual int type() = 0;

		void nonblock(int aSocket = -1);
	protected:
		CCollection
				mInput,
				mOutput;

		int read_packet(CPacket *aPacket);
		int write_packet(CPacket *aPacket);

		int 
			mSocket;
};

#define FIFO_RECEIVE        1
#define FIFO_SEND           2

class CFIFO: public CSocket
{
    public:
		CFIFO();
		virtual ~CFIFO();

		int open(const char *aFIFOName, int aMode=FIFO_RECEIVE);

		virtual int type() { return SOCKET_TYPE_FIFO; }
};   

class CClient:public CSocket
{
	public:
		CClient();
		virtual ~CClient();
		int open(const char *aServerName, int aPort);

		virtual int type() { return SOCKET_TYPE_CLIENT; }
};

// -----------------------------------------------------------------

// -------------------------------------------------------------------
/**
	서버를 열고 사용자의 접속을 받아들이면서 관리하는 
	클레스 
*/
class CServer: public CSocket, protected CList
{
	public:
		CServer(int aMaxConnection = FD_SETSIZE - 1);
		virtual ~CServer();

		virtual int read() { return 0; }
		virtual int write() { return 0; }
		virtual int type() { return SOCKET_TYPE_SERVER; }

		virtual const char *debug_info() const { return "server"; }
	protected:
		int open(int aPort);
		virtual int prepare();
		void select();
		int accept();

		virtual CSocket* new_connection(int aSocket) = 0;

		fd_set 
			mReads,
			mWrites,
			mExceptions;

		int 
			mMaxDescription,
			mMaxConnection;

		virtual bool free_item(TSomething aItem);

		virtual void handle_exception() = 0;
		virtual void handle_input() = 0;
		virtual void handle_output() = 0;
};

// ------------------------------------------------------------------

class CLoopServer: public CServer
{
	public:
		CLoopServer(int aLoopPerSecond, 
				int aMaxConnection = FD_SETSIZE-1);
		virtual ~CLoopServer();

		void wait_cycle();

		static int get_loop_per_second() { return mLoopPerSecond; }

		virtual int type() { return SOCKET_TYPE_LOOP_SERVER; }
	protected:
		static int 
			mLoopPerSecond;

		void get_remain_time();

		virtual void handle_exception();
		virtual void handle_input();
		virtual void handle_output();

	private:
		struct timeval
			mLastTime,
			mRemainTime;
};

class CIP
{
	public:
		CIP() { mName = ""; mAddress = 0; }
		virtual ~CIP() {}

		unsigned long get_address() { return mAddress; }
		const char *get_name() { return (char*)mName; }

		void set_name(const char *aName)
				{ mName = aName; }
		void set_address(unsigned long aAddress) 
				{ mAddress = aAddress; }
	protected:
		CString mName;
		unsigned long mAddress;	

		virtual const char *debug_info() const { return "ip list"; }
};

class CIPList: public CSortedList
{
	public:
		CIPList();
		virtual ~CIPList();

		bool add_ip(const char *aIPAddress);
		bool remove_ip(unsigned long aIPAddress);

		CIP *get_by_address(char *aIPAddress);			// Added by YOSHIKI(2001/04/21)

		bool load(const char* aFilename);
		bool save();

		bool has(unsigned long aIPAddress);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "ip list"; }
		CString 
			mFilename;
		bool
			mDirty;
};

char* harvestPHPSessionVar(char* aVariableName, char* aPath);
void writePHPSessionVar(char* aVariableName, char* aVariableValue, char* aVariableType, char* aPath);

#endif
