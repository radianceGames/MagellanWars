#if !defined(__ARCHSPACE_LIB_UTIL_H__)
#define __ARCHSPACE_LIB_UTIL_H__

#include <ctime>
#include <pth.h>
#include "common.h"
#include <cassert>
#include <sys/types.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include "mysql.h"

class CString;
// --------------------------------------------------------------
extern char SYSTEM_LOGFILE[];

template <class tType, tType tMax, tType tMin>
tType safe_change_type(tType aValue, tType &tTarget) //returns the excess
{
	tType aTemp = tTarget;
	aTemp += aValue;
	if ((aValue < 0)&&((aTemp > tTarget)||(aTemp < tMin)))
	{
		aTemp = tTarget - tMin + aValue;
		tTarget = tMin;
		return aTemp;
	}
	else if ((aValue > 0)&&((aTemp < tTarget)||(aTemp > tMax)))
	{
		aTemp = tTarget - tMax + aValue;
		tTarget = tMax;
		return aTemp;
	}
	tTarget = aTemp;
	return 0;
}

template <class tType, tType tMax> //assumes min is zero
tType safe_change_type(tType aValue, tType &tTarget) //returns the excess
{
	tType aTemp = tTarget;
	aTemp += aValue;
	if ((aValue < 0)&&((aTemp > tTarget)||(aTemp < 0)))
	{
		aTemp = tTarget + aValue;
		tTarget = 0;
		return aTemp;
	}
	else if ((aValue > 0)&&((aTemp < tTarget)||(aTemp > tMax)))
	{
		aTemp = tTarget - tMax + aValue;
		tTarget = tMax;
		return aTemp;
	}
	tTarget = aTemp;
	return 0;
}

template <class tType, tType tMax, tType tMin>
void safe_fast_change_type(tType aValue, tType &tTarget)
{
	tType aTemp = tTarget;
	aTemp += aValue;
	if ((aValue < 0)&&((aTemp > tTarget)||(aTemp < tMin)))
	{
		tTarget = tMin;
	}
	else if ((aValue > 0)&&((aTemp < tTarget)||(aTemp > tMax)))
	{
		tTarget = tMax;
	}
	else
	{
		tTarget = aTemp;
	}
}

template <class tType, tType tMax> //assumes minimuim is zero for optimization
void safe_fast_change_type(tType aValue, tType &tTarget)
{
	tTarget += aValue;
	if (aValue > 0)
	{
		if ((tTarget> tMax)||(tTarget < 0))
		{
			tTarget = tMax;
		}
	}
	else
	{
		if (tTarget < 0)
		{
			tTarget = 0;
		}
	}
}

template <class tType, tType tMax, tType tMin>
void safe_set_type(tType aValue, tType &tTarget)
{
	if (aValue > tMax)
	{
		tTarget = tMax;
	}
	else if (aValue < tMin)
	{
		tTarget = tMin;
	}
	else
	{
		tTarget = aValue;
	}
}

template <class tType, tType tMax> //assumes min is 0
void safe_set_type(tType aValue, tType &tTarget)
{
	if (aValue > tMax)
	{
		tTarget = tMax;
	}
	else if (aValue < 0)
	{
		tTarget = 0;
	}
	else
	{
		tTarget = aValue;
	}
}

char hex2dec(char *aHex);
void unescape(char *aString);
char *string_new(const char *aString);
char *split_word(char *aOut, char *aIn, char aStop);
void system_log(const char *aFormat, ...);
void system_log_for_pstat(const char *aFormat, ...);
int number( int i );
int dice( int n, int m );
char *dec2unit(int aNumber);
char *dec2unit_for_unsigned_int(unsigned int aNumber);
char *remove_comma(const char *aNumberString);
int as_atoi(const char *aNumberString);
double as_atof(const char *aNumberString);
char *timetostring(time_t aTime);
char *integer_with_sign(int aNumber);					// Added by YOSHIKI(2001/01/17)
bool as_isspace(int aChar);								// Added by YOSHIKI(2001/02/02)
int as_calc_date_index( int aNumber );		// by Nara 2001-2-4
int as_calc_date_index( time_t t, int aNumber );		// by Nara 2001-2-4
bool is_valid_name(const char *aString);
bool is_valid_slogan(const char *aString);
char *unsigned_int_to_char(unsigned int aNumber);
void add_dot_for_sendmail(char *aString, CString *aResultString);
pth_time_t get_pth_nap_time(int aNapTime);
char *plural(int aNumber);

