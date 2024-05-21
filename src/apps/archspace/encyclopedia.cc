#include "encyclopedia.h"
#include "define.h"
#include "archspace.h"
#include <libintl.h>
#include "tech.h"
#include "game.h"
#include "race.h"
#include "prerequisite.h"
#include "player.h"

/**
*/
bool
CEncyclopedia::read(const char *aPageName)
{
	if (!ARCHSPACE)
		return false;

	char 
		*FormPath = ARCHSPACE->configuration().get_string(
						"Game", "EncyclopediaFormPath", NULL);
	if (!FormPath) 
	{
		SLOG("could not found a path of encyclopedia form"); 
		return false;
	}

	return load(FormPath, aPageName);
}

/**
*/
bool
CEncyclopedia::write()
{
	char 
		*Path = ARCHSPACE->configuration().get_string(
					"Game", "EncyclopediaPath", NULL);
	
	if (!Path)
	{
		SLOG("could not found a path of encyclopedia"); 
		return false;
	}

	CString
		File;
	
	File = Path;
	File += "/";
	File += (char*)mName;

	return CFileHTML::write((char*)File, &mConversion);
}

/**
*/
bool
CEncyclopediaTechIndexGame::set(int aTechType)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_TECH", GETTEXT("TECH"));
	mConversion.set_value("TYPE_NAME", CTech::get_type_description(aTechType));

	static CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_login/encyclopedia/",
					(char *)CGame::mImageServerURL);

	switch (aTechType)
	{
		case CTech::TYPE_INFORMATION :
			ImageURL += "tech/info.gif";
			break;
		case CTech::TYPE_LIFE:
			ImageURL += "tech/life.gif";
			break;
		case CTech::TYPE_MATTER_ENERGY :
			ImageURL += "tech/ms.gif";
			break;
		case CTech::TYPE_SOCIAL :
			ImageURL += "tech/ss.gif";
			break;
		default :
			ImageURL += "";
	}

	mConversion.set_value("IMAGE_URL", (char *)ImageURL);

	int
		MaxLevel = 0;

	for (int i=0 ; i<TECH_TABLE->length() ; i++)
	{
		CTech *
			Tech = (CTech *)TECH_TABLE->get(i);
		if (Tech->get_type() == aTechType && MaxLevel < Tech->get_level())
		{
			MaxLevel = Tech->get_level();
		}
	}

	static CString
		TechColor,
		TechTable;
	TechColor.clear();
	TechTable.clear();

	switch (aTechType)
	{
		case CTech::TYPE_INFORMATION :
			TechColor = "#838383";
			break;
		case CTech::TYPE_LIFE :
			TechColor = "#4C7C3A";
			break;
		case CTech::TYPE_MATTER_ENERGY :
			TechColor = "#B55C49";
			break;
		case CTech::TYPE_SOCIAL :
			TechColor = "#3A657C";
			break;
		default :
			TechColor = "#838383";
			break;
	}

	TechTable = "<TABLE WIDTH=\"500\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
    
    char *TechTableColor = darker((char *)TechColor);
	for (int i=1 ; i<=MaxLevel ; i++)
	{   
        TechTableColor = darker(TechTableColor);  
		TechTable.format("<TR BGCOLOR=%s>\n",TechTableColor);
		TechTable += "<TD WIDTH=\"94\" CLASS=\"maintext\">&nbsp;<FONT COLOR=\"#1F7498\">";

		TechTable.format("<IMG SRC=\"%s\" WIDTH=\"18\" HEIGHT=\"17\" ALIGN=\"ABSMIDDLE\">\n",
							(char *)ImageURL);

		TechTable.format("<FONT COLOR=\"%s\">%s %d</FONT></FONT></TD>",
							(char *)TechColor, GETTEXT("Level"), i);
		TechTable += "<TD WIDTH=\"406\" CLASS=\"maintext\">&nbsp;";

		bool
			Comma = false;
		for (int j=0 ; j<TECH_TABLE->length() ; j++)
		{
			CTech *
				Tech = (CTech *)TECH_TABLE->get(j);
			if (Tech->get_type() != aTechType) continue;
			if (Tech->get_level() != i) continue;
			if (Comma) TechTable += ", ";
			TechTable.format("<A HREF=\"tech/%d.html\">%s</A>",
								Tech->get_id(), Tech->get_name());
			Comma = true;
		}

		TechTable += "</TD>\n";
		TechTable += "</TR>\n";
	}

	TechTable += "</TABLE>\n";

	mConversion.set_value("TECH_TABLE", (char *)TechTable);

	mName = format("tech_index%d.html", aTechType+1);

	return true;
}

