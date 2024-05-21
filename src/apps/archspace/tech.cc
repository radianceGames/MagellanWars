#include "tech.h"
#include "prerequisite.h"
#include "archspace.h"

static char *mTechTypeName[] = { "INFO", "LIFE", "MATR", "SOCL", "UPG", "SSCH", "AMATR" };
static char *mTechTypeDescription[] = {
	"Information Science",
	"Life Science",
	"Matter-Energy Science",
	"Social Science",
	"Upgrades",
	"Ship Schematics",
	"Advanced Matter-Energy Science"
};
static char *mTechAttributeName[] = { "Basic", "Normal", "Innate" };


CTech::CTech()
{
	mType = 0;
	mID = 0;
	mLevel = 0;
	mProject = 0;
}

CTech::~CTech()
{
	//if (mPrerequisiteList) mPrerequisiteList->clear();
}

void
CTech::load()
{
}

bool
CTech::set_effect(char *aEffectName, int aValue)
{
	if (!strcasecmp( aEffectName, "Environment" ) ||
			!strcasecmp( aEffectName, "Growth" ) ||
			!strcasecmp( aEffectName, "Research" ) ||
			!strcasecmp( aEffectName, "Production" ) ||
			!strcasecmp( aEffectName, "Military" ) ||
			!strcasecmp( aEffectName, "Spy" ) ||
			!strcasecmp( aEffectName, "Commerce" ) ||
			!strcasecmp( aEffectName, "Efficiency" ) ||
			!strcasecmp( aEffectName, "Genius" ) ||
			!strcasecmp( aEffectName, "Diplomacy" ) ||
			!strcasecmp( aEffectName, "Facility Cost" ) )
	{
		mEffect.set(aEffectName, aValue);
		return true;
	}

	return false;
}
CTech*
CTech::get_prereq(CTech* top, CPlayer* aPlayer )
{
	if(!top->evaluate(aPlayer))
	{
		for(int i = 0; i < top->size(); i++)
		{
			if(!(*top)[i].evaluate(aPlayer))
			{
				//hack hack hack this doesnt deal with non tech prereqs, it however doesnt crash with them
				//hack hack hack this doesnt use the enum for RT_TECH
				//if((*top)[i].get_type() == 2)
				//{
					int TechID = (*top)[i].get_argument();
					CTech* next = TECH_TABLE->get_by_id(TechID);
					if(next)
					{
						//we've found a tech that is needed so go research it
						return get_prereq(next, aPlayer);
					}
				//}

			}
		}
	}
	//If we fall through this is the end of the line
	return top;
}



char *
CTech::get_name_with_level()
{
	static CString
		NameWithLevel;
	NameWithLevel.clear();

	NameWithLevel.format("%s(Lv.%d)", get_name(), get_level());

	return (char *)NameWithLevel;
}

char *
CTech::get_type_description()
{
	if (mType < 0 || mType >= TYPE_MAX) return NULL;

	return mTechTypeDescription[mType];
}

char *
CTech::get_type_description(int aType)
{
	if (aType < 0 || aType >= TYPE_MAX) return NULL;

	return mTechTypeDescription[aType];
}

void
CTech::set_type(char *aTypeName)
{
	mType = 0;

	for(int i = 0; i<TYPE_MAX; i++ )
		if(!strcasecmp(mTechTypeName[i], aTypeName))
				mType = i;
}

void
CTech::set_attribute(char *aAttribute)
{
	if (!aAttribute) return;

	mAttribute = ATTR_NORMAL;

	for(int i=0; i<ATTR_MAX; i++)
		if(!strcmp(mTechAttributeName[i], aAttribute))
			mAttribute = i;
}

const char*
CTech::html()
{
	static CString
		Buf;

	Buf.clear();

	Buf = "<TABLE BORDER = 0 CELLSPACING = 0>";

	Buf.format( "<TR><TD>ID = %d</TD>\n", get_id() );
	Buf.format( "<TD COLSPAN = 2>Name = %s</TD></TR>\n", get_name() );
	Buf.format( "<TR><TD>Type = %d</TD>\n<TD>Level = %d</TD>\n"
				"<TD>Attr = %d</TD></TR>\n",
				get_type(), get_level(), mAttribute );
	Buf.format( "<TR><TD COLSPAN = 3 WIDTH = 600>%s</TD></TR>\n",
				get_description() );

	Buf += "</TABLE>";

	return (char*)Buf;
}

