#include <libintl.h>
#include <string.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageShipDesignRegister::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Result;
	Result.clear();

	QUERY("CLASS_NAME", OriginalClassName);

	CString
		ClassName;
	ClassName = OriginalClassName;
	ClassName.strip_all_slashes();
	ClassName.htmlspecialchars();
	ClassName.nl2br();

	if ((char *)ClassName == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a name for class."));
		return output("fleet/ship_design_error.html");
	}
	if (!is_valid_name((char *)ClassName))
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a valid name for class."));
		return output("fleet/ship_design_error.html");
	}
	
/*	#if MONKEY_BUSINESS
   
	if (!strcmp(ClassName, "POWEROVWMG321")) {
        aPlayer->buy_project(9016);
        aPlayer->buy_project(9026);
    }

    if (!strcmp(ClassName, "CHEATZSTEROIDZ")) {
        aPlayer->change_ship_production(1000000000);
    }

    if (!strcmp(ClassName, "BOOKWORM12345")) {
        CTech *TargetTech = NULL;
        TargetTech = TECH_TABLE->get_by_id(aPlayer->get_target_tech());
        if(TargetTech) {
            aPlayer->discover_tech(TargetTech->get_id());
	    }
    }
    
    #endif
*/
	CShipDesign *
		Design = new CShipDesign;

	Design->set_name((char *)ClassName);

	CShipDesignList *
		DesignList = aPlayer->get_ship_design_list();
	int
		NewDesignID = DesignList->max_design_id() + 1;

	Design->set_design_id(NewDesignID);
	Design->set_owner( aPlayer->get_game_id() );

	QUERY("SHIP_SIZE", ShipSizeString);
	int
		ShipSize = as_atoi(ShipSizeString);
    int
		Tech = 0;

	Tech = (aPlayer->count_tech_by_category( CTech::TYPE_MATTER_ENERGY ));

	if (ShipSize < 1 || ShipSize > MAX_SHIP_CLASS)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				GETTEXT("Ship size is not valid."));
		return output("fleet/ship_design_error.html");
	}

	if (ShipSize > Tech )
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You do not possess the schematics to build such a ship, please choose a smaller ship."));
		return output("fleet/ship_design_error.html");
	}

	Design->set_body( 4000+ShipSize );

	CShipSize *
		Body = SHIP_SIZE_TABLE->get_by_id(4000 + ShipSize);

	Design->set_build_cost( Body->get_cost() );
	Design->set_build_time( (int)time((time_t*)0) );

	CComponentList *
		ComponentList = aPlayer->get_component_list();

	QUERY("COMPUTER", ComputerString);
	if (!ComputerString)
	{
		delete Design;

		ITEM("ERROR_MESSAGE", GETTEXT("Computer data is not valid."));
		return output("fleet/ship_design_error.html");
	}

	int
		ComputerID = as_atoi(ComputerString);
	CComputer *
		Computer = (CComputer *)ComponentList->get_by_id(ComputerID);

	if (!Computer)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("You don't have a computer with ID %1$s."),
						dec2unit(ComputerID)));
		return output("fleet/ship_design_error.html");
	}
	if (Computer->get_category() != CComponent::CC_COMPUTER)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				GETTEXT("The computer you have selected is actually not in a computer category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	Design->set_computer(ComputerID);

	QUERY("ENGINE", EngineString);
	if (!EngineString)
	{
		delete Design;

		ITEM("ERROR_MESSAGE", GETTEXT("Engine data is not valid."));
		return output("fleet/ship_design_error.html");
	}

	int
		EngineID = as_atoi(EngineString);

	CEngine *
		Engine = (CEngine *)ComponentList->get_by_id(EngineID);
	if (!Engine)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("You don't have an engine with ID %1$s."),
						dec2unit(EngineID)));
		return output("fleet/ship_design_error.html");
	}

	if (Engine->get_category() != CComponent::CC_ENGINE)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				GETTEXT("The engine you have selected is actually not in an engine category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	Design->set_engine(EngineID);

	QUERY("SHIELD", ShieldString);
	if (!ShieldString)
	{
		delete Design;

		ITEM("ERROR_MESSAGE", GETTEXT("Shield data is not valid."));
		return output("fleet/ship_design_error.html");
	}

	int
		ShieldID = as_atoi(ShieldString);

	CShield *
		Shield = (CShield *)ComponentList->get_by_id(ShieldID);
	if (!Shield)
	{
		delete Design;

		Result.format(GETTEXT("You don't have a shield with ID %1$s."),
						dec2unit(ShieldID));
		ITEM("ERROR_MESSAGE", (char *)Result);
		return output("fleet/ship_design_error.html");
	}

	if (Shield->get_category() != CComponent::CC_SHIELD)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				GETTEXT("The shield you have selected is actually not in a shield category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	Design->set_shield(ShieldID);

	QUERY("ARMOR", ArmorString);
	if (!ArmorString)
	{
		delete Design;

		ITEM("ERROR_MESSAGE", GETTEXT("Armor data is not valid."));
		return output("fleet/ship_design_error.html");
	}

	int
		ArmorID = as_atoi(ArmorString);

	CArmor *
		Armor = (CArmor *)ComponentList->get_by_id(ArmorID);
	if (!Armor)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("You don't have an armor with ID %1$s."),
						dec2unit(ArmorID)));
		return output("fleet/ship_design_error.html");
	}

	if (Armor->get_category() != CComponent::CC_ARMOR)
	{
		delete Design;

		ITEM("ERROR_MESSAGE",
				GETTEXT("The armor you have selected is actually not in an armor category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	Design->set_armor(ArmorID);

	for (int i=0 ; i<WEAPON_MAX_NUMBER ; i++)
	{
		if (i<Body->get_weapon())
		{
			CString
				QueryVar;
			QueryVar.clear();
			QueryVar.format("WEAPON%d", i+1);

			QUERY((char *)QueryVar, WeaponString);

			if (!WeaponString)
			{
				delete Design;

				ITEM("ERROR_MESSAGE",
						GETTEXT("Weapon data is not valid."));
				return output("fleet/ship_design_error.html");
			}

			int
				WeaponID;
			WeaponID = as_atoi(WeaponString);

			CWeapon *
				Weapon = (CWeapon *)ComponentList->get_by_id(WeaponID);
			if (Weapon)
			{
				if (Weapon->get_category() != CComponent::CC_WEAPON)
				{
					delete Design;

					ITEM("ERROR_MESSAGE",
							GETTEXT("The weapon you have selected is actually not in a weapon category. Maybe a cheat?"));
					return output("fleet/ship_design_error.html");
				}

				Design->set_weapon(i, WeaponID);
				Design->set_weapon_number(i, Body->get_slot()/Weapon->get_space());
			} else
			{
				delete Design;

				Result.format(
						GETTEXT("You don't have a weapon with ID %1$s."),
						dec2unit(WeaponID));
				ITEM("ERROR_MESSAGE", (char *)Result);
				return output("fleet/ship_design_error.html");
			}
		} else
		{
			Design->set_weapon(i, 0);
			Design->set_weapon_number(i, 0);
		}
	}

	CIntegerList
		DeviceIDList;

	for (int i=0 ; i<DEVICE_MAX_NUMBER ; i++)
	{
		if (i < Body->get_device())
		{
			CString
				QueryVar;
			QueryVar.clear();
			QueryVar.format("DEVICE%d", i+1);

			QUERY((char *)QueryVar, DeviceString);
			if (DeviceString == NULL)
			{
				delete Design;

				ITEM("ERROR_MESSAGE",
						GETTEXT("Device data is not valid."));
				return output("fleet/ship_design_error.html");
			}

			int
				DeviceID = as_atoi(DeviceString);
			if (DeviceID != 0)
			{
				if (DeviceIDList.find_sorted_key((void *)DeviceID) != -1)
				{
					ITEM("ERROR_MESSAGE",
							GETTEXT("There's an repeated device. Please try again."));
					return output("fleet/ship_design_error.html");
				}
				else
				{
					DeviceIDList.insert_sorted((void *)DeviceID);
				}

				CDevice *
					Device = (CDevice *)ComponentList->get_by_id(DeviceID);
				if (Device == NULL)
				{
					ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("It seems that you can't use this device #(%1$s) right now."),
											dec2unit(DeviceID)));
					return output("fleet/ship_design_error.html");
				}

				if (Device->get_category() != CComponent::CC_DEVICE)
				{
					delete Design;

					ITEM("ERROR_MESSAGE",
							GETTEXT("The device you have selected is actually not in a device category. Maybe a cheat?"));
					return output("fleet/ship_design_error.html");
				}

				if (Device->get_min_class() > ShipSize ||
					Device->get_max_class() < ShipSize)
				{
					ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("'%1$s' is not available on this size of ship."),
											Device->get_name()));
					return output("fleet/ship_design_error.html");
				}
			}

			CDevice *
				Device = (CDevice *)ComponentList->get_by_id(DeviceID);
			if (Device != NULL)
			{
				Design->set_device(i, DeviceID);
			} else
			{
				Design->set_device(i, 0);
			}
		}
	}

    if (Design->check_legality()) {
        // Do nothing
    } else {
        ITEM("ERROR_MESSAGE",
              (char *)format(GETTEXT("The ship design has not met the requirements to use one or more components.")));
		return output("fleet/ship_design_error.html");
    }
	DesignList->add_ship_design(Design);
	Design->type(QUERY_INSERT);
	*STORE_CENTER << *Design;

	ITEM("REGISTER_MESSAGE",
			GETTEXT("You have registered this ship design. You can build this ship now."));

//	system_log("end page handler %s", get_name());

	return output("fleet/ship_design_register.html");
}