/**
*/             
bool
CEncyclopediaTechPageGame::set(CTech *aTech)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_TECH", GETTEXT("TECH"));
	mConversion.set_value("ID", aTech->get_id());
	mConversion.set_value("NAME", aTech->get_name());

	mConversion.set_value("STRING_TYPE", GETTEXT("Type"));
	mConversion.set_value("TYPE", aTech->get_type_description());

	mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
	mConversion.set_value("LEVEL", aTech->get_level());

    static CString PrerequisiteTextTemp;

    PrerequisiteTextTemp.clear();
    PrerequisiteTextTemp.format("%s",aTech->tech_description());
	mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
	mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

    PrerequisiteTextTemp.clear();
    PrerequisiteTextTemp.format("%s",aTech->project_description());	
	mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
	mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

    PrerequisiteTextTemp.clear();
    PrerequisiteTextTemp.format("%s",aTech->race_description());		
	mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
	mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);
	
	mConversion.set_value("STRING_PROJECT", GETTEXT("Project"));

	static CString
		ProjectName;
	ProjectName.clear();

	CProject *
		Project = (CProject *)PROJECT_TABLE->get_by_id(aTech->get_project());

	if (!Project)
	{
		mConversion.set_value("PROJECT", GETTEXT("NONE"));
	} else
	{
		ProjectName.format("<A HREF=\"../project/%d.html\">%s</A>",
							Project->get_id(), Project->get_name());
		mConversion.set_value("PROJECT", (char *)ProjectName);
	}

	mConversion.set_value("STRING_SPY", GETTEXT("Spy"));

	static CString
		SpyName;
	SpyName.clear();

	CSpy *
		Spy = SPY_TABLE->get_by_id(aTech->get_spy());

	if (!Spy)
	{
		mConversion.set_value("SPY", GETTEXT("NONE"));
	}
	else
	{
		SpyName.format("<A HREF=\"../special_ops/%d.html\">%s</A>",
							Spy->get_id(), Spy->get_name());
		mConversion.set_value("SPY", (char *)SpyName);
	}

	mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("Ship Component"));

	static CString
		ComponentName;
	ComponentName.clear();

	CIntegerList &
		ComponentList = (CIntegerList &)aTech->get_component_list();
	if (!ComponentList.length())
	{
		mConversion.set_value("SHIP_COMPONENT", GETTEXT("NONE"));
	} else
	{
		bool
			Comma = false;
		for (int i=0 ; i<ComponentList.length() ; i++)
		{
			CComponent *
				Component = COMPONENT_TABLE->get_by_id((int)ComponentList.get(i));
			if (Comma) ComponentName += ", ";
			ComponentName.format("<A HREF=\"../component/%d.html\">%s</A>",
									Component->get_id(), Component->get_name());
			Comma = true;
		}

		mConversion.set_value("SHIP_COMPONENT", (char *)ComponentName);
	}

	mConversion.set_value("STRING_CONTROL_MODEL_EFFECT", GETTEXT("Control Model Effect"));

	static CString
		Effect;
	Effect.clear();

	CControlModel &
		ControlModel = (CControlModel &)aTech->get_effect();
	bool
		AnyEffect = false;
	for (int i=CControlModel::CM_ENVIRONMENT ; i<CControlModel::CM_MAX ; i++)
	{
		int
			CM = ControlModel.get_value(i);
		if (CM)
		{
			if (AnyEffect) Effect += ", ";
			Effect.format("%s : %d", CControlModel::get_cm_description(i), CM);
			AnyEffect = true;
		}
	}
	if (!AnyEffect) Effect = GETTEXT("NONE");

	mConversion.set_value("CONTROL_MODEL_EFFECT", (char *)Effect);

	mConversion.set_value("DESCRIPTION", aTech->get_description());

	mConversion.set_value("INDEX_URL",
					(char *)format("../tech_index%d.html", aTech->get_type() + 1));

	mName = format("tech/%d.html", aTech->get_id());

	return true;
}

/**
*/
bool
CEncyclopediaRaceIndexGame::set(CRaceTable *aRaceTable)
{
	CString
		Table;

	Table = "<TABLE>\n"
			"<TH>Name</TH><TH>Society</TH>\n";

	for(int i=0; i<aRaceTable->length(); i++)
	{
		Table += "<TR>";

		CRace *
			Race = (CRace *)aRaceTable->get(i);

		CString
			Link;

		Link.format("%d.html", Race->get_id());

		Table.format("<TD><A HREF=\"%s\">%s</A></TD><TD>%s</TD>",
				(char*)Link, Race->get_name(), 
				Race->get_society_name());

		Table += "</TR>\n";
	}
			
	Table += "</TABLE>\n";

	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("RACE_TABLE", (char*)Table);
	return true;
}

