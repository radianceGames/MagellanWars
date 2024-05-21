#include "script.h"
#include "define.h"
#include "tech.h"
#include <cstdlib>
#include "race.h"
#include "project.h"
#include "archspace.h"
#include "game.h"
#include "prerequisite.h"
#include "component.h"
#include "ctrl.model.h"

bool
CTechScript::get(CTechTable *aTechTable)
{
	SLOG("start script no:%d", mRoot.length());

	int i=0;
	TSomething Tech;

	while((Tech = get_array(i++)))
	{
		CTech
			*TmpTech = new CTech;

		//SLOG("TechName:<%s>", get_name(Tech));

		TmpTech->set_name(get_name(Tech));

		TSomething
			Something;

		Something = get_section("Number", Tech);
		if (Something) {
			TmpTech->set_id(atoi(get_data(Something)));
		} else {
			SLOG("WRONG TECH FILE at %s", TmpTech->get_name());
			return false;
		}

		Something = get_section("Description", Tech);
		if (Something) {
			TmpTech->set_description(get_data(Something));
		} else {
			SLOG("WRONG TECH FILE at %s(%d)",
					TmpTech->get_name(), TmpTech->get_id() );
			return false;
		}

		Something = get_section("Type", Tech);
		if (Something) {
			TmpTech->set_type(get_data(Something));
		} else {
			SLOG("WRONG TECH FILE at %s(%d)",
					TmpTech->get_name(), TmpTech->get_id() );
			return false;
		}

		Something = get_section("Level", Tech);
		if (Something) {
			TmpTech->set_level(atoi(get_data(Something)));
		} else {
			SLOG("WRONG TECH FILE at %s(%d)",
					TmpTech->get_name(), TmpTech->get_id() );
			return false;
		}

		Something = get_section("Project", Tech);
		if (Something) {
			TmpTech->set_project(atoi(get_data(Something)));
		}

		Something = get_section("Spy", Tech);
		if (Something) {
			TmpTech->set_spy(atoi(get_data(Something)));
		}

		Something = get_section("Attr", Tech);
		if (Something) {
			TmpTech->set_attribute(get_data(Something));
		} else {
			SLOG("WRONG TECH FILE at %s(%d)",
					TmpTech->get_name(), TmpTech->get_id());
			return false;
		}

		Something = get_section("Prerequisite", Tech);
		if (Something)
		{
			load_prerequisites(Something, (CPrerequisiteList*)TmpTech);
		}

		Something = get_section("Component", Tech);
		if (Something)
		{
			TSomething
				Component;
			int
				i = 0;

			while( (Component = get_array(i, Something)) ){
				TmpTech->add_component(atoi(get_data(Component)));
				i++;
			}
		}

		TSomething
			Effect;

		Effect = get_section("Effect", Tech);

		if (Effect)
		{
			TSomething
				EffectData;

			int i = 0;
			while ((EffectData = get_array(i++, Effect)))
			{
				const char* Effect = get_name(EffectData);
				if (!strcasecmp(Effect, "Environment") ||
					!strcasecmp(Effect, "Growth") ||
					!strcasecmp(Effect, "Research") ||
					!strcasecmp(Effect, "Production") ||
					!strcasecmp(Effect, "Military") ||
					!strcasecmp(Effect, "Spy") ||
					!strcasecmp(Effect, "Commerce") ||
					!strcasecmp(Effect, "Efficiency") ||
					!strcasecmp(Effect, "Genius") ||
					!strcasecmp(Effect, "Diplomacy") ||
					!strcasecmp(Effect, "Facility Cost"))
					TmpTech->set_effect(get_name(EffectData),
					atoi(get_data(EffectData)));
				else
				{
					SLOG("error found in effect section : %s(%d)",
					TmpTech->get_name(), TmpTech->get_id());
					return false;
				}
			}
		}

		aTechTable->add_tech(TmpTech);
	}
	SLOG("end script");
	return true;
}


