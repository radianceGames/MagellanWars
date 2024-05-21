#include "common.h"
#include "util.h"
#include "cgi.h"

TZone gQueryListZone = 
{
	PTH_MUTEX_INIT,
    recycle_allocation,
	recycle_free,
	sizeof(CQueryList),
	0,
	0,
	NULL,   
	"Zone CQueryList",
};

CQueryList::CQueryList()
{
}

CQueryList::CQueryList(CQueryList &aQueryList)
{
	for(int i=0; i<aQueryList.length(); i++)
	{
		CQuery *
			Query = (CQuery *)aQueryList.get(i);
		set_value(Query->get_name(), Query->get_value());		
	}
}

CQueryList::~CQueryList()
{
	remove_all();
}

bool
CQueryList::free_item(TSomething aItem)
{
	CQuery
		*Query = (CQuery*)aItem;

	if (!Query) 
		return false;

	delete Query;

	return true;
}

int
CQueryList::compare(TSomething aItem1, TSomething aItem2) const
{
	CQuery
		*Item1 = (CQuery*)aItem1,
		*Item2 = (CQuery*)aItem2;

	return strcasecmp(Item1->get_name(), Item2->get_name());
}

int 
CQueryList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CQuery 
		*Item = (CQuery*)aItem;

	return strcasecmp(Item->get_name(), (char*)aKey);
}

void
CQueryList::set(CString &aString, const char aSeperator)
{
	char
//		Buffer[STRING_DATA_BLOCK_SIZE],
//		Field[STRING_DATA_BLOCK_SIZE];
		Buffer[aString.length() + 1],
		Field[aString.length() + 1];

	remove_all();

	if (!aString.length()) return;

	char 
		*Next,
		*Value;
	
	Next = split_word(Buffer, aString.get_data(), aSeperator);
	while(1)
	{
		Value = split_word(Field, Buffer, '=');
		if (Field && Value)
		{
			if (strlen(Field))
			{
				CQuery
					*Query = new CQuery;
				Query->set(Field, Value);
				Query->unescape_value();
				insert_sorted(Query);
			}
		} else break;
		if (!Next) break;
		Next = split_word(Buffer, Next, aSeperator);
	}	
}

void
CQueryList::set(const char *aString, const char aSeperator)
{
	static CString
		String;

	if (!aString) return;

	String = aString;

	set(String, aSeperator);
}



const char*
CQueryList::get(const char aSeperator)
{
	static CString
		Output;
	Output.clear();

	for (int i=0 ; i<length() ; i++)
	{
		CQuery *
			Query = (CQuery*)CSortedList::get(i);
		if (Query->get_name() == NULL) continue;

		if (Query->get_value() == NULL)
		{
			Output.format("%s=%c", Query->get_name(), aSeperator);
		}
		else
		{
			Output.format("%s=%s%c",
							Query->get_name(), 
							(char*)urlencode(Query->get_value()),
							aSeperator);
		}
	}
	if (Output.length())
		Output.set_size(Output.length()-1);

	return (char*)Output;
}

CQuery*
CQueryList::find_query(const char *aName)
{
	int 
		Res = find_sorted_key(aName);
	if (Res < 0) return NULL;

	return (CQuery*)CSortedList::get(Res);
}

char *
CQueryList::get_value(const char *aName)
{
	CQuery 
		*Query = find_query(aName);

	if (!Query) return NULL;

	return Query->get_value();
}

void
CQueryList::set_value(const char *aName, const char *aValue)
{
	if (!aName) return; 

	CQuery 
		*Query = find_query(aName);

	if (Query)
	{
		Query->set_value(aValue);
	} else {
		Query = new CQuery();
		Query->set(aName, aValue);
		insert_sorted(Query);
	}
}

void
CQueryList::set_value(const char *aName, const int aValue)
{
	CString
		Buf;

	Buf.format("%d", aValue);
	set_value( aName, (char*)Buf );
}

bool
CQueryList::replace_value(const char *aName, const char *aValue)
{
	CQuery 
		*Query = find_query(aName);

	if (!Query) return false;

	Query->set_value(aValue);

	return true;
}