/**
*/
bool
CEncyclopediaRacePageGame::set(CRace* aRace)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_RACE", GETTEXT("RACE"));

	static CString
		BorderColor,
		BGColor;
	BorderColor.clear();
	BGColor.clear();

	switch (aRace->get_id())
	{
		case CRace::RACE_HUMAN :
			BorderColor = "#262C4D";
			BGColor = "#131624";
			break;

		case CRace::RACE_TARGOID :
			BorderColor = "#264D35";
			BGColor = "#13241E";
			break;

		case CRace::RACE_BUCKANEER :
			BorderColor = "#4E3F25";
			BGColor = "#251E12";
			break;

		case CRace::RACE_TECANOID :
			BorderColor = "#25404E";
			BGColor = "#131C24";
			break;

		case CRace::RACE_EVINTOS :
			BorderColor = "#37383C";
			BGColor = "#1C1C1C";
			break;

		case CRace::RACE_AGERUS :
			BorderColor = "#4D2826";
			BGColor = "#241313";
			break;

		case CRace::RACE_BOSALIAN :
			BorderColor = "#4F4924";
			BGColor = "#201F11";
			break;

		case CRace::RACE_XELOSS :
			BorderColor = "#4E252C";
			BGColor = "#241316";
			break;

		case CRace::RACE_XERUSIAN :
			BorderColor = "#274B47";
			BGColor = "#101F1F";
			break;

		case CRace::RACE_XESPERADOS :
			BorderColor = "#354B27";
			BGColor = "#1A2314";
			break;
		default :
			BorderColor = "#262C4D";
			BGColor = "#131624";
			break;
	}

	mConversion.set_value("STRING_RACE", GETTEXT("RACE"));

	mConversion.set_value("BORDER_COLOR", (char *)BorderColor);
	mConversion.set_value("BG_COLOR", (char *)BGColor);

	mConversion.set_value("STRING_SOCIETY", GETTEXT("Society"));
	mConversion.set_value("SOCIETY", aRace->get_society_name());

	mConversion.set_value("STRING_INNATE_TECH", GETTEXT("Innate Tech"));

	static CString
		InnateTechName;
		InnateTechName.clear();
	bool
		Comma = false;
	for (int i=0 ; i<aRace->get_innate_tech_count() ; i++)
	{
		int
			InnateTechID = aRace->get_innate_tech(i);
		CTech *
			InnateTech = TECH_TABLE->get_by_id(InnateTechID);
		if (Comma) InnateTechName += ", ";
		InnateTechName.format("<A HREF=\"../tech/%d.html\">%s</A>",
								InnateTech->get_id(), InnateTech->get_name());
		Comma = true;
	}

	if (!InnateTechName.length())
	{
		mConversion.set_value("INNATE_TECH", GETTEXT("NONE"));
	} else
	{
		mConversion.set_value("INNATE_TECH", (char *)InnateTechName);
	}

	mConversion.set_value("STRING_HOME_ENVIRONMENT", GETTEXT("Home Environment"));

	mConversion.set_value("STRING_GRAVITY", GETTEXT("Gravity"));
	mConversion.set_value("GRAVITY", (char *)format("%1$.1f G", aRace->get_gravity()));

	mConversion.set_value("STRING_TEMPERATURE", GETTEXT("Temperature"));
	mConversion.set_value("TEMPERATURE", (char *)format("%d K", aRace->get_temperature()));

	mConversion.set_value("STRING_ATMOSPHERE", GETTEXT("Atmosphere"));

	static CString
		Atmosphere,
		AtmosphereName;
	Atmosphere.clear();
	AtmosphereName.clear();

	Atmosphere = aRace->get_atmosphere();

	bool
		NextLine = false;
	for (int i=CPlanet::GAS_BEGIN; i<CPlanet::GAS_MAX ; i++)
	{
		if ('0' < Atmosphere[i] && Atmosphere[i] < '6')
		{
			if (NextLine) AtmosphereName += "<BR>\n";
			AtmosphereName.format("%s %d",
									CPlanet::get_gas_description((CPlanet::EGasType)i),
									Atmosphere[i]-'0');
			NextLine = true;
		}
	}
	if (!AtmosphereName.length())
	{
		mConversion.set_value("ATMOSPHERE", "-");
	} else
	{
		mConversion.set_value("ATMOSPHERE", (char *)AtmosphereName);
	}

	mConversion.set_value("STRING_CONTROL_MODEL", GETTEXT("Control Model"));

	CControlModel &
		ControlModel = (CControlModel &)aRace->get_control_model();

	static CString
		ControlModelTable;
	ControlModelTable.clear();

	ControlModelTable = "<TABLE WIDTH=\"248\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
	for (int i=CControlModel::CM_ENVIRONMENT ; i<CControlModel::CM_MAX ; i++)
	{
		int
			CM = ControlModel.get_value(i);
		if (CM)
		{
			ControlModelTable += "<TR>\n";
			ControlModelTable += "<TD CLASS=\"tabletxt\" WIDTH=\"107\">";
			ControlModelTable.format("<FONT COLOR=\"666666\">%s</FONT>",
										CControlModel::get_cm_description(i));
			ControlModelTable += "</TD>";
			ControlModelTable.format("<TD CLASS=\"tabletxt\" COLSPAN=\"3\">%d</TD>\n", CM);
			ControlModelTable += "</TR>\n";
		}
	}
	ControlModelTable += "</TABLE>\n";

	mConversion.set_value("CONTROL_MODEL", (char *)ControlModelTable);

	mConversion.set_value("STRING_ABILITY", GETTEXT("Ability"));

	CCommandSet &
		AbilitySet = (CCommandSet &)aRace->get_ability();

	static CString
		AbilityName;
	AbilityName.clear();

	for (int i=ABILITY_BEGIN ; i<ABILITY_MAX ; i++)
	{
		if (AbilitySet.has(i))
			AbilityName.format ("&nbsp;%s<br>", index_to_ability_name(i));
	}

	if (!AbilityName.length())
	{
		mConversion.set_value("ABILITY", GETTEXT("NONE"));
	} else
	{
		mConversion.set_value("ABILITY", (char *)AbilityName);
	}

	mConversion.set_value("RACE_ID", aRace->get_id());

	mConversion.set_value("DESCRIPTION", aRace->get_description());

	mName = format("race/%d.html", aRace->get_id());

	return true;
}