bool
CRaceScript::get(CRaceTable *aRaceTable)
{
	SLOG("start script no:%d", mRoot.length());
	for(int i=0; i<mRoot.length(); i++)
	{
		CRace
			*NewRace = new CRace();
		TSomething
			Race = get_array(i);
//		SLOG("%s", get_name(Race));
		NewRace->set_name( get_name(Race) );

		TSomething
			Number = get_section("Number", Race);
		if (Number)
			NewRace->set_id(atoi(get_data(Number)));
		else
			return false;

		TSomething
			Description = get_section("Description", Race);
		if (Description)
			NewRace->set_description(get_data(Description));
		else
			SLOG("Non Description");

		TSomething
			Society = get_section("Society", Race);
		if (Society)
		{
			if(!NewRace->set_society_by_name(get_data(Society)))
				SLOG( "WRONG SOCIETY %s(%d) %s",
						NewRace->get_name(), NewRace->get_id(),
						get_data(Society) );
		} else
			return false;

		TSomething
			Empire = get_section("Empire", Race);
		if (Empire)
		{
			NewRace->set_empire_relation(atoi(get_data(Empire)));
		}


		TSomething Tech = get_section("Tech", Race);
		if (Tech)
		{
			TSomething
				TechData;
			int i=0;

			while((TechData = get_array(i++, Tech)))
				NewRace->set_innate_tech(atoi(get_data(TechData)));
		}

		TSomething Fleet = get_section("Fleet", Race);
		if (Fleet)
		{
			TSomething Morale;

			Morale = get_section("Morale", Fleet);
			if (Morale) {
				NewRace->set_morale_modifier( atoi(get_data(Morale)) );
			} else {
				NewRace->set_morale_modifier( 0 );
			}

			TSomething Berserk;

			Berserk = get_section("Berserk", Fleet);
			if (Berserk) {
				NewRace->set_berserk_modifier( atoi(get_data(Berserk)) );
			} else {
				NewRace->set_berserk_modifier( 0 );
			}

			TSomething Survival;

			Survival = get_section("Survival", Fleet);
			if (Survival) {
				NewRace->set_survival_modifier( atoi(get_data(Survival)) );
			} else {
				NewRace->set_survival_modifier( 0 );
			}
		}

		TSomething Home = get_section("Home", Race);
		if (Home)
		{
			TSomething Air;

			Air = get_section("Air", Home);
			if (Air)
			{
				if(!NewRace->set_atmosphere(get_data(Air)))
					SLOG("WRONG AIR : %s(%d) %s",
							NewRace->get_name(), NewRace->get_id(),
							get_data(Air));
			}

			TSomething Gravity;

			Gravity = get_section("Gravity", Home);

			if (Gravity)
			{
				if (!NewRace->set_gravity(atof(get_data(Gravity))))
					SLOG("WRONG GRAVITY : %s(%d) %s",
							NewRace->get_name(), NewRace->get_id(),
							get_data(Gravity));
			} else return false;

			TSomething
				Temperature;

			Temperature = get_section("Temperature", Home);

			if (Temperature)
			{
				if (!NewRace->set_temperature(atoi(get_data(Temperature))))
					SLOG("WRONG TEMPERATURE : %s(%d) %s",
							NewRace->get_name(), NewRace->get_id(),
							get_data(Temperature));
			} else
				return false;
		} else
			return false;

		TSomething Control = get_section("Control", Race);
		if (Control)
		{
			TSomething ControlData;
			int i=0;

			CControlModel ControlModel;

			while((ControlData = get_array(i++, Control)))
				ControlModel.change(get_name(ControlData),
									atoi(get_data(ControlData)) );
			NewRace->set_control_model(ControlModel);
		} else
			return false;

		TSomething Ability = get_section("Ability", Race);
		if (Ability)
		{
			TSomething AbilityData;
			int i=0;

			while ((AbilityData = get_array(i++, Ability)))
			{
				if (!NewRace->set_ability(get_data(AbilityData)))
				{
					SLOG("WRONG ABILITY : %s(%d) %s",
							NewRace->get_name(), NewRace->get_id(),
							get_data(AbilityData));
				}
			}
		}

		aRaceTable->add_race(NewRace);
	}
	SLOG("end script");
	return true;
}

