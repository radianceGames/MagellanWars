#if !defined(__ARCHSPACE_LIB_CGI_H__)
#define __ARCHSPACE_LIB_CGI_H__

#include "common.h"
#include "util.h"
#include "net.h"

#define MT_TERMINATE_REQUEST        0x0000
#define MT_ERROR_SEND               0x0001
#define MT_URL_SEND                 0x8001
#define MT_METHOD_SEND              0x8003
#define MT_REFERER_SEND             0x8005
#define MT_COOKIE_SEND              0x8007
#define MT_ACCEPT_ENCODING_SEND     0x8009
#define MT_ACCEPT_LANGUAGE_SEND     0x800B
#define MT_USER_AGENT_SEND          0x800D
#define MT_HOST_NAME_SEND           0x800F
#define MT_CONNECTION_INFO_SEND     0x8011
#define MT_QUERY_SEND               0x8013
#define MT_GET_PAGE_REQUEST         0x8015
#define MT_HEADER_SEND              0x8101
#define MT_SET_COOKIE_SEND          0x8103
#define MT_CONTENT_SEND             0x8105

#define STRING_DATA_BLOCK_SIZE 		3980

#define SOCKET_CGI_SERVER			0x0002
#define SOCKET_CGI_CONNECTION		0x0101

class CPage;
class CPageStation;
class CConnection;

extern TZone gQueryZone;
/**
*/
class CQuery
{
	public:
		CQuery();
		~CQuery();

		void set(const char *aName, const char *aValue);
		char *get_name() { return mName.get_data(); }
		char *get_value() { return mValue.get_data(); }
		void set_name(const char *aName);
		void set_value(const char *aValue);
		void unescape_value();

	protected:
		CString 
			mName,
			mValue;

		RECYCLE(gQueryZone);	
};

extern TZone gQueryListZone;
/**
*/
class CQueryList: public CSortedList
{
	public:
		CQueryList();
		CQueryList(CQueryList& aQueryList);
		virtual ~CQueryList();

		void set(CString &aString, const char aSeperator='&');
		void set(const char *aString, const char aSeperator='&');


		const char* get(const char aSeperator = '&');

		void set_value(const char *aName, const char *aValue);	
		void set_value(const char *aName, const int aValue);	
		char *get_value(const char *aName);
		bool replace_value(const char *aName, const char *aValue);
	protected:
		CQuery* find_query(const char *aName);

		virtual bool free_item(TSomething aItem);

		virtual int compare(TSomething aItem1, TSomething aItem2) const;

		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "query list"; }
		RECYCLE(gQueryListZone);	
};

/**
*/
class CPage: public CBase
{
	friend class CHTMLStation;
	friend class CPageStation;
	public:
		CPage(); 
		virtual ~CPage();

		virtual const char *get_name() = 0;
		virtual bool handle(CConnection &aConnection) = 0;
		CString *get_html(CConnection &aConnection);

		static void set_html_station(CHTMLStation* aStation)
				{ mHTMLStation = aStation; }
		static CHTMLStation* get_html_station() 
				{ return mHTMLStation; }
	protected:
		CString
			mOutput;

		static CHTMLStation
			*mHTMLStation;
};

/**
*/
class CPageStation: public CSortedList
{
	public:
		CPageStation();
		virtual ~CPageStation();

		bool initialize(const char *aHTMLPath);

		CString *get_page(CConnection &aConnection);
	protected:
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1,
				TSomething aItem2) const;
		virtual int compare_key(TSomething aItem,
				TConstSomething aKey) const;

		virtual bool registration() = 0;

		CPage *find_page(const char *aName);

		virtual const char *debug_info() const { return "page station"; }
};

/**
*/
class CHTML
{
	public:
		CHTML();
		~CHTML();

		const char *get_name();
		const char *get_html();
		bool get(CString &aOutput, CQueryList *aConversions);

		bool load(const char *aPath, const char* aName);
	protected:
		CString 
			mName,
			mData;
};

/**
*/
class CHTMLStation: public CSortedList
{
	public:
		CHTMLStation();
		virtual ~CHTMLStation();

		bool load(const char *aPath);
		bool get_html(CString& aString, const char *aName, 
				CQueryList *aConversions); 
		bool get_html(CPage& aPage, const char *aName, 
				CQueryList *aConversions);
	protected:
		CHTML *find_html(const char *aName);
		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1,
				TSomething aItem2) const;
		virtual int compare_key(TSomething aItem,
				TConstSomething aKey) const;
		virtual const char *debug_info() const { return "html station"; }
	private:
		bool load_subdirectory(const char *aPath, 
				const char *aSubPath);

};

/**
	CGI를 전송하는 서버의 접속을 관리하는 클레스
*/
class CConnection: public CSocket
{
	friend class CPageStation;

	public:
		CConnection();
		virtual ~CConnection();

		virtual int read();
		virtual int write();

		virtual bool analysis();

		const char *get_uri() { return mURI.get_data(); }
		const char *get_method() { return mMethod.get_data(); }
		const char *get_referer() { return mReferer.get_data(); }
		const char *get_encoding() { return mEncoding.get_data(); }
		const char *get_language() { return mLanguage.get_data(); }
		const char *get_agent() { return mAgent.get_data(); }
		const char *get_host_name() { return mHostName.get_data(); }
		const char *get_connection() { return mConnection.get_data(); }
		const char *get_cookie() { return mCookie.get_data(); }
		const char *get_query() { return mQuery.get_data(); }

		static void set_page_station(CPageStation* aPageStation)
				{ mPageStation = aPageStation; }
		static const CPageStation* get_page_station()
				{ return mPageStation; }

		virtual int type() { return SOCKET_CGI_CONNECTION; }

	protected:
		CCollection
			mContent;

		virtual bool make_page();

		CString
			mURI,
			mMethod,
			mReferer,
			mEncoding,
			mLanguage,
			mAgent,
			mHostName,
			mConnection,
			mCookie,
			mQuery;

		static CPageStation
				*mPageStation;

		void send_terminate();
		void set_cookie(CString &aCookie);
		void set_content(CString &aPage);
		bool send_string_packet(int aType, CString &aString);
	private:
		bool get_uri(CMessage &Message);
		bool get_method(CMessage &Message);
		bool get_referer(CMessage &Message);
		bool get_cookie(CMessage &Message);
		bool get_encoding(CMessage &Message);
		bool get_language(CMessage &Message);
		bool get_agent(CMessage &Message);
		bool get_host_name(CMessage &Message);
		bool get_connection(CMessage &Message);
		bool get_query(CMessage &Message);
		bool get_page_request(CMessage &Message);
};

/**
	CGI를 제공하기 위한 루틴이 첨가된 서버 클레스
*/
class CCGIServer: public CLoopServer
{
	public:
		CCGIServer(int aLoopPerSec, 
				int aMaxConnection = FD_SETSIZE - 1);
		virtual ~CCGIServer();

		virtual int type() { return SOCKET_CGI_SERVER; }
	protected:
		virtual CSocket* new_connection(int aSocket);
		virtual bool free_item(TSomething aItem);

		virtual void handle_message();
};

/**
*/
class CFileHTML: public CHTML
{
	public:
		~CFileHTML() {}

		bool write(const char *aFilename, CQueryList *aConversion);
};

#endif