/**
*/
bool
CEncyclopediaProjectIndexGame::set(int aProjectType)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_PROJECT", GETTEXT("PROJECT"));

	static CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_login/encyclopedia/",
					(char *)CGame::mImageServerURL);

	switch (aProjectType)
	{
		case CProject::TYPE_PLANET :
			ImageURL += "project/project_planet.gif";
			break;

		case CProject::TYPE_FIXED :
			ImageURL += "project/project_domain.gif";
			break;
		case CProject::TYPE_COUNCIL :
			ImageURL += "project/project_council.gif";
			break;
		case CProject::TYPE_SECRET :
			ImageURL += "project/project_secret.gif";
			break;

		default :
			ImageURL += "project/project_secret.gif";
			break;
	}

	mConversion.set_value("IMAGE_URL", (char *)ImageURL);

	if (aProjectType >= TYPE_BEGIN && aProjectType < TYPE_MAX)
	{
		static CString
			ProjectTypeName;
		ProjectTypeName.clear();

		ProjectTypeName.format(GETTEXT("%1$s Project(s)"),
								CProject::get_type_name(aProjectType));

		mConversion.set_value("TYPE_NAME", (char *)ProjectTypeName);
	}
	else
	{
		mConversion.set_value("TYPE_NAME", GETTEXT("Unknown"));
	}

	static CString
		ProjectIndexTable;
	ProjectIndexTable.clear();

	ProjectIndexTable = "<TABLE WIDTH=\"380\" BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"1\">\n";
	for (int i=0 ; i<PROJECT_TABLE->length() ; i++)
	{
		CProject *
			Project = (CProject *)PROJECT_TABLE->get(i);
		if (Project->get_type() != aProjectType) continue;

		ProjectIndexTable += "<TR>\n";

		ProjectIndexTable += "<TD WIDTH=\"380\" CLASS=\"maintext\">";
		ProjectIndexTable.format("<A HREF=\"project/%d.html\">%s</A>",
									Project->get_id(), Project->get_name());

		ProjectIndexTable += "</TR>\n";
	}

	ProjectIndexTable += "</TABLE>\n";

	mConversion.set_value("PROJECT_INDEX_TABLE", (char *)ProjectIndexTable);

	mName = format("project_index%d.html", aProjectType+1);

	return true;
}

/**
*/
bool
CEncyclopediaProjectPageGame::set(CProject *aProject)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_PROJECT", GETTEXT("PROJECT"));

	mConversion.set_value("PROJECT_NAME", aProject->get_name());
	mConversion.set_value("PROJECT_ID", aProject->get_id());

	mConversion.set_value("STRING_TYPE", GETTEXT("Type"));
	mConversion.set_value("TYPE", aProject->get_type_name());

	mConversion.set_value("STRING_COST", GETTEXT("Cost"));
	mConversion.set_value("COST",
							(char *)format("%s PP", dec2unit(aProject->get_cost())));

	static CString PrerequisiteTextTemp;

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s",aProject->tech_description());
	mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
	mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s",aProject->project_description());	
	mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
	mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s",aProject->race_description());		
	mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
	mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);

	mConversion.set_value("STRING_EFFECT", GETTEXT("CM Effect"));
	mConversion.set_value("STRING_OTHER_EFFECT", GETTEXT("Other Effect"));

	static CString
		EffectList;
	EffectList.clear();

	CControlModel
		Effect = (CControlModel &)aProject->get_effect();

	bool
		AnyEffect = false;

	for (int i=CControlModel::CM_ENVIRONMENT ; i<CControlModel::CM_MAX ; i++)
	{
		if (Effect.get_value(i))
		{
			if (AnyEffect) EffectList += "<BR>";
			EffectList.format("&nbsp;%s ", CControlModel::get_cm_description(i));
			if (Effect.get_value(i) > 0) EffectList += "+";
			EffectList.format("%d",	Effect.get_value(i));
			AnyEffect = true;
		}
	}

	if (AnyEffect)
	{
		mConversion.set_value("EFFECT_LIST", (char *)EffectList);
	} else
	{
		mConversion.set_value("EFFECT_LIST", GETTEXT("&nbsp;NONE"));
	}
	
        CPlayerEffectList *PlayerEffectList = aProject->get_effect_list();
	if (PlayerEffectList->length() <= 0)
	{
		mConversion.set_value("OTHER_EFFECT_LIST", GETTEXT("&nbsp;NONE"));	    
	}
	else
	{
	    static CString OtherEffectList;
	    OtherEffectList.clear();
	    int j;
	    for (j=0; j < PlayerEffectList->length()-1; j++)
	    {
	        CPlayerEffect *PlayerEffect = (CPlayerEffect *)PlayerEffectList->get(j);
	        if (!PlayerEffect)
 	            continue;
          	
          	OtherEffectList.format("&nbsp;%s: ", CPlayerEffect::get_type_name(PlayerEffect->get_type()));
                if ( PlayerEffect->get_type() == CPlayerEffect::PA_GAIN_ABILITY || PlayerEffect->get_type() == CPlayerEffect::PA_LOSE_ABILITY )
                {
                    OtherEffectList.format("%s", index_to_ability_name(PlayerEffect->get_argument1()));
                }    
                else
                {
                   if (PlayerEffect->get_argument1() > 0) OtherEffectList += "+";
                   OtherEffectList.format("%s", dec2unit(PlayerEffect->get_argument1()));
          	   if (PlayerEffect->get_apply() != CPlayerEffect::APPLY_ABSOLUTE) 
           	            OtherEffectList += "%";
   	        }    
                OtherEffectList += "<BR>";
	    }    
            CPlayerEffect *PlayerEffect = (CPlayerEffect *)PlayerEffectList->get(j);
            if (PlayerEffect) 
            {
                OtherEffectList.format("&nbsp;%s: ", CPlayerEffect::get_type_name(PlayerEffect->get_type()));
                if ( PlayerEffect->get_type() == CPlayerEffect::PA_GAIN_ABILITY || PlayerEffect->get_type() == CPlayerEffect::PA_LOSE_ABILITY )
                {
                   OtherEffectList.format("%s", index_to_ability_name(PlayerEffect->get_argument1()));
                }    
                else
                {
                   if (PlayerEffect->get_argument1() > 0) OtherEffectList += "+";
                   OtherEffectList.format("%s", dec2unit(PlayerEffect->get_argument1()));
                   if (PlayerEffect->get_apply() != CPlayerEffect::APPLY_ABSOLUTE) OtherEffectList += "%";
                }    
            }
	    mConversion.set_value("OTHER_EFFECT_LIST", (char *)OtherEffectList);	
        }    
	    
	mConversion.set_value("DESCRIPTION", aProject->get_description());

	mConversion.set_value("INDEX_URL",
							(char *)format("../project_index%d.html",
											aProject->get_type()+1));

	mName = format("project/%d.html", aProject->get_id());

	return true;
}

