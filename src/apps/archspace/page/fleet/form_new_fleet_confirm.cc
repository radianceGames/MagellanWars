#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageFormNewFleetConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	static CString
		Javascript;
	Javascript.clear();

	CPreference *aPreference =
		aPlayer->get_preference();

	CCommandSet
		AdmiralSet;
	AdmiralSet.clear();

	CAdmiralList *
		Pool = aPlayer->get_admiral_pool();
	for (int i=0 ; i<Pool->length() ; i++)
	{
		char
			QueryVar[11] = "ADMIRAL000";

		if (i < 10)
		{
			QueryVar[7] = i + '0';
			QueryVar[8] = '\0';
		}
		else if (i < 100)
		{
			QueryVar[7] = i/10 + '0';
			QueryVar[8] = i%10 + '0';
			QueryVar[9] = '\0';
		}
		else if (i < 1000)
		{
			QueryVar[7] = i/100 + '0';
			QueryVar[8] = (i%100)/10 + '0';
			QueryVar[9] = i%10 + '0';
			QueryVar[10] = '\0';
		}
		else
		{
			message_page("Strange admiral number");
			return true;
		}

		QUERY(QueryVar, AdmiralIndexString);
		if (AdmiralIndexString)
		{
			if (!strcasecmp(AdmiralIndexString, "ON"))
			{
				AdmiralSet += i;
			}
		}
	}

	if (AdmiralSet.is_empty())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't select any fleet commanders."));
		return output("fleet/form_new_fleet_error.html");
	}

	int AdmiralCount = 0;
	int AdmiralIndex = 0;
	for (int i = 0 ; i < Pool->length(); i++)
	{
		if (AdmiralSet.has(i) == true)
		{
			AdmiralCount++;
			AdmiralIndex = i;
		}
	}
	if (AdmiralCount == 1)
	{
		QUERY("FLEET_ID", FleetIDString);
		if (!FleetIDString)
		{
			ITEM("ERROR_MESSAGE",GETTEXT("The new fleet ID was not found."
															" Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		int
			FleetID = as_atoi(FleetIDString);

		if (FleetID < 1)
		{
			ITEM("ERROR_MESSAGE",
								GETTEXT("The new fleet ID is invalid."
												" Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		CFleetList *
			FleetList = aPlayer->get_fleet_list();
		CFleet *
			Fleet = (CFleet *)FleetList->get_by_id(FleetID);

		if (Fleet)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("ID %1$s is already in use."),
									dec2unit(FleetID)));
			return output("fleet/form_new_fleet_error.html");
		}

		QUERY("NEW_FLEET_NAME", OriginalFleetName);

		CString
			FleetName;
		FleetName = OriginalFleetName;
		FleetName.htmlspecialchars();
		FleetName.nl2br();
		FleetName.strip_all_slashes();
		if ((char *)FleetName == NULL)
		{
			ITEM("ERROR_MESSAGE", GETTEXT("You didn't enter a fleet name."));
			return output("fleet/form_new_fleet_error.html");
		}

		if(!is_valid_name((char *)FleetName))
		{
			ITEM("ERROR_MESSAGE", GETTEXT("You did not enter a valid fleet name."));
			return output("fleet/form_new_fleet_error.html");
		}

		CAdmiral *
			Admiral = (CAdmiral *)Pool->get(AdmiralIndex);

		if (!Admiral)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("The admiral you chose doesn't exist."));
			return ("fleet/form_new_fleet_error.html");
		}

		QUERY("SHIP_CLASS_ID", ShipClassString);
		if (!ShipClassString)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("Class String is NULL."
							" Please ask Archspace Development Team."));
			return ("fleet/form_new_fleet_error.html");
		}

		int
			ClassID = as_atoi(ShipClassString);
		CShipDesign *
			Class = aPlayer->get_ship_design_list()->get_by_id(ClassID);
		if (!Class)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("The admiral you chose doesn't exist."));
			return ("fleet/form_new_fleet_error.html");
		}

		QUERY("SHIP_NUMBER", ShipNumberString);
		int
			ShipNumber = as_atoi(ShipNumberString);

		if (ShipNumber < 1)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You have to enter a larger number than 0."));
			return ("fleet/form_new_fleet_error.html");
		}

		if (ShipNumber > aPlayer->get_dock()->count_ship(ClassID))
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You have only %1$d %2$s ships."),
									aPlayer->get_dock()->count_ship(ClassID),
									Class->get_name()));
			return ("fleet/form_new_fleet_error.html");
		}

		if (ShipNumber > Admiral->get_fleet_commanding())
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(
							GETTEXT("Fleet Commander %1$s can command %2$d ship(s)"
									" at most."),
							Admiral->get_name(),
							Admiral->get_fleet_commanding()));
			return ("fleet/form_new_fleet_error.html");
		}

		Fleet = new CFleet;
		Fleet->set_id(FleetID);
		Fleet->set_owner(aPlayer->get_game_id());
		Fleet->set_name((char *)FleetName);
		Fleet->set_admiral(Admiral->get_id());
		Fleet->set_ship_class(Class);
		Fleet->set_max_ship(ShipNumber);
		Fleet->set_current_ship(ShipNumber);
		Fleet->set_exp(25 + aPlayer->get_control_model()->get_military()*3);

		aPlayer->get_dock()->change_ship(Class, -ShipNumber);
		aPlayer->get_fleet_list()->add_fleet(Fleet);

		Fleet->type(QUERY_INSERT);
		*STORE_CENTER << *Fleet;

		Admiral->set_fleet_number(FleetID);
		Pool->remove_without_free_admiral(Admiral->get_id());
		aPlayer->get_admiral_list()->add_admiral(Admiral);

		Admiral->type(QUERY_UPDATE);
		*STORE_CENTER << *Admiral;

		ITEM("CONFIRM_TABLE_JAVASCRIPT", "//No table.");

		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("The new fleet %1$s has been formed.<BR>"
						"It is now on stand-by."),
						Fleet->get_nick()));

		return output( "fleet/form_new_fleet_result.html" );

	}
	else
	{
		CFleetList *
			FleetList = aPlayer->get_fleet_list();

		//Add column javascript.
		Javascript += "var FleetingColumns = \n{\n\tID : 1,\n\tNAME : 2,\n\tDESIGN : 3,\n\tNUMBER : 4\n}\n";
		Javascript += "function selectedAdmiralSwapAndSort(column)\n{\n";
		Javascript += "\tvar Action = \n\t{\n\t\tNONE : -1,\n\t\tASCENDING : 0,\n\t\tDESCENDING : 1\n\t}\n";
		Javascript += "\tswitch (SelectedAdmiralTable.getSortingOrder(column))\n\t{\n";
		Javascript += "\t\tcase Action.NONE:\n\t\tSelectedAdmiralTable.addPriorityRank(column, Action.ASCENDING);\n\t\tif (SelectedAdmiralTable.getPriorityRank(column) > 4) SelectedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\telse SelectedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-\" + (parseInt(SelectedAdmiralTable.getPriorityRank(column)) + 1) + \".gif\\\"></TH>\");\t\tbreak;\n";
		Javascript += "\t\tcase Action.ASCENDING:\n\t\tSelectedAdmiralTable.setSortingOrder(column, Action.DESCENDING);\n\t\tif (SelectedAdmiralTable.getPriorityRank(column) > 4) SelectedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-12.gif\\\"></TH>\");\n\t\telse SelectedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-\" + (parseInt(SelectedAdmiralTable.getPriorityRank(column)) + 6) + \".gif\\\"></TH>\");\t\tbreak;\n";
		Javascript += "\t\tcase Action.DESCENDING:\n\t\tSelectedAdmiralTable.removePriorityRank(column);\n\t\tSelectedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"</TH>\");\n";
		Javascript += "\t\tfor (var col = 0; col < SelectedAdmiralTable.getTotalColumns(); col++)\n\t\t{\n\t\t\tif (SelectedAdmiralTable.getSortingOrder(col) == Action.ASCENDING)\n\t\t\t{\n\t\t\t\tif (SelectedAdmiralTable.getPriorityRank(col) > 4) SelectedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse SelectedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-\" + (parseInt(SelectedAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n";
		Javascript += "\t\t\telse if (SelectedAdmiralTable.getSortingOrder(col) == Action.DESCENDING)\n\t\t\t{\n\t\t\t\tif (SelectedAdmiralTable.getPriorityRank(col) > 4) SelectedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse SelectedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, SelectedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://archspace.org/image/as_game/admrlArrow-\" + (parseInt(SelectedAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n\t\t}\n\t\tbreak;\n";
		Javascript += "\t}\n";
		Javascript += "\tvar TempTableValues = new Array(SelectedAdmiralTable.getTotalColumns());\nfor (var i = 0; i < SelectedAdmiralTable.getTotalRows(); i++)\n{\n\tTempTableValues[(\"FLEET_ID\" + SelectedAdmiralTable.getCell(i, FleetingColumns.ID))] = document.getElementsByName(\"FLEET_ID\" + SelectedAdmiralTable.getCell(i, FleetingColumns.ID))[0].value;\n\tTempTableValues[(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NAME))] = document.getElementsByName(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NAME))[0].value;\n\tTempTableValues[(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).indexOf(\"\\\"\")))] = document.getElementsByName(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(0, FleetingColumns.DESIGN).indexOf(\"\\\"\")))[0].selectedIndex;\n\tTempTableValues[(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))] = document.getElementsByName(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))[0].value;\n}\n";
		Javascript += "\tSelectedAdmiralTable.sort();\n";
		Javascript += "\tdocument.getElementById(\"SelectedAdmirals\").innerHTML = SelectedAdmiralTable.getTableHTML();\n";
		Javascript += "\tfor (var i = 0; i < SelectedAdmiralTable.getTotalRows(); i++)\n{\n\tdocument.getElementsByName(\"FLEET_ID\" + SelectedAdmiralTable.getCell(i, FleetingColumns.ID))[0].value = TempTableValues[(\"FLEET_ID\" + SelectedAdmiralTable.getCell(i, FleetingColumns.ID))];\n\tdocument.getElementsByName(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NAME))[0].value = TempTableValues[(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NAME))];\n\tdocument.getElementsByName(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).indexOf(\"\\\"\")))[0].selectedIndex = TempTableValues[(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).indexOf(\"\\\"\")))];\n\tdocument.getElementsByName(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))[0].value = TempTableValues[(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))];\n}\n";
		Javascript += "}\n\n";
		Javascript += "function assignAllFleetID ()\n{\n\tvar TopFleetID = parseInt(document.getElementsByName(\"FLEET_ID\" + SelectedAdmiralTable.getCell(0, FleetingColumns.ID))[0].value);\n\tfor (var i = 1; i < SelectedAdmiralTable.getTotalRows(); i++)\n\t{\n\t\tdocument.getElementsByName(\"FLEET_ID\" + SelectedAdmiralTable.getCell(i, FleetingColumns.ID))[0].value = (TopFleetID + i);\n\t}\n}\n";
		Javascript += "function assignAllFleetName ()\n{\n\tvar TopFleetName = document.getElementsByName(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(0, FleetingColumns.NAME))[0].value;\n\tfor (var i = 1; i < SelectedAdmiralTable.getTotalRows(); i++)\n\t{\n\t\tdocument.getElementsByName(\"FLEET_NAME\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NAME))[0].value = TopFleetName;\n\t}\n}\n";
		Javascript += "function assignAllShipDesign ()\n{\n\tvar TopShipDesign = document.getElementsByName(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(0, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(0, FleetingColumns.DESIGN).indexOf(\"\\\"\")))[0].selectedIndex;\n\tfor (var i = 1; i < SelectedAdmiralTable.getTotalRows(); i++)\n\t{\n\t\tdocument.getElementsByName(\"SHIP_DESIGN\" + SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).substring(0,SelectedAdmiralTable.getCell(i, FleetingColumns.DESIGN).indexOf(\"\\\"\")))[0].selectedIndex = TopShipDesign;\n\t}\n}\n";
		Javascript += "function assignAllShipNumber ()\n{\n\tvar TopShipNumber = parseInt(document.getElementsByName(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(0, FleetingColumns.NUMBER))[0].value);\n\tfor (var i = 1; i < SelectedAdmiralTable.getTotalRows(); i++)\n\t{\n\t\tdocument.getElementsByName(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))[0].value = TopShipNumber;\n\t}\n}\n";
		Javascript += "function assignAllShipNumberMax ()\n{\n\tfor (var i = 0; i < SelectedAdmiralTable.getTotalRows(); i++)\n\t{\n\t\tdocument.getElementsByName(\"SHIP_NUMBER\" + SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER))[0].value = SelectedAdmiralTable.getCell(i, FleetingColumns.NUMBER + 1);\n\t}\n}\n";
		Javascript += "SelectedAdmiralTable = new Table();\n";
		Javascript += "SelectedAdmiralTable.setTableAttributes(\"WIDTH=\\\"550\\\" BORDER=\\\"1\\\" CELLSPACING=\\\"0\\\" CELLPADDING=\\\"0\\\" BORDERCOLOR=\\\"#2A2A2A\\\"\");\n";
		Javascript += "SelectedAdmiralTable.setHeaderRowAttributes(\"STYLE=\\\"vertical-align: bottom;\\\" BGCOLOR=\\\"#171717\\\"\");\n";
		Javascript += "SelectedAdmiralTable.setSortDelimiter(\"#SORT#\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:selectedAdmiralSwapAndSort(0)\\\">ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\">Fleet&nbsp;ID<BR><A STYLE=\\\"font-size: 11;\\\" href=\\\"javascript:assignAllFleetID();\\\">Repeat</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"3\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"text\\\" CLASS=\\\"tbltextbox\\\" SIZE=\\\"3\\\" NAME=\\\"FLEET_ID\", \"\\\" VALUE=\\\"\\\"></TD>\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\">Fleet Name<BR><A STYLE=\\\"font-size: 11;\\\" href=\\\"javascript:assignAllFleetName();\\\">Repeat</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"text\\\" CLASS=\\\"tbltextbox\\\" SIZE=\\\"10\\\" NAME=\\\"FLEET_NAME\", \"\\\" VALUE=\\\"\\\"></TD>\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\">Ship Design<BR><A STYLE=\\\"font-size: 11;\\\" href=\\\"javascript:assignAllShipDesign();\\\">Repeat</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"15\\\" ALIGN=\\\"CENTER\\\"><SELECT CLASS=\\\"tbltextbox\\\" NAME=\\\"SHIP_DESIGN\", \"</TD>\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\">Ships<BR><A STYLE=\\\"font-size: 11;\\\" href=\\\"javascript:assignAllShipNumber();\\\">Repeat</A>&nbsp;<A STYLE=\\\"font-size: 11;\\\" href=\\\"javascript:assignAllShipNumberMax();\\\">Max</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"5\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"text\\\" CLASS=\\\"tbltextbox\\\" SIZE=\\\"3\\\" NAME=\\\"SHIP_NUMBER\", \"\\\" VALUE=\\\"\\\"></TD>\");\n";
		Javascript += "SelectedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:selectedAdmiralSwapAndSort(5)\\\">FC</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"10\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
		int columns = 6;

#define COMMANDER_STAT_TITLE(enum, title, type, width) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Javascript.format("SelectedAdmiralTable.addColumn(%d,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:selectedAdmiralSwapAndSort(%d)\\\">%s</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",\
					type, width, columns, title, width);\
			columns++;\
		}

		COMMANDER_STAT_TITLE(EFFICIENCY, "Eff", 0, 10);
		COMMANDER_STAT_TITLE(OFFENSE, "OS", 0, 10);
		COMMANDER_STAT_TITLE(DEFENSE, "DS", 0, 10);
		COMMANDER_STAT_TITLE(MANEUVER, "MV", 0, 10);
		COMMANDER_STAT_TITLE(DETECTION, "DE", 0, 10);
		COMMANDER_STAT_TITLE(ARMADA_CLASS, "AC", 1, 10);
		COMMANDER_STAT_TITLE(ABILITY, "Common&nbsp;Ability", 1, 10);
		COMMANDER_STAT_TITLE(ABILITY, "Racial&nbsp;Ability", 1, 10);
