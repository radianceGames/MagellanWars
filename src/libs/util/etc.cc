#include <sys/types.h>
#include <cstring>
#include "debug.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "util.h"
#include <cctype>

char SYSTEM_LOGFILE[256] = "./systemlog";

char*
string_new(const char *aString)
{
	if (aString)
	{
		size_t
			StringLength = strlen(aString) + 1;
		MEMORY(char *Res = new char [StringLength]);
		memcpy(Res, aString, StringLength);
		Res[StringLength-1] = 0;
		return Res;
	} else
		return NULL;
}

void
log(const char* aLogName, const char* aFormat, va_list aArgs)
{
	long
		Time;
	char
		*TimeString;

	FILE *File;

	File = fopen(aLogName, "a");
	if (!File)
	{
		printf("Could not open log file %s\n", aLogName);
		exit(-1);
	}

	Time = time(0);
	TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) -1) = '\0';

	fprintf(File, "%s :: ", TimeString+4);
	vfprintf(File, aFormat, aArgs);
	fprintf(File, "\n");

	fclose(File);
}

void
log_for_pstat(const char* aLogName, const char* aFormat, va_list aArgs)
{
	long
		Time;
	char
		*TimeString;

	FILE *File;

	File = fopen(aLogName, "a");
	if (!File)
	{
		printf("Could not open log file %s\n", aLogName);
		exit(-1);
	}

	Time = time(0);
	TimeString = (char *)asctime(localtime(&Time));
	*(TimeString + strlen(TimeString) -6) = '\0';

	fprintf(File, "%s ", TimeString+4);
	vfprintf(File, aFormat, aArgs);
	fprintf(File, "\n");

	fclose(File);
}

void
system_log(const char *aFormat, ...)
{
	va_list Args;

	va_start(Args, aFormat);
	log(SYSTEM_LOGFILE, aFormat, Args);
	va_end(Args);
}

void
system_log_for_pstat(const char *aFormat, ...)
{
	va_list Args;

	va_start(Args, aFormat);
	log_for_pstat(SYSTEM_LOGFILE, aFormat, Args);
	va_end(Args);
}

char *
split_word(char *aOut, char *aIn, char aStop)
{
	while(*aIn == ' ') aIn++;
	while(*aIn != aStop && *aIn)
		*aOut++ = *aIn++;

	*aOut = 0;
	if (*aIn)
		return aIn+1;

	return NULL;
}

char
hex2dec(char *aHex)
{
	char
		Char;

	Char = ((aHex[0] >= 'A') ?
			((aHex[0] & 0xDF) - 'A') + 10 : (aHex[0] - '0'));
	Char <<= 4;
	Char +=  ((aHex[1] >= 'A') ?
			((aHex[1] & 0xDF) - 'A') + 10 : (aHex[1] - '0'));

	return Char;
}

void
unescape(char *aString)
{
	int
		i,
		j;

	for (i=0, j=0; aString[j]; i++, j++)
	{
		if ((aString[i] == aString[j]) == '%')
		{
			aString[i] = hex2dec(aString+j+1);
			j += 2;
		} else if (aString[j] == '+') {
			aString[i] = ' ';
		}
	}
	aString[i] = 0;
}

// 1 - i 사이 랜덤 넘버
int number( int i )
{
  static int initialized;
  double x;

  if( i <= 0 ) return 0;

  if( initialized == 0 ){
  	long
		seed = time(0) * 1000 + getpid();
	srandom(seed);
	srand48(seed);

    initialized = 1;
  }

  x = drand48() * (double)i + 1.0;
  if( (int)x > i ) x = i;

  return (int)x;
}

int dice( int i, int j )
{
  int ret = 0;

  if( i <= 0 || j <= 0 ) return 0;
  for( ; i > 0; i-- ){
    ret += number(j);
  }
  return ret;
}

char*
dec2unit(int aNumber)
{
	static const
		char NumberPool[] = "0123456789";

	static char
		Result[31] = { 0, };

	int
		Number = abs(aNumber);

	memset(Result, 0, sizeof(Result));

	int
		Position = 0;
	int
		Comma = 0;

	if (!Number)
	{
		Result[29] = '0';
		return &Result[29];
	}

	while(Number)
	{
		Result[29-Position++] = NumberPool[Number % 10];
		Comma++;
		Number /= 10;
		if (Number && Comma && (Comma % 3) == 0)
		{
			Result[29-Position++] = ',';
		}
	}

	if (aNumber < 0)
		Result[29-Position++] = '-';
	return &Result[29-Position+1];
}

char*
dec2unit_for_unsigned_int(unsigned int aNumber)
{
	static const
		char NumberPool[] = "0123456789";

	static char
		Result[31] = { 0, };

	memset(Result, 0, sizeof(Result));

	int
		Position = 0;
	int
		Comma = 0;

	if (!aNumber)
	{
		Result[29] = '0';
		return &Result[29];
	}

	while(aNumber)
	{
		Result[29-Position++] = NumberPool[aNumber % 10];
		Comma++;
		aNumber /= 10;
		if (aNumber && Comma && (Comma % 3) == 0)
		{
			Result[29-Position++] = ',';
		}
	}

	return &Result[29-Position+1];
}

char *
remove_comma(const char *aNumberString)
{
	static char Buffer[512*1024];

	int
		Index = 0;

	for (; *aNumberString ; aNumberString++)
	{
		if (!(*aNumberString >= '0' && (*aNumberString) <= '9' ||
			*aNumberString == '.' ||
			*aNumberString == '-')) continue;

		Buffer[Index++] = *aNumberString;
	}

	Buffer[Index] = '\0';

	if (!strlen(Buffer)) return "0";
	return Buffer;
}

