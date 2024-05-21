#include "cgi.h"
#include <fcntl.h>
#include <sys/stat.h>

bool  
CFileHTML::write(const char *aFilename, CQueryList *aConversion)
{    
    int     
		File;   

	File = open(aFilename, O_WRONLY+O_CREAT+O_TRUNC);

	if (File < 0) 
	{
		system_log("could not write a FileHTML:%s", aFilename);
		return false;
	}

	CString 
		Output; 

	if (!get(Output, aConversion)) 
		return false;

	if (::write(File, (char*)Output, 
				Output.length()) != (signed)Output.length())
		return false;

	fchmod(File, 0644);

	close(File);    

	return true;
}