#undef COMMANDER_STAT_TITLE

		CDock *
			ShipPool = aPlayer->get_dock();

		for (int i = 0 ; i < Pool->length(); i++)
		{
			CAdmiral *
				Admiral = (CAdmiral *)Pool->get(i);
			if (AdmiralSet.has(i) == true)
			{
				if (Admiral->get_fleet_number() != 0)
				{
					CFleet *
						Fleet = FleetList->get_by_id(Admiral->get_fleet_number());
					if (Fleet == NULL)
					{
						Admiral->set_fleet_number(0);

						Admiral->type(QUERY_UPDATE);
						STORE_CENTER->store(*Admiral);

						SLOG((char *)format("WARNING : The admiral %s was assigned to a fleet, but the fleet doesn't exist. So the admiral has been moved to the admiral pool.", Admiral->get_nick()));
					}
					else
					{
						ITEM("ERROR_MESSAGE",
								(char *)format(GETTEXT("The admiral %1$s is assigned to the fleet %2$s, so you can't fleet the admiral right now."),
												Admiral->get_nick(), Fleet->get_nick()));
						return output("fleet/form_new_fleet_error.html");
					}
				}
				else
				{
					Javascript.format("SelectedAdmiralTable.addRow(\"|\", \"%d|%d|%d|%d\\\">%s|%d",
							Admiral->get_id(), i, i, i, ShipPool->print_javascript_select(), i);

					Javascript.format("|%d", Admiral->get_fleet_commanding());

#define COMMANDER_STAT(enum, function) \
					if (aPreference->hasCommanderStat(CPreference::enum))\
					{\
						Javascript.format("|%d", Admiral->function());\
					}

					COMMANDER_STAT(EFFICIENCY, get_real_efficiency);
					COMMANDER_STAT(OFFENSE, get_overall_attack);
					COMMANDER_STAT(DEFENSE, get_overall_defense);
					COMMANDER_STAT(MANEUVER, get_maneuver_level);
					COMMANDER_STAT(DETECTION, get_detection_level);
#undef COMMANDER_STAT
#define COMMANDER_STAT(enum, function) \
					if (aPreference->hasCommanderStat(CPreference::enum))\
					{\
						Javascript.format("|%s", Admiral->function());\
					}
					COMMANDER_STAT(ARMADA_CLASS, get_armada_commanding_name);
					COMMANDER_STAT(ABILITY, get_special_ability_name);
					COMMANDER_STAT(ABILITY, get_racial_ability_name);
#undef COMMANDER_STAT
					Javascript += "\");\n";
				}
			}
		}

		ITEM("CONFIRM_TABLE_JAVASCRIPT", Javascript);

		ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

		ITEM("POOL_INFO", aPlayer->get_dock()->dock_info_html());

		ITEM("STRING_CURRENTLY_POSSESSED_FLEET_S_",
			GETTEXT("Currently Possessed Fleet(s)"));

		ITEM("POSSESSED_FLEET_INFO", FleetList->possessed_fleet_info_html());

		return output( "fleet/form_new_fleet_confirm.html" );
	}

//	system_log("end page handler %s", get_name());


}