/**
*/
bool
CEncyclopediaComponentIndexGame::set(int aComponentCategory)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

	static CString
		ImageURL,
		TypeName;
	ImageURL.clear();
	ImageURL.format("%s/image/as_login/encyclopedia/",
					(char *)CGame::mImageServerURL);
	TypeName.clear();

	switch (aComponentCategory)
	{
		case CComponent::CC_ARMOR :
			ImageURL += "ship_component/armor.gif";
			break;

		case CComponent::CC_COMPUTER :
			ImageURL += "ship_component/computer.gif";
			break;

		case CComponent::CC_SHIELD :
			ImageURL += "ship_component/shield.gif";
			break;

		case CComponent::CC_ENGINE :
			ImageURL += "ship_component/engine.gif";
			break;

		case CComponent::CC_DEVICE :
			ImageURL += "ship_component/device.gif";
			break;

		case CComponent::CC_WEAPON :
			ImageURL += "ship_component/weapon.gif";
			break;

		default :
			break;
	}

	mConversion.set_value("COMPONENT_TYPE_NAME",
							CComponent::get_category_name_normal(aComponentCategory));

	mConversion.set_value("IMAGE_URL", (char *)ImageURL);

	int
		MaxLevel = 0;

	for (int i=0 ; i<COMPONENT_TABLE->length() ; i++)
	{
		CComponent *
			Component = (CComponent *)COMPONENT_TABLE->get(i);
		if (Component->get_category() == aComponentCategory &&
			MaxLevel < Component->get_level())
		{
			MaxLevel = Component->get_level();
		}
	}

	static CString
		ComponentTable;
	ComponentTable.clear();

	ComponentTable = "<TABLE WIDTH=\"500\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";

    char *Color;
	for (int i=1 ; i<=MaxLevel ; i++)
	{
        switch(i)
        {
          case 1: Color = "#0F0F0F";
                  break;
          case 2: Color = "#171717";
                  break;
          case 3: Color = "#262626";
                  break;
          case 4: Color = "#2F2F2F";
                  break;
          case 5: Color = "#3A3A3A";
                  break;
          default:Color = "#3A3A3A";
                  break;        
        }
        
		ComponentTable.format("<TR BGCOLOR=%s>\n",Color);
		ComponentTable.format("<TD WIDTH=\"61\" CLASS=\"maintext\">&nbsp;%s %d</TD>",
							GETTEXT("Level"), i);

		ComponentTable += "<TD WIDTH=\"439\" CLASS=\"maintext\">&nbsp;";

		bool
			Comma = false;
		for (int j=0 ; j<COMPONENT_TABLE->length() ; j++)
		{
			CComponent *
				Component = (CComponent *)COMPONENT_TABLE->get(j);
			if (Component->get_category() != aComponentCategory) continue;
			if (Component->get_level() != i) continue;
			if (Comma) ComponentTable += ", ";
			ComponentTable.format("<A HREF=\"component/%d.html\">%s</A>",
								Component->get_id(), Component->get_name());
			Comma = true;
		}

		ComponentTable += "</TD>\n";
		ComponentTable += "</TR>\n";
	}

	ComponentTable += "</TABLE>\n";

	mConversion.set_value("COMPONENT_TABLE", (char *)ComponentTable);

	mName = format("component_index%d.html", aComponentCategory+1);

	return true;
}