void *memory_allocation(TZone *aZone, size_t aSize);
void memory_free(void *aObject);
void *recycle_allocation(TZone *aZone, size_t aSize);
void recycle_free(void *aObject);


void zone_info(TZone *aZone);

extern TZone gStringZone;
extern TZone gNodeZone;
extern TZone gCollectionZone;
extern TZone gIniFileZone;
extern TZone gCommandSetZone;
extern TZone gMySQLZone;
extern TZone gScriptZone;

// --------------------------------------------------------------

typedef void* TSomething;
typedef const void* TConstSomething;

/**
*/
class CList: public CBase
{
	public:
		CList(int aLimit = 16, int aTreshold = 8);
		virtual ~CList();

		inline TSomething get(int aIndex);

		inline void at(int aIndex, TSomething aItem);

		void set_length(int aLength);

		inline int length() const;

		inline int limit() const;

		inline void exchange(int aIndex1, int aIndex2);

		int find(TSomething aWhich);

		bool remove(int aIndex);
		bool remove_without_free(int aIndex);

		bool replace(int aIndex, TSomething aWhat);

		void remove_all();
		void remove_without_free_all();


		bool insert(int aIndex, TSomething aItem);

		inline int push(TSomething aWhat);
		inline TSomething pop();

	protected:
		int
			mCount;

		int
			mLimit;

		int
			mThreshold;

		TSomething
			*mRoot;

		size_t
			mSize;

//		static TSomething
//			mTrouble;

		virtual bool free_item(TSomething aItem)=0;
		virtual const char *debug_info() const { return "list"; }

#ifdef MEMORY_DEBUG
		void check_border( void *aPoint, char *msg = "" ) const;
		void check_trouble( TSomething aCandi, char *msg = "" ) const;
#endif
		void my_memset( void *b, int c, size_t len );
		void my_memmove( void *dst, void *src, size_t len );
};

inline TSomething
CList::get(int aIndex)
{
#ifdef MEMORY_DEBUG
	check_border( mRoot+aIndex, "get" );
#endif
	if (aIndex < 0) return NULL;
	if (aIndex >= mCount) return NULL;

	return mRoot[aIndex];
}

inline void
CList::at(int aIndex, TSomething aItem)
{
	if (aIndex < 0) return;
	if (aIndex >= mCount)
		set_length(aIndex+1);
#ifdef MEMORY_DEBUG
	check_border( mRoot+aIndex, "at" );
#endif
	mRoot[aIndex] = aItem;
}

inline int
CList::limit() const
{
	return (mLimit);
}

inline int
CList::length() const
{
	return (mCount);
}

inline void
CList::exchange(int aIndex1, int aIndex2)
{
#ifdef MEMORY_DEBUG
	check_border( mRoot+aIndex1, "ex1" );
	check_border( mRoot+aIndex2, "ex2" );
#endif
	TSomething
		Temp = mRoot[aIndex1];
	mRoot[aIndex1] = mRoot[aIndex2];
	mRoot[aIndex2] = Temp;
}

inline int
CList::push(TSomething aWhat)
{
	set_length(mCount+1);
#ifdef MEMORY_DEBUG
	check_border( mRoot+mCount-1, "psh" );
#endif
	mRoot[mCount-1] = aWhat;
	return (mCount-1);
}

inline TSomething
CList::pop()
{
	TSomething
		Ret = mRoot[mCount-1];
	set_length(mCount-1);
	return Ret;
}

// --------------------------------------------------------------
/**
*/
class CSortedList: public CList
{
	public:
		CSortedList(int aLimit = 8, int aThreshold = 16);
		virtual ~CSortedList();


		int find_key(TConstSomething aKey) const;

