#include "cgi.h"
#include <cstdio>
#include <fcntl.h>

CHTML::CHTML()
{
}

CHTML::~CHTML()
{
}

const char *
CHTML::get_name()
{
	return (char*)mName;
}

const char *
CHTML::get_html()
{
	return (char*)mData;
}

bool
CHTML::load(const char *aPath, const char *aName)
{
	if (!aName) return false;
	if (!aPath) return false;

	char
		Filename[256];

	sprintf(Filename, "%s/%s", aPath, aName);
//	system_log("html load %s", Filename);
	

	if (access(Filename, F_OK+R_OK) != 0) return false;

	int
		File = 0;
	int 
		Size = 0;

	File = open(Filename, O_RDONLY);
	if (File)
	{
		int
			NRead;
		char
			Buffer[4097];

		mData.clear();
		while((NRead = pth_read(File, Buffer, 4096)))
		{
			Buffer[NRead] = 0;
			mData.append(Buffer);
			Size += NRead;
		}
		close(File);

		mName = aName;
//		system_log("html load complete [%x]%s", 
//			(char*)mName, (char*)mName);

		return true;
	}

	system_log("could not open html file %s", Filename);

	return false;
}

bool
CHTML::get(CString &aOutput, CQueryList *aConversions)
{
	int 
		Length = mData.length();

	int 
		Done = 0;

	if (!aConversions) 
	{
		aOutput.append(get_html());
		return true;
	}

	char 
		*Src = (char*)get_html();

//	system_log("source\n[%s]", Src);

	while(Done < Length)
	{
		int 
			Size = 0;

//		system_log("done:%d length:%d", Done, Length);

		while((Src[Size]) && (Src[Size] != '$')) Size++;

		if (Src[Size] == '$')
		{
			if (Src[Size+1] == '$')
			{
				aOutput.append(Src, Size+1);
				Src = &Src[Size+2];
				Done += Size+2;
			} else {
				aOutput.append(Src, Size);

				int 
					Start = 0;

				char
					*FS = &Src[Size + 1];
				while((FS[Start]) &&
						(((FS[Start] >= 'A')
							&& (FS[Start] <= 'Z'))
							|| (FS[Start] == '_'))) Start++;

				char
					Field[256];

				if (Start)
				{
					strncpy(Field, FS, Start);
					Field[Start] = 0;
//					system_log("get item %s", Field);
					
					char *
						Value = aConversions->get_value(Field);
//					system_log("get_item end");
					if (Value)
					{
//						system_log("item %s", Field);
						aOutput.append(Value);
					} else {
//						system_log("non item");
						aOutput.append("$");
						aOutput.append(Field);
					}
				}

				Src = &Src[Size+1+Start];
				Done += Size + 1 + Start;
			}
		} else {
//			system_log("tail");
			aOutput.append(Src, Size);
			Done += Size;
		}
	}
//	system_log("CHTML::get end");
	return true;
}