int
as_atoi(const char *aNumberString)
{
	if( aNumberString == NULL )
		return 0;
	return atoi(remove_comma(aNumberString));
}

double
as_atof(const char *aNumberString)
{
	if( aNumberString == NULL )
		return 0;
	return atof(remove_comma(aNumberString));
}

char *
timetostring(time_t aTime)
{
    int TotalMinutes = (int)((double)aTime/(double)60.0 + 0.5);
    int DaysLeft = TotalMinutes / 60 / 24;
    int HoursLeft = (TotalMinutes - (DaysLeft * 60 * 24)) / 60;
    int MinutesLeft = TotalMinutes - (HoursLeft * 60) - (DaysLeft * 60 * 24);

    static CString aOutput;
    aOutput.clear();

    if (DaysLeft > 1)
       aOutput.format("%d Days", DaysLeft);
    else if (DaysLeft > 0)
       aOutput.format("%d Day", DaysLeft);

    if (aOutput.length() > 0)
       aOutput += " ";

    if (HoursLeft > 1)
       aOutput.format("%d Hours", HoursLeft);
    else if (HoursLeft > 0)
       aOutput.format("%d Hour", HoursLeft);

    if (aOutput.length() > 0)
       aOutput += " ";

    if (MinutesLeft > 1)
       aOutput.format("%d Minutes", MinutesLeft);
    else if (MinutesLeft > 0)
       aOutput.format("%d Minute", MinutesLeft);
    else if (TotalMinutes <= 0)
       aOutput.format("less than 1 Minute");
    return (char*)aOutput;
}

char *
integer_with_sign(int aNumber)
{
	if (aNumber > 0) return (char *)format("+%d", aNumber);

	return (char *)format("%d", aNumber);
}

bool
as_isspace(int aChar)
{
	if (aChar == '\n' || aChar == ' ' || aChar == '\b') return true;
	return false;
}

int
as_calc_date_index( int aNumber )
{
	if( aNumber <=1 ) return 0;

	time_t
		t = time(0);

	int
		i = t/86400;

	return i%aNumber;
}

int
as_calc_date_index( time_t t, int aNumber )
{
	if( aNumber <=1 ) return 0;

	int
		i = t/86400;

	return i%aNumber;
}

bool
is_valid_name(const char *aString)
{
	if (!aString || strlen(aString) == 0) return false;
    if (strlen(aString) > 25) return false;
	int
		NumberOfSpace = 0;
	for (int i=0 ; i<(int)strlen(aString) ; i++)
	{
		char
			Char = *(aString+i);
		if (Char == ' ')
		{
			NumberOfSpace++;
			continue;
		}
		if (isspace(Char)) return false;

	}

	if (NumberOfSpace == (int)strlen(aString)) return false;

	return true;
}
bool
is_valid_slogan(const char *aString)
{
	if (!aString || strlen(aString) == 0) return false;
    if (strlen(aString) > 100) return false;
	int
		NumberOfSpace = 0;
	for (int i=0 ; i<(int)strlen(aString) ; i++)
	{
		char
			Char = *(aString+i);
		if (Char == ' ')
		{
			NumberOfSpace++;
			continue;
		}
		if (isspace(Char)) return false;

	}

	if (NumberOfSpace == (int)strlen(aString)) return false;

	return true;
}

char *
unsigned_int_to_char(unsigned int aNumber)
{
	int
		Length = sizeof(unsigned int);
	char
		Temp[Length + 1];
	char
		MaxString[32];

	memset(&Temp, 255, Length);
	Temp[Length] = '\0';

	sprintf(MaxString, "%u", *(unsigned int *)Temp);

	int
		MaxLength = strlen(MaxString);

	static char
		Result[32];

	Result[MaxLength] = '\0';

	for (int i=MaxLength-1 ; i>=0 ; i--)
	{
		int
			Mod = aNumber % 10;
		aNumber = aNumber / 10;
		Result[i] = Mod + '0';
	}

	return Result;
}

void
add_dot_for_sendmail(char *aString, CString *aResultString)
{
	if (aString == NULL) return;

	int
		MaxResultLength = strlen(aString) + strlen(aString)/2;

	char *
		Result = new char[MaxResultLength + 1];

	int
		IndexOld = 0,
		IndexNew = 0;

	while (*(aString + IndexOld) != '\0')
	{
		if (*(aString + IndexOld) == '.')
		{
			if (IndexOld == 0)
			{
				if (
					*(aString + IndexOld + 1) == '\n' ||
					*(aString + IndexOld + 1) == '\0' ||
					*(aString + IndexOld + 1) == '.')
				{
					Result[IndexNew] = '.';
					IndexNew++;
				}
			}
			else if (*(aString + IndexOld - 1) == '\n')
			{
				if (
					*(aString + IndexOld + 1) == '\n' ||
					*(aString + IndexOld + 1) == '\0' ||
					*(aString + IndexOld + 1) == '.')
				{
					Result[IndexNew] = '.';
					IndexNew++;
				}
			}
		}

		Result[IndexNew] = *(aString + IndexOld);
		IndexOld++;
		IndexNew++;
	}

	Result[IndexNew] = '\0';

	*aResultString = Result;

	delete Result;
}

pth_time_t
get_pth_nap_time(int aNapTime)
{
	int
		Min = aNapTime/60;
	int
		Sec = aNapTime%60;

	return (pth_time_t){Min, Sec};
}

char *
plural(int aNumber)
{
	if (aNumber != 1) return "s";
	return "";
}