		int find_sorted_key(TConstSomething aKey) const;

		void quick_sort(int aLeft, int aRight);

		inline void quick_sort();

		int insert_sorted(TSomething aItem,
				int *aEqual = NULL);
	protected:

		virtual int compare(TSomething aItem1,
				TSomething aItem2) const = 0;

		virtual int compare_key(TSomething aItem,
				TConstSomething aKey) const = 0;
};

inline void
CSortedList::quick_sort()
{
	if (mCount > 0)
		quick_sort(0, mCount-1);
}

/**
*/
class CString
{
	public:
		static void print_debug();
		static unsigned int mClassCount;
		static unsigned int mMemoryUsage;
	protected:
		char
			*mData;

		size_t
			mSize;

		size_t
			mMaxSize;
	public:
		CString();
		CString(size_t aLength);
		CString(const CString& aCopy);
		CString(const char *aCopy);

		~CString();

		inline CString clone() const;

		void set_size(size_t aNewSize);

		void free();

		inline void reclaim();

		inline CString& clear();

		inline char* get_data() const;

		inline size_t length() const;

		inline char& operator[] (int aPosition);

		inline void set_at(size_t aPostion, const char aChar);

		inline char get_at(size_t aPosition) const;

		CString& insert(size_t aPosition, const CString& aString);

		CString& overwrite(size_t aPosition, const CString& aString);

		CString& append(const char* aString, size_t aCount=(size_t)-1);

		CString& append(const CString& aString,
				size_t aCount=(size_t)-1);

		inline CString& replace(const CString& aString,
				size_t aCount=(size_t)-1);

		inline bool compare(const CString &aString) const;

		inline bool compare(const char *aString) const;

		inline bool compare_nocase(const CString& aString) const;

		inline bool compare_nocase(const char* aString) const;

		operator char*() const;

		const CString& operator=(const char *aString);
		const CString& operator=(const CString &aString);
		const CString& operator+=(const char *aString);
		const CString& operator+=(const CString &aString);

		friend CString operator+(const CString &aString1,
				const char *aString2);
		friend CString operator+(const char *aString1,
				const CString &aString2);
		friend CString operator+(const CString &aString1,
				const CString &aString2);

		CString& format(const char *aFormat, ...);
		friend CString& format(const char *aFormat, ...);

		void resize();

		void add_slashes();
		void mark_forward_slashes();
		void strip_slashes();
		void strip_all_slashes();
		void asciiunprintablechars();
		void htmlspecialchars();
		void htmlentities();
		void nl2br();

		void unescape();
		void urlencode();
#ifdef MEMORY_DEBUG
		bool check_border(char *msg) const;
#endif

	RECYCLE(gStringZone);
};

inline
CString
CString::clone() const
{
	return CString(*this);
}

inline void
CString::reclaim()
{
	set_size(mSize);
}

inline CString&
CString::clear()
{
	set_size(0);
	mSize = 0;
	return *this;
}

inline char*
CString::get_data() const
{
	return mData;
}

inline size_t
CString::length() const
{
	return mSize;
}

inline char&
CString::operator[] (int aPosition)
{
	if (aPosition >= 0 && (size_t)aPosition<length()) return mData[aPosition];

	system_log("out of range in CString::operator[]");

	kill(getpid(), SIGSEGV);

	static char dummy;
	return dummy;
}

inline void
CString::set_at(size_t aPosition, const char aChar)
{
	mData[aPosition] = aChar;
#ifdef MEMORY_DEBUG
	check_border("set_at()");
#endif
}

inline char
CString::get_at(size_t aPosition) const
{
	return mData[aPosition];
}

inline CString&
CString::replace(const CString &aString, size_t aCount)
{
	mSize = 0;
	return append(aString, aCount);
}

inline bool
CString::compare(const CString& aString) const
{
	if (mSize != aString.length())
		return false;
	return (memcmp(mData, aString.get_data(), mSize) == 0);
}

inline bool
CString::compare(const char *aString) const
{
	return (strcmp(mData, aString) == 0);
}

