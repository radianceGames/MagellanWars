#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool get_message(char* aInputName, 
		FILE *aOutputFile, FILE* aInputFile);

int
main(int argc, char* argv[])
{
	char
		OutputFile[256];
	int 
		InputIndex;

	if (argc < 2)
	{
		printf("Usage: %s [-o OUTPUTFILE] INPUTFILE ...\n",
						argv[0]);
		printf("\n");
		return -1;
	}

	if (strcmp(argv[1], "-o"))
	{
		InputIndex = 1;
		strcpy(OutputFile, "message.msg");
	} else {
		if (argc < 4) 
		{
			printf("Usage: %s [-o OUTPUTFILE] INPUTFILE ...\n",
						argv[0]);
			printf("\n");
			return -1;
		}
		InputIndex = 3;
		strcpy(OutputFile, argv[2]);
	}

	FILE
		*Output = fopen(OutputFile, "w");

	if (!Output)
	{
		printf("Could not open OutputFile:%s\n", OutputFile);
		return -1;	
	}

	for(; InputIndex < argc; InputIndex++)
	{
		FILE
			*Input = fopen(argv[InputIndex], "r");
		if (!Input)
		{
			printf("Could not open InputFile:%s\n", 
					argv[InputIndex]);
			fclose(Output);
			unlink(OutputFile);
			return -1;
		}
		if (!get_message(argv[InputIndex], Output, Input)) 
		{
			printf("Could not get message:%s\n", 
					argv[InputIndex]);
			fclose(Output);
			unlink(OutputFile);
			return -1;
		}
		fclose(Input);
	}

	fclose(Output);

	return 0;
}

extern FILE* _MSGin;
extern int LineNo;
extern void _MSGrestart(FILE *aFILE);
extern int _MSGlex(void);
extern char *_MSGtext;

#define TEXT		400

#define GETTEXT		500
#define BEGIN_ITEM	501
#define END_ITEM	502

#define ETC			2000
#define LINEFEED	20001

#define BEGIN_REMARK 	1000
#define END_REMARK		1001

bool 
get_message(char* aInputName, FILE* aOutput, FILE* aInput)
{
	(void)aOutput;
	(void)aInput;

	static char 
		Buffer[51200];

	_MSGin = aInput;
	_MSGrestart(_MSGin);

	LineNo = 1;

	bool 
		Remark = false;

	int 
		Val,
		Item = 0;

	Buffer[0] = 0;
	while((Val = _MSGlex())) 
	{
//		printf("%4d:%03d|[%s]\n", LineNo, Val, _MSGtext);
		if (Val == END_REMARK)
		{
			if (!Remark)
			{
				printf("error in line %d\n", LineNo);
				return false;
			}
			Remark = false;
		} 
		if (Remark)
			continue;
		if (Val == BEGIN_REMARK)
		{
			Remark = true;
		} else 
		if (Val == GETTEXT)
		{
			strcat(Buffer, _MSGtext);
			while((Val = _MSGlex()))
			{
				if (Val == END_ITEM)
				{
					strcat(Buffer, _MSGtext);
					fprintf(aOutput, "%s\n\n", Buffer);
					Buffer[0] = 0;
					Item++;
					break;	
				} else strcat(Buffer, _MSGtext);
			}
		}
	}
	if (Remark)
	{
		printf("unterminate remark error in line %d\n", LineNo);
		return false;
	}
	
	printf("found item %d in %s\n", Item, aInputName);

	return true;
}
