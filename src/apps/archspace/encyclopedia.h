#if !defined(__ARCHSPACE_ENCYCLOPEDIA_H__)
#define __ARCHSPACE_ENCYCLOPEDIA_H__

#include "cgi.h"

class CTech;
class CRaceTable;
class CRace;
class CProject;
class CComponent;
class CShipSizeTable;
class CShipSize;
class CSpyTable;
class CSpy;
                          
/**
*/
class CEncyclopedia: public CFileHTML
{
	public:
		~CEncyclopedia() {}
        
		bool read(const char *aPageName);
		bool write();
        inline char* lighter(char* source);
        inline char* darker(char* source);
	protected:
		CQueryList
			mConversion;
};

inline char* CEncyclopedia::lighter(char* source)
{
	int index = 0;
	char *dump = new char[sizeof(source)];
	while(source[index] != '\0')
	{
		switch (source[index])
		{
			//case '0': should only modify existing colors
			//	dump[index] = '1';
			//	break;
			case '1':
				dump[index] = '2';
				break;
			case '2':
				dump[index] = '3';
				break;
			case '3':
				dump[index] = '4';
				break;
			case '4':
				dump[index] = '5';
				break;
			case '5':
				dump[index] = '6';
				break;
			case '6':
				dump[index] = '7';
				break;
			case '7':
				dump[index] = '8';
				break;
			case '8':
				dump[index] = '9';
				break;
			case '9':
				dump[index] = 'A';
				break;
			case 'A':
				dump[index] = 'B';
				break;
			case 'B':
				dump[index] = 'C';
				break;
			case 'C':
				dump[index] = 'D';
				break;
			case 'D':
				dump[index] = 'E';
				break;
			case 'E':
				dump[index] = 'F';
				break;
			default:
				dump[index] = source[index];
		}
		index++;
	}
	dump[index] = '\0';
	return dump;
} 


inline char* CEncyclopedia::darker(char* source)
{
	int index = 0;
	char *dump = new char[sizeof(source)];
	while(source[index] != '\0')
	{
		switch (source[index])
		{
			//case '0': should only modify existing collors
			//	dump[index] = '1';
			//	break;
			case '1':
				dump[index] = '0';
				break;
			case '2':
				dump[index] = '1';
				break;
			case '3':
				dump[index] = '2';
				break;
			case '4':
				dump[index] = '3';
				break;
			case '5':
				dump[index] = '4';
				break;
			case '6':
				dump[index] = '5';
				break;
			case '7':
				dump[index] = '6';
				break;
			case '8':
				dump[index] = '7';
				break;
			case '9':
				dump[index] = '8';
				break;
			case 'A':
				dump[index] = '9';
				break;
			case 'B':
				dump[index] = 'A';
				break;
			case 'C':
				dump[index] = 'B';
				break;
			case 'D':
				dump[index] = 'C';
				break;
			case 'E':
				dump[index] = 'D';
				break;
			case 'F':
				dump[index] = 'E';
			default:
				dump[index] = source[index];
		}
		index++;
	}
	dump[index] = '\0';
	return dump;
} 

/**
*/
class CEncyclopediaTechIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaTechIndexGame() {}

		bool set(int aTechType);
};

/**
*/
class CEncyclopediaTechPageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaTechPageGame() {}

		bool set(CTech *aTech);
};

/**
*/
class CEncyclopediaRaceIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaRaceIndexGame() {}

		bool set(CRaceTable *aRaceTable);
};

/**
*/
class CEncyclopediaRacePageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaRacePageGame() {}

		bool set(CRace *aRace);
};

/**
*/
class CEncyclopediaProjectIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaProjectIndexGame() {}
		
		bool set(int aProject);
};

/**
*/
class CEncyclopediaProjectPageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaProjectPageGame() {}

		bool set(CProject *aProject);
};


/**
*/
class CEncyclopediaComponentIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaComponentIndexGame() {}
		bool set(int aComponentType);
};

/**
*/
class CEncyclopediaComponentPageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaComponentPageGame() {}
		bool set(CComponent* aComponent);
};

/**
*/
class CEncyclopediaShipIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaShipIndexGame() {}
		bool set(CShipSizeTable *aShipTable);
};

/**
*/
class CEncyclopediaShipPageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaShipPageGame() {}
		bool set(CShipSize* aShip);
};

/**
*/
class CEncyclopediaSpyIndexGame: public CEncyclopedia
{
	public:
		~CEncyclopediaSpyIndexGame() {}
		bool set(CSpyTable *aSpyTable);
};

class CEncyclopediaSpyPageGame: public CEncyclopedia
{
	public:
		~CEncyclopediaSpyPageGame() {}
		bool set(CSpy *aSpy);
};

#endif