inline bool
CString::compare_nocase(const CString& aString) const
{
	if (mSize != aString.length())
		return false;
	return (strncasecmp(mData, aString.get_data(), mSize) == 0);
}

inline bool
CString::compare_nocase(const char* aString) const
{
	return (strncasecmp(mData, aString, mSize) == 0);
}

CString& add_slashes(CString& aString);
CString& mark_forward_slashes(const char *aString);
CString& strip_slashes(CString& aString);

CString& htmlspecialchars(CString& aString);
CString& htmlentities(CString& aString);
CString& nl2br(CString& aString);

CString& add_slashes(const char* aString);
CString& strip_slashes(const char* aString);
CString& htmlspecialchars(const char* aString);
CString& htmlentities(const char* aString);
CString& nl2br(const char* aString);

CString& urlencode(const char* aString);

// ---------------------------------------------------------
/**
*/
class CNode
{
	public:
		CNode();
		~CNode();

		inline CNode* next() const {return mNext;}
		inline CNode* previous() const {return mPrevious;}

		CNode* last() const;
		CNode* first() const;

		CNode* next(CNode* aNode);
		CNode* previous(CNode* aNode);

		void unlink();

	protected:
		CNode
			*mNext,
			*mPrevious;

	RECYCLE(gNodeZone);
};

// ----------------------------------------------------------
/**
*/
class CCollection
{
	public:
		CCollection();

		~CCollection();

		CNode* append(CNode* aNode);//puts new node at the end of the stack

		CNode* insert(CNode* aNode);//inserts in the begining
		CNode* insert(CNode* pNode, CNode* aNode);//inserts before the given element

		bool remove(CNode* aNode);//deletes an element

		bool unlink(CNode* aNode);//unlinks an element from the list

		CNode* first();//make internal pointer goto the begining
		CNode* next();//make internal pointer increment
		inline void operator ++ (){ next();}//alias for next()
		CNode* previous();//make internal pointer go back
		inline void operator -- (){ previous();}//alias for previous()
		CNode* last();//make internal pointer goto the end
		CNode* operator + (int aNumber);//increment a number of steps
		CNode* operator - (int aNumber);//decrement a number of steps

		void remove_all();//deletes all elements

		bool is_empty();//returns true if collection is empty

		inline int length() { return mLength; }

	protected:
		CNode
			*mRoot,
			*mTail,
			*mCurrent;
		int
			mLength;

	RECYCLE(gCollectionZone);
};

// ---------------------------------------------------------------
/**
*/
class CStringList: public CSortedList
{
	public:
		CStringList(int aLimit=64, int aThreshold = 64):
			CSortedList(aLimit, aThreshold) {}
		virtual ~CStringList();

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		virtual const char *debug_info() const { return "string list"; }
};

// ---------------------------------------------------------------
/**
*/
class CIntegerList: public CSortedList
{
	public:
		CIntegerList(int aLimit=64, int aThreshold = 64):
			CSortedList(aLimit, aThreshold) {}
		virtual ~CIntegerList();

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		CIntegerList &operator=(CIntegerList &aList);
		CIntegerList &operator+=(CIntegerList &aList);

		virtual const char *debug_info() const { return "integer list"; }
};

// ---------------------------------------------------------------
/**
*/
class CUnsignedIntegerList: public CSortedList
{
	public:
		CUnsignedIntegerList(int aLimit=64, int aThreshold = 64):
			CSortedList(aLimit, aThreshold) {}
		virtual ~CUnsignedIntegerList();

		virtual bool free_item(TSomething aItem);
		virtual int compare(TSomething aItem1, TSomething aItem2) const;
		virtual int compare_key(TSomething aItem, TConstSomething aKey) const;
		CUnsignedIntegerList &operator=(CUnsignedIntegerList &aList);
		CUnsignedIntegerList &operator+=(CUnsignedIntegerList &aList);

		virtual const char *debug_info() const { return "unsigned integer list"; }
};

// ------------------------------------------------------------
/**
*/
class CIniFile: public CBase
{
	private:
		class CBranch: public CList
		{
			public:
				CBranch(): CList(16, 8) {}
				virtual ~CBranch();
				virtual bool free_item(TSomething aItem);
				virtual const char *debug_info() const { return "branch"; }
		};

