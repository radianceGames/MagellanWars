#include "common.h"
#include "util.h"
#include "tech.h"
#include "define.h"
#include "archspace.h"
#include "game.h"

const int mPowerMult = 10;

TZone gKnownTechZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CKnownTech),
	0,
	0,
	NULL,
	"Zone CKnownTech"
};

CKnownTech::CKnownTech()
{
	mTech = NULL;
	mAttribute = 0;
	mOwner = 0;
}

CKnownTech::CKnownTech(int aOwner, int aTechID, int aAttribute)
{
	CTech
		*Tech = TECH_TABLE->get_by_id(aTechID);
	
	mOwner = aOwner;
	mAttribute = aAttribute;
	mTech = Tech;
}

CKnownTech::CKnownTech( CTech *aTech )
{
	mTech = aTech;
	mAttribute = CTech::ATTR_NORMAL;
	mOwner = 0;
}

CKnownTech::~CKnownTech()
{
	mOwner = 0;
	mTech = NULL;
}

bool
CKnownTech::initialize(int aOwner, int aTechID, int aAttribute)
{
	CTech
		*Tech = TECH_TABLE->get_by_id(aTechID);
	
	mOwner = aOwner;
	mAttribute = aAttribute;
	mTech = Tech;

	if(Tech == NULL) return false;
	return true;
}

char *
CKnownTech::get_name_with_level()
{
	static CString
		NameWithLevel;
	NameWithLevel.clear();

	NameWithLevel.format("%s(Lv.%d)", get_name(), get_level());

	return (char *)NameWithLevel;
}

int
CKnownTech::get_power()
{
	return (get_level() * mPowerMult);
}

const char*
CKnownTech::html()
{
	static CString
		Buf;

	Buf.clear();

	Buf = "<TABLE BORDER = 0 CELLSPACING = 0>";

	Buf.format( "<TR><TD>ID = %d</TD>\n", mTech->get_id() );
	Buf.format( "<TD COLSPAN = 2>Name = %s</TD></TR>\n", 
			mTech->get_name());
	Buf.format( "<TR><TD>Type = %d</TD>\n<TD>Level = %d</TD>\n"
				"<TD>Attr = %d</TD></TR>\n", 
				mTech->get_type(), mTech->get_level(), mAttribute);
	Buf.format( "<TR><TD COLSPAN = 3 WIDTH = 600>%s</TD></TR>\n", 
				mTech->get_description());

	Buf += "</TABLE>";

	return (char*)Buf;
}