bool
CProjectScript::get(CProjectTable *aProjectTable)
{
	SLOG("start script no:%d", mRoot.length());
	for(int i=0; i<mRoot.length(); i++)
	{
		CProject
			*NewProject = new CProject();
		TSomething
			Project = get_array(i);
		NewProject->set_name( get_name(Project) );
		TSomething
			Number = get_section("Number", Project);
		if (Number)
			NewProject->set_id(atoi(get_data(Number)));
		else {
			SLOG( "error found at number section : %s(%d)",
					NewProject->get_name(), NewProject->get_id() );
			return false;
		}

		TSomething Description = get_section("Description", Project);
		if (Description)
			NewProject->set_description(get_data(Description));
		else
			SLOG("error found at description section : %s(%d)",
					NewProject->get_name(), NewProject->get_id() );

		TSomething Type = get_section("Type", Project);
		if (Type)
			NewProject->set_type(get_data(Type));
		else
		{
			SLOG("error found at type section : %s(%d)",
				NewProject->get_name(), NewProject->get_id() );
			return false;
		}

		TSomething Cost = get_section("Cost", Project);
		if (Cost)
			NewProject->set_cost( atoi(get_data(Cost)) );
		else
		{
			SLOG("error found at cost section : %s(%d)",
					NewProject->get_name(), NewProject->get_id() );
			return false;
		}

		TSomething Prerequisite = get_section("Prerequisite", Project);
		if (Prerequisite)
		{
			load_prerequisites(Prerequisite, (CPrerequisiteList*)NewProject);
		}

		TSomething
			Effect;

		Effect = get_section("Effect", Project);

		if (Effect)
		{
			TSomething
				EffectData;

			int i = 0;
			while ((EffectData = get_array(i++, Effect)))
			{
				const char* Effect = get_name(EffectData);
				if (!strcasecmp(Effect, "Environment") ||
							!strcasecmp(Effect, "Growth") ||
							!strcasecmp(Effect, "Research") ||
							!strcasecmp(Effect, "Production") ||
							!strcasecmp(Effect, "Military") ||
							!strcasecmp(Effect, "Spy") ||
							!strcasecmp(Effect, "Commerce") ||
							!strcasecmp(Effect, "Efficiency") ||
							!strcasecmp(Effect, "Genius") ||
							!strcasecmp(Effect, "Diplomacy") ||
							!strcasecmp(Effect, "Facility Cost"))
					NewProject->set_effect(get_name(EffectData),
									atoi(get_data(EffectData)));
				else
				{
					CPlayerEffect *
						PlayerEffect = new CPlayerEffect();

					if (!PlayerEffect->set_type(Effect))
					{
						SLOG("error found in effect section, no effect by name : %s",
                                                Effect);
                                                delete PlayerEffect;

                                                return false;
					}
					TSomething
						EffectField;
					int j=0;
					while ((EffectField = get_array(j++, EffectData)))
					{
						char *
							EffectFieldName = get_name(EffectField);
						if (!strcmp(EffectFieldName, "Amount"))
						{
							PlayerEffect->set_argument1(atoi(get_data(EffectField)));
						}
						else if (!strcmp(EffectFieldName, "Apply"))
						{
							PlayerEffect->set_apply(get_data(EffectField));
						}
						else if (!strcmp(EffectFieldName, "Ability"))
						{
                          if (ability_name_to_index(get_data(EffectField)) == -1)
                          {
                          	SLOG("error found at effect section : %s(%d) Ability: %s",
                          	NewProject->get_name(), NewProject->get_id(), get_data(EffectField));
							delete PlayerEffect;
                          }
                          else
                            PlayerEffect->set_argument1(ability_name_to_index(get_data(EffectField)));
						}
						else
						{
							SLOG("error found in effect section : %s(%d)",
									NewProject->get_name(), NewProject->get_id());
							delete PlayerEffect;

							return false;
						}

					}
					NewProject->add_effect(PlayerEffect);
				}
			}
		} else {
			SLOG("strange effect section : %s(%d)",
				NewProject->get_name(), NewProject->get_id());
			return false;
		}
		aProjectTable->add_project(NewProject);
	}

	SLOG("end script");
	return true;
}