		struct TNode
		{
			char
				type;
			CBranch
				*comments;
			union
			{
				struct
				{
					char
						*name;
					CBranch
						*list;
				} section;

				struct
				{
					char
						*name;
					size_t
						size;
					TSomething
						pointer;
				} data;

				struct
				{
					char
						*text;
				} comment;
			};
		};

		CBranch
			mRoot;

		char
			mCommentChar;
		bool
			mDirty;

	public:
		CIniFile(char aCommentChar = ';');
		CIniFile(const char *aFilename, char aCommentChar = ';');
		virtual ~CIniFile();

		bool load(const char *aFilename);
		virtual bool error(int aLineNo, const char *aLine, int aPos);

		bool save(const char *aFilename);
		bool enum_sections(
				bool (*aIterator)(TSomething aParameter, char *aName),
				TSomething aParameter);
		bool enum_sections(CStringList *aList);

		bool enum_data(const char *aSectionPath,
				bool (*aIterator)(TSomething aParameter, char *aName,
						size_t aDataSize, TSomething aData),
				TSomething aParameter);
		bool enum_data(const char *aSectionPath, CStringList *aList);
		bool enum_comments(const char *aSectionPath,
				const char *aKeyName,
				bool (*aIterator)(TSomething aParameter, char *aText),
				TSomething aParameter);
		bool enum_comments(const char *aSectionPath,
				const char *aKeyName, CStringList *aList);

		bool section_exists(const char *aSectionPath);
		bool key_exists(const char *aSectionPath,
				const char *aKeyName);

		bool get_data(const char *aSectionPath, const char *aKeyName,
				TSomething &aData, size_t &aDataSize);
		int get_integer(const char *aSectionPath, const char *aKeyName,
				int aDefault = 0);
		float get_float(const char *aSectionPath, const char *aKeyName,
				float aDefault = 0.0);
		char* get_string(const char *aSectionPath, const char *aKeyName,
				char *aDefault = "");
		bool get_boolean(const char *aSectionPath, const char *aKeyName,
				bool aDefault = false);

		bool set_data(const char *aSectionPath, const char *aKeyName,
				TConstSomething aData, size_t aDataSize);
		bool set_string(const char *aSectionPath, const char *aKeyName,
				const char *aValue);
		bool set_integer(const char *aSectionPath, const char *aKeyName,
				int aValue);
		bool set_float(const char *aSectionPath, const char *aKeyName,
				float aValue);
		bool set_comment(const char *aSectionPath, const char *aKeyName,
				const char *aText);

		bool remove(const char *aSectionPath, const char *aKeyName);
		bool remove_comment(const char *aSectionPath,
				const char *aKeyName);

	private:
		bool load(bool (*aReadLine)(TSomething aStream, void *aData,
						size_t aSize),
				TSomething aStream);
		CBranch *find_node(const char *aSectionPath);
		static bool save_enum_comments(TSomething aStrunct,
						char *aText);
		static bool save_enum_data(TSomething aStruct, char *aName,
				size_t aDataSize, TSomething aData);
		static bool save_enum_section(TSomething aStruct, char *aName);

	RECYCLE(gIniFileZone);
};

/**
*/
class CCommandSet
{
	public:
		CCommandSet();
		CCommandSet(const CCommandSet& aCommandSet);

		void clear();
		bool has(int aCommand);
		void disable_command(int aCommand);
		void enable_command(int aCommand);
		void operator += (int aCommand);
		void operator -= (int aCommand);
		void disable_command(const CCommandSet& aCommandSet);
		void enable_command(const CCommandSet& aCommandSet);
		void operator += (const CCommandSet& aCommandSet);
		void operator -= (const CCommandSet& aCommandSet);

		bool is_empty();

