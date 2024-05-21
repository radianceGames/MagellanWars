%{
#define YY_NO_UNPUT

#define NUMBER			400
#define TEXT			401
#define NAME			402

#define BEGIN_ITEM		500
#define END_ITEM		501

#define BEGIN_SECTION	600
#define END_SECTION		601

#define BEGIN_ARRAY		700
#define END_ARRAY		701

#define CONT			1000
#define UNTERMINATE		1100

#define ETC				2000


static void _ASFatalError(const char *aMessage);

#define YY_FATAL_ERROR(msg) _ASFatalError(msg)

int
	LineNo = 0;

%}
%%
[ \t]+				;
[0-9]+			|
[0-9]+\.[0-9]+	|
-[0-9]+			|
\+[0-9]+		|
\.[0-9]+			{ return NUMBER; }
#.*					;
'[^']*'			|
\"[^\"]*\"			{ 
						if (yytext[yyleng-2] == '\\')
						{
							yyless(yyleng-1);
							yymore();
						} else return TEXT;
					}
\"[^\"\n]*$		| 	{ return UNTERMINATE }
[a-zA-Z][a-zA-Z 0-9]+\( { return BEGIN_ITEM; }
\)					{ return END_ITEM; }
\<[^\>]*\>			{ return NAME; }
\{					{ return BEGIN_SECTION; }
\}					{ return END_SECTION; }
\[					{ return BEGIN_ARRAY; }
\]					{ return END_ARRAY; }
\,					{ return CONT; }	
\n					{ LineNo++; }
.					{ return ETC; }
%%

#include "util.h"

static void _ASFatalError(const char* aMessage)
{
	system_log("Script error:%s", aMessage);
	yy_fatal_error(aMessage);
}

int
_ASwrap(void)
{
	return feof(_ASin);
}

/*
main(int argc, char* argv[])
{
	int 
		val;

	struct stat 
		Stat;	
	
	int 
		File = -1;

	File = open("/space/space/test/sample.scr", O_RDONLY);
	if (File < 0) return -1;

	fstat(File, &Stat);

	Data = (char*)malloc(Stat.st_size);

	read(File, Data, Stat.st_size);

	close(File);

	printf("%s", Data);

	free(Data);

	yyin = fopen("/space/space/test/sample.scr", "r");

	while(val =	yylex()) printf("%4d:%03d|[%s]\n", LineNo, val, yytext);
}
*/