/**
*/
bool
CEncyclopediaComponentPageGame::set(CComponent *aComponent)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	CString
		EffectBuf;

	for( int i = 0; i < aComponent->effect_length(); i++ ){
		CFleetEffect
			*Effect = aComponent->get_effect(i);

		EffectBuf += Effect->print_html();
	}

	if( EffectBuf.length() == 0 )
		EffectBuf = "NONE";

	switch (aComponent->get_category())
	{
		case CComponent::CC_ARMOR :
		{
			CArmor *
				Armor = (CArmor *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Armor->get_name());
			mConversion.set_value("COMPONENT_ID", Armor->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Armor->get_category_name_normal());

			mConversion.set_value("STRING_TYPE", GETTEXT("Type"));
			mConversion.set_value("TYPE", Armor->get_armor_type_name_normal());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Armor->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Armor->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Armor->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);


			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Armor->get_level());

			mConversion.set_value("STRING_DEFENSE_RATE", GETTEXT("Defense Rating"));
			mConversion.set_value("DEFENSE_RATE", Armor->get_defense_rate());

			mConversion.set_value("STRING_HP_MULTIPLIER", GETTEXT("HP Multiplier"));
			mConversion.set_value("HP_MULTIPLIER",
					(char *)format("%.2f", Armor->get_hp_multiplier()));

			mConversion.set_value("STRING_EFFECT", GETTEXT("Effect"));

			mConversion.set_value("EFFECT", (char*)EffectBuf);

			mConversion.set_value("DESCRIPTION", Armor->get_description());

			mConversion.set_value("CATEGORY_ID", Armor->get_category()+1);

			break;
		}

		case CComponent::CC_COMPUTER :
		{
			CComputer *
				Computer = (CComputer *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Computer->get_name());
			mConversion.set_value("COMPONENT_ID", Computer->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Computer->get_category_name_normal());

			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Computer->get_level());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Computer->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Computer->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Computer->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);


			mConversion.set_value("STRING_ATTACK_RATE", GETTEXT("Attack Rating"));
			mConversion.set_value("ATTACK_RATE", Computer->get_attacking_rate());

			mConversion.set_value("STRING_DEFENSE_RATE", GETTEXT("Defense Rating"));
			mConversion.set_value("DEFENSE_RATE", Computer->get_defense_rate());

			mConversion.set_value("DESCRIPTION", Computer->get_description());

			mConversion.set_value("CATEGORY_ID", Computer->get_category()+1);

			break;
		}

		case CComponent::CC_SHIELD :
		{
			CShield *
				Shield = (CShield *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Shield->get_name());
			mConversion.set_value("COMPONENT_ID", Shield->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Shield->get_category_name_normal());

			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Shield->get_level());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Shield->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Shield->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Shield->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);


			mConversion.set_value("STRING_DEFLECTION_SOLIDITY",
									GETTEXT("Deflection Solidity"));
			mConversion.set_value("DEFLECTION_SOLIDITY", Shield->get_deflection_solidity());

			mConversion.set_value("STRING_RECHARGE_RATE", GETTEXT("Recharge Rate"));
			mConversion.set_value("RECHARGE_RATE",
									(char *)format("From %d to %d (in proportion to the ship class)",
													Shield->get_recharge_rate(0),
													Shield->get_recharge_rate(MAX_SHIP_CLASS - 1)));

			mConversion.set_value("STRING_SHIELD_STRENGTH", GETTEXT("Shield Strength"));
			mConversion.set_value("SHIELD_STRENGTH",
									(char *)format("From %d to %d (in proportion to the ship class)",
													Shield->get_strength(0),
													Shield->get_strength(MAX_SHIP_CLASS - 1)));

			mConversion.set_value("DESCRIPTION", Shield->get_description());

			mConversion.set_value("CATEGORY_ID", Shield->get_category()+1);

			break;
		}

		case CComponent::CC_ENGINE :
		{
			CEngine *
				Engine = (CEngine *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Engine->get_name());
			mConversion.set_value("COMPONENT_ID", Engine->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Engine->get_category_name_normal());

			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Engine->get_level());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Engine->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Engine->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Engine->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);

			mConversion.set_value("STRING_BATTLE_SPEED", GETTEXT("Battle Speed"));
			mConversion.set_value("BATTLE_SPEED",
									(char *)format("From %d to %d (in proportion to the ship class)",
													Engine->get_battle_speed(0),
													Engine->get_battle_speed(MAX_SHIP_CLASS - 1)));

			mConversion.set_value("STRING_BATTLE_MOBILITY", GETTEXT("Battle Mobility"));
			mConversion.set_value("BATTLE_MOBILITY",
									(char *)format("From %d to %d (in proportion to the ship class)",
													Engine->get_battle_mobility(0),
													Engine->get_battle_mobility(MAX_SHIP_CLASS - 1)));

			mConversion.set_value("DESCRIPTION", Engine->get_description());

			mConversion.set_value("CATEGORY_ID", Engine->get_category()+1);
			break;
		}

		case CComponent::CC_DEVICE :
		{
			CDevice *
				Device = (CDevice *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Device->get_name());
			mConversion.set_value("COMPONENT_ID", Device->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Device->get_category_name_normal());

			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Device->get_level());

			mConversion.set_value("STRING_MIN__CLASS", GETTEXT("Min. Class"));
			mConversion.set_value("MIN__CLASS", Device->get_min_class());

			mConversion.set_value("STRING_MAX__CLASS", GETTEXT("Max. Class"));
			mConversion.set_value("MAX__CLASS", Device->get_max_class());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Device->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Device->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Device->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);

			mConversion.set_value("STRING_EFFECT", GETTEXT("Effect"));

			mConversion.set_value("EFFECT", (char*)EffectBuf);

			mConversion.set_value("DESCRIPTION", Device->get_description());

			mConversion.set_value("CATEGORY_ID", Device->get_category()+1);

			break;
		}

		case CComponent::CC_WEAPON :
		{
			CWeapon *
				Weapon = (CWeapon *)aComponent;

			mConversion.set_value("STRING_SHIP_COMPONENT", GETTEXT("SHIP COMPONENT"));

			mConversion.set_value("COMPONENT_NAME", Weapon->get_name());
			mConversion.set_value("COMPONENT_ID", Weapon->get_id());

			mConversion.set_value("STRING_CATEGORY", GETTEXT("Category"));
			mConversion.set_value("CATEGORY", Weapon->get_category_name_normal());

			mConversion.set_value("STRING_TYPE", GETTEXT("Type"));
			mConversion.set_value("TYPE", Weapon->get_weapon_type_name_normal());

			static CString PrerequisiteTextTemp;

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Weapon->tech_description());
			mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
			mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Weapon->project_description());	
			mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
			mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

			PrerequisiteTextTemp.clear();
			PrerequisiteTextTemp.format("%s",Weapon->race_description());		
			mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
			mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);

			mConversion.set_value("STRING_LEVEL", GETTEXT("Level"));
			mConversion.set_value("LEVEL", Weapon->get_level());

			mConversion.set_value("STRING_EFFECT", GETTEXT("Effect"));
			mConversion.set_value("EFFECT", (char*)EffectBuf);

			mConversion.set_value("STRING_ATTACK_RATE", GETTEXT("Attack Rating"));
			mConversion.set_value("ATTACK_RATE", Weapon->get_attacking_rate());

			mConversion.set_value("STRING_DICE_ROLLED", GETTEXT("Dice Rolled"));
			mConversion.set_value("DICE_ROLLED", Weapon->get_damage_roll());

			mConversion.set_value("STRING_DICE_TYPE", GETTEXT("Dice Type"));
			mConversion.set_value("DICE_TYPE", Weapon->get_damage_dice());

			mConversion.set_value("STRING_COOLING_TIME", GETTEXT("Cooling Time"));
			mConversion.set_value("COOLING_TIME", Weapon->get_cooling_time());

			mConversion.set_value("STRING_ANGLE_OF_FIRE", GETTEXT("Angle of Fire"));
			mConversion.set_value("ANGLE_OF_FIRE", Weapon->get_angle_of_fire());

			mConversion.set_value("STRING_WEAPON_SPEED", GETTEXT("Weapon Speed"));
			mConversion.set_value("WEAPON_SPEED", Weapon->get_speed());

			mConversion.set_value("STRING_ATTACK_RANGE", GETTEXT("Attack Range"));
			mConversion.set_value("ATTACK_RANGE", Weapon->get_range());

			mConversion.set_value("STRING_SPACE", GETTEXT("Space"));
			mConversion.set_value("SPACE", Weapon->get_space());

			mConversion.set_value("DESCRIPTION", Weapon->get_description());

			mConversion.set_value("CATEGORY_ID", Weapon->get_category()+1);

			break;
		}

		default :
			break;
	}

	mName = format("component/%d.html", aComponent->get_id());

	return true;
}