		CCommandSet& operator &= (const CCommandSet& aCommandSet);
		CCommandSet& operator |= (const CCommandSet& aCommandSet);
		friend CCommandSet operator & (
				const CCommandSet& aCommandSet1,
				const CCommandSet& aCommandSet2);
		friend CCommandSet operator | (
				const CCommandSet& aCommandSet1,
				const CCommandSet& aCommandSet2);
		friend int operator == (
				const CCommandSet& aCommandSet1,
				const CCommandSet& aCommandSet2);
		friend int operator != (
				const CCommandSet& aCommandSet1,
				const CCommandSet& aCommandSet2);

		const char* get_string(int aSize=-1) const;
		bool set_string(const char* aString);
	private:
		int location(int aPosition);
		int mask(int aPosition);
		static int
			mMasks[8];
		unsigned char
			mCommands[128];

	RECYCLE(gCommandSetZone);
};

inline void
CCommandSet::operator+=(int aCommand)
{
	enable_command(aCommand);
}

inline void
CCommandSet::operator-=(int aCommand)
{
	disable_command(aCommand);
}

inline void
CCommandSet::operator+=(const CCommandSet& aCommandSet)
{
	enable_command(aCommandSet);
}

inline void
CCommandSet::operator-=(const CCommandSet& aCommandSet)
{
	disable_command(aCommandSet);
}

inline int
operator!=(const CCommandSet& aCommandSet1,
		const CCommandSet& aCommandSet2)
{
	return !operator == (aCommandSet1, aCommandSet2);
}

inline int
CCommandSet::location(int aCommand)
{
	return (int)(aCommand/8);
}

inline int
CCommandSet::mask(int aCommand)
{
	return mMasks[aCommand & 0x07];
}

/**
*/
class CMySQL
{
	public:
		CMySQL();
		~CMySQL();

		bool open(const char *aHost, const char *aUser,
						const char *aPassword, const char *aDatabase);
		void close();

		int query(const char *aQuery);

		inline MYSQL_RES* use_result();
		inline bool has_result();						// Added by YOSHIKI(2001/03/22)
		inline void free_result();
		inline MYSQL_ROW fetch_row();

		MYSQL *handle() { return mMySQL; }

		inline MYSQL_ROW row() { return mRow; }
		inline char *row(int aIndex);

		inline unsigned int error_no() { return (mysql_errno(mMySQL)); }
	protected:
		MYSQL_ROW
			mRow;
		MYSQL
			*mMySQL;
		MYSQL_RES
			*mResult;

	RECYCLE(gMySQLZone);
};

inline char *
CMySQL::row(int aIndex)
{
	return mRow[aIndex];
}

inline MYSQL_RES*
CMySQL::use_result()
{
	return mResult = mysql_use_result(mMySQL);
}

inline bool
CMySQL::has_result()
{
	if (mResult) return true;

	return false;
}

inline void
CMySQL::free_result()
{
	if( mResult ) { mysql_free_result(mResult); mResult = NULL; }
}

inline MYSQL_ROW
CMySQL::fetch_row()
{
	return mRow = mysql_fetch_row(mResult);
}

/**
*/
class CScript: public CBase
{
	protected:
		class CBranch: public CList
		{
			public:
				CBranch():CList(16, 8) {}
				virtual ~CBranch();
				virtual bool free_item(TSomething aItem);
				virtual const char *debug_info() const { return "branch script"; }
		};

		struct TNode
		{
			char
				type;

			union
			{
				struct
				{
					char
						*name;
					CBranch
						*list;
				} section;

				struct
				{
					char
						*name;
					CBranch
						*list;
				} array;

				struct
				{
					char
						*name;
					size_t
						size;
					TSomething
						pointer;
				} data;
			};
		};

	public:
		CScript();
		virtual ~CScript();

		bool load(const char *aFilename);

		TSomething get_section(const char *aName,
				TSomething aParent = NULL);
		TSomething get_array(int aIndex, TSomething aArray = NULL);

		int get_type(TSomething aData);
		char* get_name(TSomething aData);
		char* get_data(TSomething aData);
	protected:
		CBranch
			mRoot;
		virtual void error(int aLineNo, const char *aMessage);

	RECYCLE(gScriptZone);
};

double as_sin( int degree );
double as_cos( int degree );
int as_asin( double as );
int as_acos( double ac );


#endif