bool
CClusterNameScript::get(CClusterNameList *aNameList)
{
	SLOG("start script no:%d", mRoot.length());

	for(int i=0; i<mRoot.length(); i++)
	{
		TSomething
			Name = get_array(i);

		if (!strncasecmp(get_name(Name), "NAME", 4))
		{
			if (get_data(Name))
				aNameList->add_name(get_data(Name));
		}
	}

	SLOG("end script");
	return true;
}

bool
CAdmiralNameScript::get(CAdmiralNameTable *aAdmiralNameTable)
{
	SLOG("start admiral name script no:%d", mRoot.length());

	int
		RaceCount = 0,
		NameCount = 0;
	TSomething
		Race;

	for (int i=0 ; (Race = get_array(i)) ; i++)
	{
		TSomething
			First = get_section("First", Race);
		if (First)
		{
			CAdmiralNameList *
				NameList = new CAdmiralNameList();
			TSomething
				Name;
			int
				i = 0;
			while ((Name = get_array(i++, First)))
			{
				NameList->add_name(string_new(get_data(Name)));
				NameCount++;
			}

			NameList->set_race(atoi(get_name(Race)));
			NameList->first();
			aAdmiralNameTable->add_name_list(NameList);
		}

		TSomething
			Last = get_section("Last", Race);
		if (Last)
		{
			CAdmiralNameList
				*NameList = new CAdmiralNameList();
			TSomething
				Name;
			int
				i = 0;
			while ((Name = get_array(i++, Last)))
			{
				NameList->add_name(string_new(get_data(Name)));
				NameCount++;
			}

			NameList->set_race(atoi(get_name(Race)));
			NameList->last();
			aAdmiralNameTable->add_name_list(NameList);
		}

		RaceCount++;
	}

	SLOG("Total %d names and %d races loaded", NameCount, RaceCount);

	return true;
}

bool
CSpyScript::get(CSpyTable *aSpyTable)
{
	SLOG("start spy script no:%d", mRoot.length());

	for(int i=0; i<mRoot.length(); i++)
	{
		TSomething
			Spy = get_array(i);
		CSpy *
			NewSpy = new CSpy();

		NewSpy->set_name(get_name(Spy));

		TSomething
			Number = get_section("Number", Spy);
		if (Number)
		{
			NewSpy->set_id(atoi(get_data(Number)));
		}
		else
		{
			SLOG("error found at number section : %s", NewSpy->get_name());
			delete NewSpy;

			return false;
		}

		TSomething
			Description = get_section("Description", Spy);
		if (Description)
		{
			NewSpy->set_description(get_data(Description));
		}
		else
		{
			SLOG("error found at description section : %s(%d)",
					NewSpy->get_name(), NewSpy->get_id());
			delete NewSpy;

			return false;
		}

		TSomething
			Difficulty = get_section("Difficulty", Spy);
		if (Difficulty)
		{
			NewSpy->set_difficulty(atoi(get_data(Difficulty)));
		}
		else
		{
			SLOG("error found at difficulty section : %s(%d)",
				NewSpy->get_name(), NewSpy->get_id() );
			delete NewSpy;

			return false;
		}

		TSomething
			Cost = get_section("Cost", Spy);
		if (Cost)
		{
			NewSpy->set_cost(atoi(get_data(Cost)));
		}
		else
		{
			SLOG("error found at cost section : %s(%d)",
					NewSpy->get_name(), NewSpy->get_id());
			delete NewSpy;

			return false;
		}


		TSomething Prerequisite = get_section("Prerequisite", Spy);
		if (Prerequisite)
		{
            load_prerequisites(Prerequisite, (CPrerequisiteList*)NewSpy);
		}

		TSomething
			Type = get_section("Type", Spy);
		if (Type)
		{
			char *TypeName = get_data(Type);

			if (!strcasecmp(TypeName, "ACPT")) NewSpy->set_type(CSpy::TYPE_ACCEPTABLE);
			else
			if (!strcasecmp(TypeName, "ORDN")) NewSpy->set_type(CSpy::TYPE_ORDINARY);
			else
			if (!strcasecmp(TypeName, "HOST")) NewSpy->set_type(CSpy::TYPE_HOSTILE);
			else
			if (!strcasecmp(TypeName, "ATRO")) NewSpy->set_type(CSpy::TYPE_ATROCIOUS);
			else
			{
				SLOG("strange type section : %s(%d)",
					NewSpy->get_name(), NewSpy->get_id());
				delete NewSpy;

				return false;
			}
		}
		else
		{
			SLOG("error found at type section : %s(%d)",
				NewSpy->get_name(), NewSpy->get_id());
			delete NewSpy;

			return false;
		}

		aSpyTable->add_spy(NewSpy);
	}

	SLOG("end spy script");

	return true;
}