/**
*/
bool
CEncyclopediaShipIndexGame::set(CShipSizeTable *aShipTable)
{
	CString
		Table;

	Table = "<TABLE>\n"
			"<TH>Name</TH><TH>Class</TH>\n";

	for(int i=0; i<aShipTable->length(); i++)
	{
		Table += "<TR>";

		CShipSize *
			ship = (CShipSize *)aShipTable->get(i);

		CString
			Link;

		Link.format("%d.html", ship->get_id());

		Table.format("<TD><A HREF=\"%s\">%s</A></TD><TD>%d</TD>",
				(char*)Link, ship->get_name(), ship->get_class() );

		Table += "</TR>\n";
	}

	Table += "</TABLE>\n";

	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("SHIP_TABLE", (char*)Table);

	return true;
}

/**
*/
bool
CEncyclopediaShipPageGame::set(CShipSize *aShip)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_SHIP_CLASS", GETTEXT("SHIP CLASS"));

	static CString
		ShipImageURL;
	ShipImageURL.clear();
	ShipImageURL.format("%s/image/as_login/encyclopedia/",
						(char *)CGame::mImageServerURL);
	ShipImageURL.format("ship_class/%d.gif", aShip->get_id());

	mConversion.set_value("SHIP_IMAGE", (char *)ShipImageURL);

	mConversion.set_value("STRING_CLASS", GETTEXT("Class"));
	mConversion.set_value("CLASS", aShip->get_id() - 4000);

	mConversion.set_value("STRING_COST", GETTEXT("Cost"));
	mConversion.set_value("COST", (char *)format("%s PP", dec2unit(aShip->get_cost())));

	mConversion.set_value("STRING_WEAPON_SLOT", GETTEXT("Weapon Slot"));
	mConversion.set_value("WEAPON_SLOT", aShip->get_weapon());

	mConversion.set_value("STRING_DEVICE_SLOT", GETTEXT("Device Slot"));
	mConversion.set_value("DEVICE_SLOT", aShip->get_device());

	mConversion.set_value("STRING_SPACE", GETTEXT("Space"));
	mConversion.set_value("SPACE", dec2unit(aShip->get_space()));

	mConversion.set_value("STRING_SPACE_PER_SLOT", GETTEXT("Space per Slot"));
	mConversion.set_value("SPACE_PER_SLOT", dec2unit(aShip->get_slot()));

	mConversion.set_value("DESCRIPTION", aShip->get_description());

	mName = format("ship/%d.html", aShip->get_id());

	return true;
}

