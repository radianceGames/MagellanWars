#include "net.h"
#include "util.h"
#include <stdlib.h>
#include <fstream>
#include <string.h>

char* harvestPHPSessionVar(char* aVariableName, char* aPath)
{
	std::ifstream inFile(aPath);//typically /tmp/sess_<sessionid>
	if (inFile.fail()) return NULL;
	char line[80];
	char *aValue;
	int ch;
	while ( (ch = inFile.peek()) != EOF)
	{
		inFile.getline(line, 80, '|');
		if (!strcmp(line, aVariableName))
		{
			inFile.getline(line, 80, ':'); // discard variable type
			inFile.getline(line, 80, ';'); // get the value
			aValue = (char*)malloc(strlen(line) + 1);
			strcpy(aValue,line);
			inFile.close();
			return aValue;
		}
		inFile.getline(line, 80, ';');
	}
	inFile.close();
	return NULL;
} 

void writePHPSessionVar(char* aVariableName, char* aVariableValue, char* aVariableType, char* aPath)
{
	std::ifstream inFile(aPath);//typically /tmp/sess_<sessionid>
	if (inFile.fail()) return;
	char line[80];
	int ch, flag;
	static CString aOut;
	aOut.clear();
	flag = 0;
	while ( (ch = inFile.peek()) != EOF)
	{
		inFile.getline(line, 80, '|');
		aOut.format("%s|", line);
		if (!strcmp(line, aVariableName))
		{
			aOut.format("%s:%s;",aVariableType,aVariableValue);
			inFile.getline(line, 80, ';'); // goto next value
			flag = 1;
		}
		else
		{
			inFile.getline(line, 80, ';'); // goto next value
			aOut.format("%s;",line);
		}
	}
	inFile.close();
	if (flag == 0)
	{
		aOut.format("%s:%s;",aVariableType,aVariableValue);
	}
	std::ofstream outFile(aPath);
	outFile << aOut;
	outFile.close();
}

