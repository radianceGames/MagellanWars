#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "define.h"

bool
CPageShipDesignConfirm::handler( CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("STRING_SIZE", GETTEXT("Size"));
	ITEM("STRING_COMPUTER", GETTEXT("Computer"));
	ITEM("STRING_ARMOR", GETTEXT("Armor"));
	ITEM("STRING_ENGINE", GETTEXT("Engine"));
	ITEM("STRING_SHIELD", GETTEXT("Shield"));
	ITEM("STRING_BUILDING_COST", GETTEXT("Building Cost"));

	QUERY("CLASS_NAME", ClassNameData);
	if (!ClassNameData)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a valid name for class."));
		return output("fleet/ship_design_error.html");
	}
	CString ClassName = ClassNameData;
	ClassName.strip_all_slashes();


	if (!is_valid_name((char *)ClassName))
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a valid name for class."));
		return output("fleet/ship_design_error.html");
	}

	ITEM("NAME",
			format("%s<INPUT TYPE=hidden NAME=CLASS_NAME VALUE=\"%s\">",
					(char*)ClassName, (char*)ClassName));

	QUERY("SHIP_SIZE", SizeString);
	int
		Size = as_atoi(SizeString);
	int
		TechReq = 0;

        // Simple prerequisites for now
	TechReq = (aPlayer->count_tech_by_category( CTech::TYPE_MATTER_ENERGY ));


	if (Size > TechReq)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You do not possess the required number of ME techs to build such a ship, please choose a smaller ship."));
		return output("fleet/ship_design_error.html");
	}
	if (Size < 1 || Size > MAX_SHIP_CLASS)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Ship size is not valid."));
		return output("fleet/ship_design_error.html");
	}
	CShipSize *
		Body = (CShipSize *)(SHIP_SIZE_TABLE->get_by_id(4000 + Size));
	if (!Body)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected wrong ship size. Please try again."));
		return output("fleet/ship_design_error.html");
	}

	ITEM("SIZE",
			format("%s<INPUT TYPE=hidden NAME=SHIP_SIZE VALUE=\"%d\">",
					Body->get_name(), Size));

	CComponentList *
		ComponentList = aPlayer->get_component_list();

	QUERY("COMPUTER", ComputerString);
	int
		ComputerID = as_atoi(ComputerString);
	CComputer *
		Computer = (CComputer *)(ComponentList->get_by_id(ComputerID));
	if (!Computer)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("It seems that you can't use this computer #(%1$s) right now."),
								dec2unit(ComputerID)));
		return output("fleet/ship_design_error.html");
	}
	if (Computer->get_category() != CComponent::CC_COMPUTER)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The computer you have selected is actually not in a computer category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	ITEM("COMPUTER",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=COMPUTER VALUE=\"%d\">",
					Computer->get_name(), GETTEXT("Level"), Computer->get_level(), Computer->get_id()));

	QUERY("ARMOR", ArmorString);
	int
		ArmorID = as_atoi(ArmorString);
	CArmor *
		Armor = (CArmor *)(ComponentList->get_by_id(ArmorID));
	if (!Armor)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("It seems that you can't use this armor #(%1$s) right now."),
								dec2unit(ArmorID)));
		return output("fleet/ship_design_error.html");
	}
	if (Armor->get_category() != CComponent::CC_ARMOR)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The armor you have selected is actually not in an armor category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	ITEM("ARMOR",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=ARMOR VALUE=\"%d\">",
					Armor->get_name(), GETTEXT("Level"), Armor->get_level(), Armor->get_id()));

	QUERY("ENGINE", EngineString);
	int
		EngineID = as_atoi(EngineString);
	CEngine *
		Engine = (CEngine *)ComponentList->get_by_id(EngineID);
	if (!Engine)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("It seems that you can't use this engine #(%1$s) right now."),
								dec2unit(EngineID)));
		return output("fleet/ship_design_error.html");
	}
	if (Engine->get_category() != CComponent::CC_ENGINE)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The engine you have selected is actually not in an engine category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	ITEM("ENGINE",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=ENGINE VALUE=\"%d\">",
					Engine->get_name(), GETTEXT("Level"), Engine->get_level(), Engine->get_id()));

	QUERY("SHIELD", ShieldString);
	int
		ShieldID= as_atoi(ShieldString);
	CShield *
		Shield = (CShield *)ComponentList->get_by_id(ShieldID);
	if (!Shield)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("It seems that you can't use this shield #(%1$s) right now."),
								dec2unit(ShieldID)));
		return output("fleet/ship_design_error.html");
	}
	if (Shield->get_category() != CComponent::CC_SHIELD)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The shield you have selected is actually not in a shield category. Maybe a cheat?"));
		return output("fleet/ship_design_error.html");
	}

	ITEM("SHIELD",
			format("%s (%s %d)<INPUT TYPE=hidden NAME=SHIELD VALUE=\"%d\">",
					Shield->get_name(), GETTEXT("Level"), Shield->get_level(), Shield->get_id()));

	if (Armor->get_armor_type() == AT_BIO &&
		aPlayer->has_ability(ABILITY_GREAT_SPAWNING_POOL))
	{
		ITEM("BUILDING_COST", dec2unit(Body->get_cost() * 80 / 100));
	}
	else
	{
		ITEM("BUILDING_COST", dec2unit(Body->get_cost()));
	}

	static CString
		WeaponInfo;
	WeaponInfo.clear();

	for (int i=0 ; i<Body->get_weapon() ; i++)
	{
		CString
			QueryVar;
		QueryVar.clear();
		QueryVar.format("WEAPON%d", i+1);

		QUERY((char *)QueryVar, WeaponString);
		int
			WeaponID = as_atoi(WeaponString);
		CWeapon *
			Weapon = (CWeapon *)ComponentList->get_by_id(WeaponID);
		if (!Weapon)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("It seems that you can't use this weapon #(%1$s) right now."),
									dec2unit(WeaponID)));
			return output("fleet/ship_design_error.html");
		}

		if (Weapon->get_category() != CComponent::CC_WEAPON)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("The weapon you have selected is actually not in a weapon category. Maybe a cheat?"));
			return output("fleet/ship_design_error.html");
		}

		if (Body->get_slot() < Weapon->get_space())
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("%1$s can't be put in this ship's slot"
											" because it's too big."),
									Weapon->get_name()));
			return output("fleet/ship_design_error.html");
		}

		WeaponInfo += "<TR>\n";
		WeaponInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"75\">";
		WeaponInfo.format("<FONT COLOR=\"#666666\">%s %d</FONT></TD>\n",
				GETTEXT("Weapon"), i+1);
		if (!WeaponID)
		{
			WeaponInfo.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"366\">%s</TD>\n",
					GETTEXT("None"));
		} else
		{
			WeaponInfo.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"366\">%s x %d</TD>\n",
					Weapon->get_name(), Body->get_slot()/Weapon->get_space());
		}
		WeaponInfo.format("<INPUT TYPE=hidden NAME=WEAPON%d VALUE=\"%d\">",
				i+1, WeaponID);
		WeaponInfo += "</TR>\n";
	}

	ITEM("WEAPON_INFO", (char *)WeaponInfo);

	static CString
		DeviceInfo;
	DeviceInfo.clear();

	if (!Body->get_device())
	{
		DeviceInfo = "<TR>\n";
		DeviceInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"75\">";
		DeviceInfo.format("<FONT COLOR=\"#666666\">%s</FONT></TD>\n",
				GETTEXT("Device"));
		DeviceInfo.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"366\">%s</TD>\n",
				GETTEXT("No devices available on this type of ship."));
		DeviceInfo += "</TR>\n";
	} else
	{
		CIntegerList
			DeviceIDList;

		for (int i=0 ; i<Body->get_device() ; i++)
		{
			CString
				QueryVar;
			QueryVar.clear();
			QueryVar.format("DEVICE%d", i+1);

			QUERY((char *)QueryVar, DeviceString);
			int
				DeviceID = as_atoi(DeviceString);
			if (DeviceID != 0)
			{
				if (DeviceIDList.find_sorted_key((void *)DeviceID) != -1)
				{
					ITEM("ERROR_MESSAGE",
							GETTEXT("There's a repeated device. Please try again."));
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
					ITEM("ERROR_MESSAGE",
							GETTEXT("The device you have selected is actually not in a device category. Maybe a cheat?"));
					return output("fleet/ship_design_error.html");
				}

				if (Device->get_min_class() > Size ||
					Device->get_max_class() < Size)
				{
					ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("'%1$s' is not available on this size of ship."),
											Device->get_name()));
					return output("fleet/ship_design_error.html");
				}
			}

			CDevice *
				Device = (CDevice *)ComponentList->get_by_id(DeviceID);
			DeviceInfo += "<TR>\n";
			DeviceInfo += "<TD CLASS=\"tabletxt\" WIDTH=\"75\">";
			DeviceInfo.format("<FONT COLOR=\"#666666\">%s %d</FONT></TD>\n",
					GETTEXT("Device"), i+1);
			if (DeviceID == 0)
			{
				DeviceInfo.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"366\">%s</TD>\n",
						GETTEXT("None"));
			} else
			{
				DeviceInfo.format("<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"366\">%s</TD>\n",
						Device->get_name());
			}
			DeviceInfo.format("<INPUT TYPE=hidden NAME=DEVICE%d VALUE=\"%d\">",
					i+1, DeviceID);
			DeviceInfo += "</TR>\n";
		}
	}

	ITEM("DEVICE_INFO", (char *)DeviceInfo);

//	system_log( "end page handler %s", get_name() );

	return output( "fleet/ship_design_confirm.html" );
}