bool
CEmpireShipDesignScript::get(CShipDesignList *aList)
{
	SLOG("Empire Ship Design Script Start");

	int
		Index = 0;
	TSomething
		TempShipDesign;

	while ((TempShipDesign = get_array(Index)))
	{
		CShipDesign *
			NewShipDesign = new CShipDesign();

		NewShipDesign->set_name(get_name(TempShipDesign));

		TSomething
			Something;

		Something = get_section("Number", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_design_id(as_atoi(get_data(Something)));
		}

		Something = get_section("Body", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_body(as_atoi(get_data(Something)));
		}

		Something = get_section("Armor", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_armor(as_atoi(get_data(Something)));
		}

		Something = get_section("Computer", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_computer(as_atoi(get_data(Something)));
		}

		Something = get_section("Shield", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_shield(as_atoi(get_data(Something)));
		}

		Something = get_section("Engine", TempShipDesign);
		if (Something == NULL)
		{
			SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s", NewShipDesign->get_name());
			return false;
		}
		else
		{
			NewShipDesign->set_engine(as_atoi(get_data(Something)));
		}

		Something = get_section("Device", TempShipDesign);
		if (Something)
		{
			int
				Index = 0;
			TSomething
				Device;

			while ((Device = get_array(Index, Something)))
			{
				NewShipDesign->set_device(Index, atoi(get_data(Device)));
				Index++;
			}
		}

		Something = get_section("WeaponList", TempShipDesign);
		if (Something)
		{
			int
				Index = 0;
			TSomething
				Weapon;

			while ((Weapon = get_array(Index, Something)))
			{
				TSomething
					WeaponID = get_section("Number", Weapon),
					Amount = get_section("Amount", Weapon);
				if (WeaponID != NULL && Amount != NULL)
				{
					NewShipDesign->set_weapon(Index, as_atoi(get_data(WeaponID)));
					NewShipDesign->set_weapon_number(Index, as_atoi(get_data(Amount)));
				}
				else
				{
					SLOG("WRONG EMPIRE SHIP DESIGN FILE at %s in %s",
							get_data(Weapon), get_data(Something));
					return false;
				}
				Index++;
			}
		}

		NewShipDesign->set_owner(0);
		aList->add_ship_design(NewShipDesign);

		Index++;
	}

	SLOG("Empire Ship Design Script End");

	return true;
}