/**
*/
bool
CEncyclopediaSpyIndexGame::set(CSpyTable *aSpyTable)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_SPECIAL_OPS_", GETTEXT("SPECIAL OPS."));

	mConversion.set_value("SPY_INTRODUCE_MESSAGE",
			GETTEXT("There are four levels of spy operations in the world of Archspace."));

	static CString
		Acceptable,
		Ordinary,
		Hostile,
		Atrocious;
	Acceptable.clear();
	Ordinary.clear();
	Hostile.clear();
	Atrocious.clear();

	for (int i=0 ; i<aSpyTable->length() ; i++)
	{
		CSpy *
			Spy = (CSpy *)aSpyTable->get(i);

		switch (Spy->get_type())
		{
			case CSpy::TYPE_ACCEPTABLE :
				Acceptable.format("<A HREF=\"special_ops/%d.html\">%s</A>&nbsp;&nbsp;",
									Spy->get_id(), Spy->get_name());
				Acceptable += "<BR>\n";

				break;

			case CSpy::TYPE_ORDINARY :
				Ordinary.format("<A HREF=\"special_ops/%d.html\">%s</A>&nbsp;&nbsp;",
									Spy->get_id(), Spy->get_name());
				Ordinary += "<BR>\n";

				break;

			case CSpy::TYPE_HOSTILE :
				Hostile.format("<A HREF=\"special_ops/%d.html\">%s</A>&nbsp;&nbsp;",
									Spy->get_id(), Spy->get_name());
				Hostile += "<BR>\n";

				break;

			case CSpy::TYPE_ATROCIOUS :
				Atrocious.format("<A HREF=\"special_ops/%d.html\">%s</A>&nbsp;&nbsp;",
									Spy->get_id(), Spy->get_name());
				Atrocious += "<BR>\n";

				break;

			default :
				break;
		}
	}

	mConversion.set_value("ACCEPTABLE_SPY_LIST", (char *)Acceptable);
	mConversion.set_value("ORDINARY_SPY_LIST", (char *)Ordinary);
	mConversion.set_value("HOSTILE_SPY_LIST", (char *)Hostile);
	mConversion.set_value("ATROCIOUS_SPY_LIST", (char *)Atrocious);

	mName = "special_ops.html";

	return true;
}

/**
*/
bool
CEncyclopediaSpyPageGame::set(CSpy *aSpy)
{
	mConversion.remove_all();

	mConversion.set_value("CHAR_SET", GAME->get_charset());

	mConversion.set_value("IMAGE_SERVER_URL", (char *)CGame::mImageServerURL);

	mConversion.set_value("STRING_SPECIAL_OPERATIONS", GETTEXT("SPECIAL OPERATIONS"));

	mConversion.set_value("SPY_NAME", aSpy->get_name());
	mConversion.set_value("SPY_ID", aSpy->get_id());

	mConversion.set_value("STRING_DIFFICULTY", GETTEXT("Difficulty"));
	mConversion.set_value("DIFFICULTY", aSpy->get_difficulty());

	mConversion.set_value("STRING_COST", GETTEXT("Cost"));
	mConversion.set_value("COST",
							(char *)format("%s PP", dec2unit(aSpy->get_cost())));

	mConversion.set_value("STRING_REQUIRED_TECH", GETTEXT("Required Tech"));

    static CString PrerequisiteTextTemp;

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s", aSpy->tech_description());
	mConversion.set_value("STRING_PREREQUISITE_T_", GETTEXT("Technology Prerequisite(s)"));
	mConversion.set_value("TECH_PREREQUISITE", (char *)PrerequisiteTextTemp);

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s", aSpy->project_description());	
	mConversion.set_value("STRING_PREREQUISITE_P_", GETTEXT("Project Prerequisite(s)"));
	mConversion.set_value("PROJECT_PREREQUISITE", (char *)PrerequisiteTextTemp);

	PrerequisiteTextTemp.clear();
	PrerequisiteTextTemp.format("%s", aSpy->race_description());		
	mConversion.set_value("STRING_PREREQUISITE_R_", GETTEXT("Race Prerequisite(s)"));
	mConversion.set_value("RACE_PREREQUISITE", (char *)PrerequisiteTextTemp);

	mConversion.set_value("STRING_HOSTILITY_LEVEL", GETTEXT("Hostility Level"));
	mConversion.set_value("HOSTILITY_LEVEL", aSpy->get_type_name());

	mConversion.set_value("DESCRIPTION", aSpy->get_description());

	mName = format("special_ops/%d.html", aSpy->get_id());

	return true;
}

