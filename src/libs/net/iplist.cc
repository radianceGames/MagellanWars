#include "net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>

CIPList::CIPList()
{
	mDirty = 0;
	mFilename = "";
}

CIPList::~CIPList()
{
	remove_all();
}

bool
CIPList::free_item(TSomething aItem)
{
	CIP
		*IP = (CIP*)aItem;

	if (!IP) return false;

	delete IP;

	return true;
}

int
CIPList::compare(TSomething aItem1, TSomething aItem2) const
{
	CIP
		*IP1 = (CIP*)aItem1,
		*IP2 = (CIP*)aItem2;

	if (IP1->get_address() > IP2->get_address()) return 1;
	if (IP1->get_address() < IP2->get_address()) return -1;

	return 0;
}

int 
CIPList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CIP
		*IP = (CIP*)aItem;

	if (IP->get_address() > (unsigned long)aKey) return 1;
	if (IP->get_address() < (unsigned long)aKey) return -1;

	return 0;
}

bool
CIPList::add_ip(const char *aIPAddress)
{
	unsigned long 
		Address;

	struct hostent
		*HostEntry;

//	system_log("IPAddress:%s", aIPAddress);

	if ((Address = inet_addr(aIPAddress)) == -1)
	{
//		system_log("Lookup name:%s", aIPAddress);

		HostEntry = gethostbyname(aIPAddress);

//		system_log("pass2");
		if (!HostEntry)
		{
			system_log("Could not get ip address(%s)", aIPAddress);
			return false;
		}
//		system_log("result:%d, %d", HostEntry->h_addr, 
//									HostEntry->h_length);
	
		memcpy((char*)&Address, 
				HostEntry->h_addr, HostEntry->h_length);
	}

	CIP
		*IP = new CIP();

	IP->set_name(aIPAddress);
	IP->set_address(Address);

	insert_sorted(IP);

	mDirty = true;

	return true;
}

bool
CIPList::remove_ip(unsigned long aIPAddress)
{
	int 
		Pos = find_sorted_key((TSomething)aIPAddress);

	if (Pos < 0) return false;

	CList::remove(Pos);

	mDirty = true;

	return true;
}

CIP *
CIPList::get_by_address(char *aIPAddress)
{
	if (!length()) return NULL;

	unsigned long
		Address;

	struct hostent
		*HostEntry;

	if ((Address = inet_addr(aIPAddress)) == -1)
	{
		HostEntry = gethostbyname(aIPAddress);

		if (!HostEntry)
		{
			system_log("Could not get ip address(%s)", aIPAddress);
			return NULL;
		}

		memcpy((char*)&Address,
			HostEntry->h_addr, HostEntry->h_length);
	}

	int
		Index = find_sorted_key((void *)Address);

	if (Index < 0) return NULL;

	return (CIP *)get(Index);
}

bool
CIPList::load(const char* aFilename)
{
	mFilename = aFilename;

	const char 
		CharPool[] = "01234567890."
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz";

	char 
		Buffer[4096];
	FILE 
		*File;

	File = fopen(aFilename, "r");

	system_log("open file %s", aFilename);

	if (!File) 
	{
		system_log("Could not open ip list file %s", aFilename);
		return false;
	}

	while(fgets(Buffer, 4095, File))
	{
//		system_log("[%s]:%d", Buffer, strlen(Buffer));
		int 
			i=0;
		while(Buffer[i] && strchr(CharPool, Buffer[i])) i++;
		if (Buffer[i]) 
			Buffer[i] = 0;
//		system_log("pass [%s]", Buffer);
	
		if (!add_ip(Buffer)) 
		{
			system_log("Could not add this ip %s", Buffer);
			return false;
		}
	}

	fclose(File);

	mDirty = false;

	return true;
}

bool
CIPList::save()
{
	if (!mDirty) 
		return true;

	FILE
		*File;

	File = fopen((char*)mFilename, "w");

	if (!File)
	{
		system_log("Could not open ip list file %s", 
											(char*)mFilename);
		return false;
	}

	for(int i=0; i<length(); i++)
	{
		CIP 
			*IP = (CIP*)get(i);

		fprintf(File, "%s\n", IP->get_name());
	}

	fclose(File);

	mDirty = false;

	return true;
}

bool 
CIPList::has(unsigned long aIPAddress)
{
	int 
		Pos = find_sorted_key((TSomething)aIPAddress);

	if (Pos < 0) return false;

	return true;
}