bool
CComponentScript::get(CComponentTable *aComponentTable)
{
	system_log("start script no:%d", mRoot.length());
	int i=0;
	TSomething
		component;

	while( (component = get_array(i++)) )
	{
		//system_log("Component Name:%s", get_name(component));

		TSomething
			something;

		CComponent
			*tmpComponent;
		/*
		something = get_section("Prerequisite", component);
		if (something)
		{
			TSomething
				Prerequisite = get_array(0, something);
			if (Prerequisite)
			{
				tmpComponent->set_prerequisite(as_atoi(get_data(Prerequisite)));
			}
		}
		*/
		something = get_section("Category", component);
		if (something) {
			if ( strcmp( tmpComponent->get_category_name(0),
										get_data(something) ) == 0 ) {
				tmpComponent = new CArmor;
			} else if ( strcmp( tmpComponent->get_category_name(1),
										get_data(something) ) == 0 ) {
				tmpComponent = new CComputer;
			} else if ( strcmp( tmpComponent->get_category_name(2),
										get_data(something) ) == 0 ) {
				tmpComponent = new CShield;
			} else if ( strcmp( tmpComponent->get_category_name(3),
										get_data(something) ) == 0 ) {
				tmpComponent = new CEngine;
			} else if ( strcmp( tmpComponent->get_category_name(4),
										get_data(something) ) == 0 ) {
				tmpComponent = new CDevice;
			} else if ( strcmp( tmpComponent->get_category_name(5),
										get_data(something) ) == 0 ) {
				tmpComponent = new CWeapon;
			} else {
					system_log("WRONG COMPONENT FILE at %s", get_name(component));
					return false;
			}
		} else {
			system_log("WRONG COMPONENT FILE at %s", get_name(component));
			return false;
		}

		something = get_section("Prerequisite", component);
		if (something)
		{
			load_prerequisites(something, (CPrerequisiteList*)tmpComponent);
		}

		/* name */
		tmpComponent->set_name( get_name(component) );

		/* index number */
		something = get_section("Number", component);
		if (something) {
			tmpComponent->set_id(atoi(get_data(something)));
		} else {
			system_log("WRONG COMPONENT FILE at %s", tmpComponent->get_name());
			return false;
		}

		/* description */
		something = get_section("Description", component);
		if (something) {
			tmpComponent->set_description(get_data(something));
		} else {
			system_log("WRONG COMPONENT FILE at %s(%d)",
						tmpComponent->get_name(), tmpComponent->get_id() );
			return false;

		}

		/* level */
		something = get_section("Level", component);
		if (something) {
			tmpComponent->set_level(atoi(get_data(something)));
		} else {
			system_log("WRONG COMPONENT FILE at %s(%d)",
						tmpComponent->get_name(), tmpComponent->get_id() );
			return false;
		}

		/* Armor */
		if ( CComponent::CC_ARMOR == tmpComponent->get_category() ) {
			CArmor
				*tmpArmor = (CArmor *)tmpComponent;

			something = get_section("Type", component);
			if (something) {
				tmpArmor->set_armor_type(get_data(something));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("DR", component);
			if (something) {
				tmpArmor->set_defense_rate(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("HPMult", component);
			if (something) {
				tmpArmor->set_hp_multiplier(as_atof(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}
		}

		/* Computer */
		if ( CComponent::CC_COMPUTER == tmpComponent->get_category() ) {
			CComputer
				*tmpComputer = (CComputer *)tmpComponent;

			something = get_section("AR", component);
			if (something) {
				tmpComputer->set_attacking_rate(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
			}

			something = get_section("DR", component);
			if (something) {
				tmpComputer->set_defense_rate(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
			}
		}

		/* Shield */
		if ( CComponent::CC_SHIELD == tmpComponent->get_category() ) {
			CShield
				*tmpShield = (CShield *)tmpComponent;

			something = get_section("Deflection", component);
			if (something) {
				tmpShield->set_deflection_solidity(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
			}

			something = get_section("Recharge", component);
			if (something) {
				for(int i=0; i < MAX_SHIP_CLASS; i++)
				{
					tmpShield->set_recharge_rate( i,
						atoi( get_data( get_array(i, something) ) ) );
				}
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Str", component);
			if (something) {
				for(int i=0; i < MAX_SHIP_CLASS; i++)
				{
					tmpShield->set_strength(i,
						atoi( get_data( get_array(i, something) ) ) );
				}
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

		}

		/* Engine */
		if ( CComponent::CC_ENGINE == tmpComponent->get_category() ) {
			CEngine
				*tmpEngine = (CEngine *)tmpComponent;

			something = get_section("BSpeed", component);
			if (something) {
				for(int i=0; i < MAX_SHIP_CLASS; i++)
				{
					tmpEngine->set_battle_speed(i,
						atoi( get_data( get_array(i, something) ) ) );
				}
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Mobility", component);
			if (something) {
				for(int i=0; i < MAX_SHIP_CLASS; i++)
				{
					tmpEngine->set_battle_mobility(i,
						atoi( get_data( get_array(i, something) ) ) );
				}
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}
		}

		/* Device */
		if ( CComponent::CC_DEVICE == tmpComponent->get_category() ) {
			CDevice
				*tmpDevice = (CDevice *)tmpComponent;

			something = get_section("MinC", component);
			if (something) {
				tmpDevice->set_min_class(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("MaxC", component);
			if (something) {
				tmpDevice->set_max_class(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}
		}

		/* Weapon */
		if ( CComponent::CC_WEAPON == tmpComponent->get_category() ) {
			CWeapon
				*tmpWeapon = (CWeapon *)tmpComponent;

			something = get_section("Type", component);
			if (something) {
				tmpWeapon->set_weapon_type(get_data(something));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("AR", component);
			if (something) {
				tmpWeapon->set_attacking_rate(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Roll", component);
			if (something) {
				tmpWeapon->set_damage_roll(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Dice", component);
			if (something) {
				tmpWeapon->set_damage_dice(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Space", component);
			if (something) {
				tmpWeapon->set_space(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("CT", component);
			if (something) {
				tmpWeapon->set_cooling_time(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("Range", component);
			if (something) {
				tmpWeapon->set_range(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("AOF", component);
			if (something) {
				tmpWeapon->set_angle_of_fire(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}

			something = get_section("WSpeed", component);
			if (something) {
				tmpWeapon->set_speed(atoi(get_data(something)));
			} else {
				system_log("WRONG COMPONENT FILE at %s(%d)",
							tmpComponent->get_name(),
							tmpComponent->get_id() );
				return false;
			}
		}

		something = get_section( "Attribute", component );
		if( something ){
			TSomething
				Attr;
			int
				i = 0;
			while( (Attr = get_array(i, something)) ){
				tmpComponent->set_attribute( get_data(Attr) );
				i++;
			}
		}

		something = get_section("Effect", component);
		if( something ){
			TSomething
				Effect;
			int
				i = 0;
			while( (Effect = get_array(i, something)) ){
				CFleetEffect
					*NewEffect = new CFleetEffect();

				NewEffect->set_type( get_name(Effect) );

				TSomething
					Tmp = get_section( "Amount", Effect );
				if( Tmp )
					NewEffect->set_amount( atoi(get_data(Tmp)) );
				else
					NewEffect->set_amount(0);

				Tmp = get_section( "Type", Effect );
				if( Tmp )
					NewEffect->set_apply_type( get_data(Tmp) );
				else
					NewEffect->set_apply_type( CFleetEffect::AT_ABSOLUTE );

				Tmp = get_section( "Period", Effect );
				if( Tmp )
					NewEffect->set_period( atoi(get_data(Tmp)) );
				else
					NewEffect->set_period( 0 );

				Tmp = get_section( "Range", Effect );
				if( Tmp )
					NewEffect->set_range( atoi(get_data(Tmp)) );
				else
					NewEffect->set_range( 0 );

				Tmp = get_section( "Target", Effect );
				if( Tmp )
					NewEffect->set_target( get_data(Tmp) );
				else
					NewEffect->set_target( CFleetEffect::LOCAL_EFFECT );

				tmpComponent->add_effect( NewEffect );

				i++;
			}
		}

		//system_log("%s %d", tmpComponent->get_name(), tmpComponent->get_prerequisite() );
		aComponentTable->add_component(tmpComponent);
	}

	system_log("end script");
	return true;
}


