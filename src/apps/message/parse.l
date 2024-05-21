%{
#define YY_NO_UNPUT

#define TEXT			400

#define GETTEXT			500
#define BEGIN_ITEM		501
#define END_ITEM		502

#define ETC				2000
#define LINEFEED		2001

#define BEGIN_REMARK	1000
#define END_REMARK		1001

/*static void _ASFatalError(const char *aMessage);

#define YY_FATAL_ERROR(msg) _ASFatalError(msg)*/

int
	LineNo = 0;

%}
%%
\"[^\"]*\"			{ return TEXT; }
GETTEXT				{ return GETTEXT; }
\(					{ return BEGIN_ITEM; }
\)					{ return END_ITEM; }
\n					{ LineNo++; return LINEFEED; }
.					{ return ETC; }
\/\/.*				;
\/\*				{ return BEGIN_REMARK; }
\*\/				{ return END_REMARK; }
%%

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int
_MSGwrap(void)
{
	return feof(yyin);
}

/*
main(int argc, char* argv[])
{
	int 
		val;

	yyin = fopen("/space/space/src/apps/message/test.cc", "r");

	while(val =	yylex()) printf("%4d:%03d|[%s]\n", LineNo, val, yytext);
}
*/
