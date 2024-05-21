#include "admiral.h"
#include "define.h"
#include <libintl.h>
#include "fleet.h"
#include "player.h"
#include "archspace.h"

CAdmiralList::CAdmiralList(): CSortedList(50, 50)
{
}

CAdmiralList::~CAdmiralList()
{
	remove_all();
}

bool
	CAdmiralList::free_item(TSomething aItem)
{
	CAdmiral
		*Admiral = (CAdmiral *)aItem;

	if (!Admiral) return false;

	delete Admiral;

	return true;
}

int
	CAdmiralList::compare(TSomething aItem1, TSomething aItem2) const
{
	CAdmiral
		*Admiral1 = (CAdmiral*)aItem1,
	*Admiral2 = (CAdmiral*)aItem2;

	if (Admiral1->get_id() > Admiral2->get_id()) return 1;
	if (Admiral1->get_id() < Admiral2->get_id()) return -1;
	return 0;
}

int
	CAdmiralList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CAdmiral
		*Admiral = (CAdmiral*)aItem;

	if (Admiral->get_id() > (int)aKey) return 1;
	if (Admiral->get_id() < (int)aKey) return -1;
	return 0;
}

bool
	CAdmiralList::remove_admiral(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
	CAdmiralList::remove_without_free_admiral(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return false;

	return remove_without_free(Index);
}



bool
	CAdmiralList::add_admiral(CAdmiral *aAdmiral)
{
	if (!aAdmiral) return false;

	if (find_sorted_key((TConstSomething)aAdmiral->get_id()) >= 0)
		return false;

	insert_sorted(aAdmiral);

	return true;
}

CAdmiral*
CAdmiralList::get_by_id(int aID)
{
	if (!length()) return NULL;

	int Index = find_sorted_key((TSomething)aID);

	if (Index < 0) return NULL;

	return (CAdmiral*)get(Index);
}

bool
	CAdmiralList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)get(i);
		if (Admiral->is_changed() == false) continue;

		Admiral->type(QUERY_UPDATE);
		STORE_CENTER->store(*Admiral);
	}

	return true;
}

char *
	CAdmiralList::attached_fleet_commander_info_html()
{
	static CString
		Info;
	Info.clear();

	if (!length())
	{
		Info = GETTEXT("There are no fleet commanders attached to a fleet.");
		return (char *)Info;
	}

	Info += "\t<div id=\"AttachedAdmirals\"><!-- Attached fleet commander info !--></div>\n";
	return (char *)Info;
}

char *
	CAdmiralList::attached_fleet_commander_info_javascript(CPlayer *aPlayer)
{
	static CString
		Info;
	Info.clear();

	CPreference *aPreference =
		parent->get_preference();

	if (!length())
	{
		Info += "/***** NO ATTACHED ADMIRALS *****/";
		return (char *)Info;
	}

	Info += "function attachedAdmiralSwapAndSort(column)\n{\n";
	Info += "\tvar Action = \n\t{\n\t\tNONE : -1,\n\t\tASCENDING : 0,\n\t\tDESCENDING : 1\n\t}\n";
	Info += "\tswitch (AttachedAdmiralTable.getSortingOrder(column))\n\t{\n";
	Info += "\t\tcase Action.NONE:\n\t\tAttachedAdmiralTable.addPriorityRank(column, Action.ASCENDING);\n\t\tif (AttachedAdmiralTable.getPriorityRank(column) > 4) AttachedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\telse AttachedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(AttachedAdmiralTable.getPriorityRank(column)) + 1) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.ASCENDING:\n\t\tAttachedAdmiralTable.setSortingOrder(column, Action.DESCENDING);\n\t\tif (AttachedAdmiralTable.getPriorityRank(column) > 4) AttachedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-12.gif\\\"></TH>\");\n\t\telse AttachedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(AttachedAdmiralTable.getPriorityRank(column)) + 6) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.DESCENDING:\n\t\tAttachedAdmiralTable.removePriorityRank(column);\n\t\tAttachedAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"</TH>\");\n";
	Info += "\t\tfor (var col = 0; col < AttachedAdmiralTable.getTotalColumns(); col++)\n\t\t{\n\t\t\tif (AttachedAdmiralTable.getSortingOrder(col) == Action.ASCENDING)\n\t\t\t{\n\t\t\t\tif (AttachedAdmiralTable.getPriorityRank(col) > 4) AttachedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse AttachedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(AttachedAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n";
	Info += "\t\t\telse if (AttachedAdmiralTable.getSortingOrder(col) == Action.DESCENDING)\n\t\t\t{\n\t\t\t\tif (AttachedAdmiralTable.getPriorityRank(col) > 4) AttachedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse AttachedAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, AttachedAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(AttachedAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n\t\t}\n\t\tbreak;\n";
	Info += "\t}\n";
	Info += "\tAttachedAdmiralTable.sort();\n";
	Info += "\tdocument.getElementById(\"AttachedAdmirals\").innerHTML = AttachedAdmiralTable.getTableHTML();\n";
	Info += "}\n\n";
	Info += "AttachedAdmiralTable = new Table();\n";
	Info += "AttachedAdmiralTable.setTableAttributes(\"WIDTH=\\\"550\\\" BORDER=\\\"1\\\" CELLSPACING=\\\"0\\\" CELLPADDING=\\\"0\\\" BORDERCOLOR=\\\"#2A2A2A\\\"\");\n";
	Info += "AttachedAdmiralTable.setHeaderRowAttributes(\"STYLE=\\\"vertical-align: bottom;\\\" BGCOLOR=\\\"#171717\\\"\");\n";
	Info += "AttachedAdmiralTable.setSortDelimiter(\"#SORT#\");\n";
	Info += "AttachedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:attachedAdmiralSwapAndSort(0)\\\">ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	Info += "AttachedAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:attachedAdmiralSwapAndSort(1)\\\">Name</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\" ALIGN=\\\"CENTER\\\"><A HREF=\\\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=\", \"</A></TD>\");\n";

	int columns = 2;

#define COMMANDER_STAT_TITLE(enum, title, type, width) \
	if (aPreference->hasCommanderStat(CPreference::enum))\
	{\
		Info.format("AttachedAdmiralTable.addColumn(%d,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:attachedAdmiralSwapAndSort(%d)\\\">%s</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",\
				type, width, columns, title, width);\
		columns++;\
	}

	COMMANDER_STAT_TITLE(LEVEL, "Level", 0, 51);
	COMMANDER_STAT_TITLE(EXP, "Exp", 0, 44);
	COMMANDER_STAT_TITLE(FLEET_COMMANDING, "Fleet<BR>Commanding", 0, 102);
	COMMANDER_STAT_TITLE(EFFICIENCY, "Efficiency", 0, 42);
	/*COMMANDER_STAT_TITLE(SIEGE_PLANET, "Siege<BR>Planet", 0, 44);
	COMMANDER_STAT_TITLE(BLOCKADE, "Blockade", 0, 40);
	COMMANDER_STAT_TITLE(RAID, "Raid", 0, 40);
	COMMANDER_STAT_TITLE(PRIVATEER, "Privateer", 0, 40);
	COMMANDER_STAT_TITLE(SIEGE_REPELLING, "Siege<BR>Repelling", 0, 44);
	COMMANDER_STAT_TITLE(BREAK_BLOCKADE, "Break<BR>Blockade", 0, 44);
	COMMANDER_STAT_TITLE(PREVENT_RAID, "Prevent<BR>Raid", 0, 44);*/
	COMMANDER_STAT_TITLE(OFFENSE, "Offense", 0, 40);
	COMMANDER_STAT_TITLE(DEFENSE, "Defense", 0, 40);
	COMMANDER_STAT_TITLE(MANEUVER, "Maneuver", 0, 40);
	COMMANDER_STAT_TITLE(DETECTION, "Detection", 0, 40);
	//COMMANDER_STAT_TITLE(INTERPRETATION, "Interpretation", 0, 44);
	COMMANDER_STAT_TITLE(ARMADA_CLASS, "Armada<BR>Class", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Common<BR>Ability", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Racial<BR>Ability", 1, 44);
#undef COMMANDER_STAT_TITLE

	Info.format("AttachedAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:attachedAdmiralSwapAndSort(%d)\\\">Fleet ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",
		columns);
	columns++;
	Info.format("AttachedAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:attachedAdmiralSwapAndSort(%d)\\\">Fleet&nbsp;Name</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",
		columns);

	Info += "AttachedAdmiralTable.addRows(\"|\", new Array(";

	for (int i = 0 ; i < length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)get(i);
		Info.format("\"%d|%d\\\">#SORT#%s#SORT#",
		Admiral->get_id(), Admiral->get_id(), Admiral->get_name());

#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%d",\
					Admiral->function());\
		}

		COMMANDER_STAT(LEVEL, get_level);
		COMMANDER_STAT(EXP, get_exp);
		COMMANDER_STAT(FLEET_COMMANDING, get_fleet_commanding);
		COMMANDER_STAT(EFFICIENCY, get_real_efficiency);
/*		COMMANDER_STAT(SIEGE_PLANET, get_siege_planet_level);
		COMMANDER_STAT(BLOCKADE, get_blockade_level);
		COMMANDER_STAT(RAID, get_raid_level);
		COMMANDER_STAT(PRIVATEER, get_privateer_level);
		COMMANDER_STAT(SIEGE_REPELLING, get_siege_repelling_level);
		COMMANDER_STAT(BREAK_BLOCKADE, get_break_blockade_level);
		COMMANDER_STAT(PREVENT_RAID, get_prevent_raid_level);*/
		COMMANDER_STAT(OFFENSE, get_overall_attack);
		COMMANDER_STAT(DEFENSE, get_overall_defense);
		COMMANDER_STAT(MANEUVER, get_maneuver_level);
		COMMANDER_STAT(DETECTION, get_detection_level);
		//COMMANDER_STAT(INTERPRETATION, get_interpretation_level);
#undef COMMANDER_STAT
#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%s",\
					Admiral->function());\
		}
		COMMANDER_STAT(ARMADA_CLASS, get_armada_commanding_name);
		COMMANDER_STAT(ABILITY, get_special_ability_name);
		COMMANDER_STAT(ABILITY, get_racial_ability_name);
#undef COMMANDER_STAT

		CFleetList *
		FleetList = aPlayer->get_fleet_list();
		CFleet *
		Fleet = FleetList->get_by_id(Admiral->get_fleet_number());
		if( Fleet )
		{
			Info.format("|%d|%s\"",
				Fleet->get_id(), Fleet->get_name());
		}
		else
		{
			Info += "|-\"";
		}
		if (i + 1 >= length()) Info += ")";
		else Info += ",\n\t";
	}
	Info += ");\n";
	return (char *)Info;
}

char *
	CAdmiralList::pool_fleet_commander_info_html()
{
	static CString
		Info;
	Info.clear();

	if (!length())
	{
		Info = GETTEXT("There are no fleet commanders in the pool.");
		return (char *)Info;
	}

	Info += "\t<div id=\"PooledAdmirals\"><!-- Pooled fleet commander info !--></div>\n";
	return (char *)Info;
}

char *
	CAdmiralList::pool_fleet_commander_info_javascript(CPlayer *aPlayer)
{
	static CString
		Info;
	Info.clear();

	CPreference *aPreference =
		parent->get_preference();

	if (!length())
	{
		Info += "/***** NO POOLED ADMIRALS *****/";
		return (char *)Info;
	}

	Info += "function pooledAdmiralSwapAndSort(column)\n{\n";
	Info += "\tvar Action = \n\t{\n\t\tNONE : -1,\n\t\tASCENDING : 0,\n\t\tDESCENDING : 1\n\t}\n";
	Info += "\tswitch (PooledAdmiralTable.getSortingOrder(column))\n\t{\n";
	Info += "\t\tcase Action.NONE:\n\t\tPooledAdmiralTable.addPriorityRank(column, Action.ASCENDING);\n\t\tif (PooledAdmiralTable.getPriorityRank(column) > 4) PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\telse PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(column)) + 1) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.ASCENDING:\n\t\tPooledAdmiralTable.setSortingOrder(column, Action.DESCENDING);\n\t\tif (PooledAdmiralTable.getPriorityRank(column) > 4) PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-12.gif\\\"></TH>\");\n\t\telse PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(column)) + 6) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.DESCENDING:\n\t\tPooledAdmiralTable.removePriorityRank(column);\n\t\tPooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"</TH>\");\n";
	Info += "\t\tfor (var col = 0; col < PooledAdmiralTable.getTotalColumns(); col++)\n\t\t{\n\t\t\tif (PooledAdmiralTable.getSortingOrder(col) == Action.ASCENDING)\n\t\t\t{\n\t\t\t\tif (PooledAdmiralTable.getPriorityRank(col) > 4) PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n";
	Info += "\t\t\telse if (PooledAdmiralTable.getSortingOrder(col) == Action.DESCENDING)\n\t\t\t{\n\t\t\t\tif (PooledAdmiralTable.getPriorityRank(col) > 4) PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n\t\t}\n\t\tbreak;\n";
	Info += "\t}\n";
	Info += "\tPooledAdmiralTable.sort();\n";
	Info += "\tdocument.getElementById(\"PooledAdmirals\").innerHTML = PooledAdmiralTable.getTableHTML();\n";
	Info += "}\n\n";
	Info += "PooledAdmiralTable = new Table();\n";
	Info += "PooledAdmiralTable.setTableAttributes(\"WIDTH=\\\"550\\\" BORDER=\\\"1\\\" CELLSPACING=\\\"0\\\" CELLPADDING=\\\"0\\\" BORDERCOLOR=\\\"#2A2A2A\\\"\");\n";
	Info += "PooledAdmiralTable.setHeaderRowAttributes(\"STYLE=\\\"vertical-align: bottom;\\\" BGCOLOR=\\\"#171717\\\"\");\n";
	Info += "PooledAdmiralTable.setSortDelimiter(\"#SORT#\");\n";
	Info += "PooledAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(0)\\\">ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	Info += "PooledAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(1)\\\">Name</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\" ALIGN=\\\"CENTER\\\"><A HREF=\\\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=\", \"</A></TD>\");\n";

	int columns = 2;

#define COMMANDER_STAT_TITLE(enum, title, type, width) \
	if (aPreference->hasCommanderStat(CPreference::enum))\
	{\
		Info.format("PooledAdmiralTable.addColumn(%d,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(%d)\\\">%s</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",\
				type, width, columns, title, width);\
		columns++;\
	}

	COMMANDER_STAT_TITLE(LEVEL, "Level", 0, 51);
	COMMANDER_STAT_TITLE(EXP, "Exp", 0, 44);
	COMMANDER_STAT_TITLE(FLEET_COMMANDING, "Fleet<BR>Commanding", 0, 102);
	COMMANDER_STAT_TITLE(EFFICIENCY, "Efficiency", 0, 42);
	/*COMMANDER_STAT_TITLE(SIEGE_PLANET, "Siege<BR>Planet", 0, 44);
	COMMANDER_STAT_TITLE(BLOCKADE, "Blockade", 0, 40);
	COMMANDER_STAT_TITLE(RAID, "Raid", 0, 40);
	COMMANDER_STAT_TITLE(PRIVATEER, "Privateer", 0, 40);
	COMMANDER_STAT_TITLE(SIEGE_REPELLING, "Siege<BR>Repelling", 0, 44);
	COMMANDER_STAT_TITLE(BREAK_BLOCKADE, "Break<BR>Blockade", 0, 44);
	COMMANDER_STAT_TITLE(PREVENT_RAID, "Prevent<BR>Raid", 0, 44);*/
	COMMANDER_STAT_TITLE(OFFENSE, "Offense", 0, 40);
	COMMANDER_STAT_TITLE(DEFENSE, "Defense", 0, 40);
	COMMANDER_STAT_TITLE(MANEUVER, "Maneuver", 0, 40);
	COMMANDER_STAT_TITLE(DETECTION, "Detection", 0, 40);
	//COMMANDER_STAT_TITLE(INTERPRETATION, "Interpretation", 0, 44);
	COMMANDER_STAT_TITLE(ARMADA_CLASS, "Armada<BR>Class", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Common<BR>Ability", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Racial<BR>Ability", 1, 44);
#undef COMMANDER_STAT_TITLE

	Info += "PooledAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\">Dismiss<INPUT TYPE=checkbox onClick=allCheck()></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"checkbox\\\" NAME=\\\"ADMIRAL\", \"\\\"></TD>\");\n";

	Info += "PooledAdmiralTable.addRows(\"|\", new Array(";

	for (int i = 0 ; i < length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)get(i);
		Info.format("\"%d|%d\\\">#SORT#%s#SORT#",
			Admiral->get_id(), Admiral->get_id(), Admiral->get_name());

#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%d",\
					Admiral->function());\
		}

		COMMANDER_STAT(LEVEL, get_level);
		COMMANDER_STAT(EXP, get_exp);
		COMMANDER_STAT(FLEET_COMMANDING, get_fleet_commanding);
		COMMANDER_STAT(EFFICIENCY, get_real_efficiency);
		/*COMMANDER_STAT(SIEGE_PLANET, get_siege_planet_level);
		COMMANDER_STAT(BLOCKADE, get_blockade_level);
		COMMANDER_STAT(RAID, get_raid_level);
		COMMANDER_STAT(PRIVATEER, get_privateer_level);
		COMMANDER_STAT(SIEGE_REPELLING, get_siege_repelling_level);
		COMMANDER_STAT(BREAK_BLOCKADE, get_break_blockade_level);
		COMMANDER_STAT(PREVENT_RAID, get_prevent_raid_level);*/
		COMMANDER_STAT(OFFENSE, get_overall_attack);
		COMMANDER_STAT(DEFENSE, get_overall_defense);
		COMMANDER_STAT(MANEUVER, get_maneuver_level);
		COMMANDER_STAT(DETECTION, get_detection_level);
		//COMMANDER_STAT(INTERPRETATION, get_interpretation_level);
#undef COMMANDER_STAT
#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%s",\
					Admiral->function());\
		}
		COMMANDER_STAT(ARMADA_CLASS, get_armada_commanding_name);
		COMMANDER_STAT(ABILITY, get_special_ability_name);
		COMMANDER_STAT(ABILITY, get_racial_ability_name);
#undef COMMANDER_STAT

		Info.format("|%d\"", i);
		if (i + 1 >= length()) Info += ")";
		else Info += ",\n\t";
	}
	Info += ");\n";

	return (char *)Info;
}

char *
	CAdmiralList::fleet_commander_list_html()
{
	static CString
		Info;
	Info.clear();

	if (!length())
	{
		Info = GETTEXT("There are no fleet commanders in the pool.");
		return (char *)Info;
	}

	Info += "\t<div id=\"PooledAdmirals\"><!-- Pooled fleet commander info !--></div>\n";
	return (char *)Info;
}

char *
	CAdmiralList::fleet_commander_list_javascript(CPlayer *aPlayer)
{
	static CString
		Info;
	Info.clear();

	CPreference *aPreference =
		parent->get_preference();

	if (!length())
	{
		Info += "/***** NO POOLED ADMIRALS *****/";
		return (char *)Info;
	}

	Info += "function pooledAdmiralSwapAndSort(column)\n{\n";
	Info += "\tvar Action = \n\t{\n\t\tNONE : -1,\n\t\tASCENDING : 0,\n\t\tDESCENDING : 1\n\t}\n";
	Info += "\tswitch (PooledAdmiralTable.getSortingOrder(column))\n\t{\n";
	Info += "\t\tcase Action.NONE:\n\t\tPooledAdmiralTable.addPriorityRank(column, Action.ASCENDING);\n\t\tif (PooledAdmiralTable.getPriorityRank(column) > 4) PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\telse PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(column)) + 1) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.ASCENDING:\n\t\tPooledAdmiralTable.setSortingOrder(column, Action.DESCENDING);\n\t\tif (PooledAdmiralTable.getPriorityRank(column) > 4) PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-12.gif\\\"></TH>\");\n\t\telse PooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(column)) + 6) + \".gif\\\"></TH>\");\t\tbreak;\n";
	Info += "\t\tcase Action.DESCENDING:\n\t\tPooledAdmiralTable.removePriorityRank(column);\n\t\tPooledAdmiralTable.setColumnData(column, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(column, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"</TH>\");\n";
	Info += "\t\tfor (var col = 0; col < PooledAdmiralTable.getTotalColumns(); col++)\n\t\t{\n\t\t\tif (PooledAdmiralTable.getSortingOrder(col) == Action.ASCENDING)\n\t\t\t{\n\t\t\t\tif (PooledAdmiralTable.getPriorityRank(col) > 4) PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n";
	Info += "\t\t\telse if (PooledAdmiralTable.getSortingOrder(col) == Action.DESCENDING)\n\t\t\t{\n\t\t\t\tif (PooledAdmiralTable.getPriorityRank(col) > 4) PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-11.gif\\\"></TH>\");\n\t\t\t\telse PooledAdmiralTable.setColumnData(col, ColumnData.HEADER_HTML, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).substring(0, PooledAdmiralTable.getColumnData(col, ColumnData.HEADER_HTML).indexOf(\"</A>\") + 4) + \"<BR><IMG src=\\\"http://magellanwars.com/image/as_game/admrlArrow-\" + (parseInt(PooledAdmiralTable.getPriorityRank(col)) + 1) + \".gif\\\"></TH>\");\n\t\t\t}\n\t\t}\n\t\tbreak;\n";
	Info += "\t}\n";
	Info += "\tPooledAdmiralTable.sort();\n";
	Info += "\tdocument.getElementById(\"PooledAdmirals\").innerHTML = PooledAdmiralTable.getTableHTML();\n";
	Info += "}\n\n";
	Info += "PooledAdmiralTable = new Table();\n";
	Info += "PooledAdmiralTable.setTableAttributes(\"WIDTH=\\\"550\\\" BORDER=\\\"1\\\" CELLSPACING=\\\"0\\\" CELLPADDING=\\\"0\\\" BORDERCOLOR=\\\"#2A2A2A\\\"\");\n";
	Info += "PooledAdmiralTable.setHeaderRowAttributes(\"STYLE=\\\"vertical-align: bottom;\\\" BGCOLOR=\\\"#171717\\\"\");\n";
	Info += "PooledAdmiralTable.setSortDelimiter(\"#SORT#\");\n";
	Info += "PooledAdmiralTable.addColumn(0,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(0)\\\">ID</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"45\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n";
	Info += "PooledAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\"><INPUT TYPE=\\\"checkbox\\\" onClick=\\\"javascript:allCheck();\\\"/></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"134\\\" ALIGN=\\\"CENTER\\\"><INPUT TYPE=\\\"checkbox\\\" onClick=\\\"javascript:shipSel();\\\" NAME=\\\"ADMIRAL\", \"\\\"></TD>\");\n";
	Info += "PooledAdmiralTable.addColumn(1,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(2)\\\">Name</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"118\\\" ALIGN=\\\"CENTER\\\"><A HREF=\\\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=\", \"</A></TD>\");\n";

	int columns = 3;

#define COMMANDER_STAT_TITLE(enum, title, type, width) \
	if (aPreference->hasCommanderStat(CPreference::enum))\
	{\
		Info.format("PooledAdmiralTable.addColumn(%d,true, \"<TH CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\"><A STYLE=\\\"color:#666666\\\" HREF=\\\"javascript:pooledAdmiralSwapAndSort(%d)\\\">%s</A></TH>\", \"<TD CLASS=\\\"tabletxt\\\" WIDTH=\\\"%d\\\" ALIGN=\\\"CENTER\\\">\", \"</TD>\");\n",\
				type, width, columns, title, width);\
		columns++;\
	}

	COMMANDER_STAT_TITLE(LEVEL, "Level", 0, 51);
	COMMANDER_STAT_TITLE(EXP, "Exp", 0, 44);
	COMMANDER_STAT_TITLE(FLEET_COMMANDING, "Fleet<BR>Commanding", 0, 102);
	COMMANDER_STAT_TITLE(EFFICIENCY, "Efficiency", 0, 42);
	/*COMMANDER_STAT_TITLE(SIEGE_PLANET, "Siege<BR>Planet", 0, 44);
	COMMANDER_STAT_TITLE(BLOCKADE, "Blockade", 0, 40);
	COMMANDER_STAT_TITLE(RAID, "Raid", 0, 40);
	COMMANDER_STAT_TITLE(PRIVATEER, "Privateer", 0, 40);
	COMMANDER_STAT_TITLE(SIEGE_REPELLING, "Siege<BR>Repelling", 0, 44);
	COMMANDER_STAT_TITLE(BREAK_BLOCKADE, "Break<BR>Blockade", 0, 44);
	COMMANDER_STAT_TITLE(PREVENT_RAID, "Prevent<BR>Raid", 0, 44);*/
	COMMANDER_STAT_TITLE(OFFENSE, "Offense", 0, 40);
	COMMANDER_STAT_TITLE(DEFENSE, "Defense", 0, 40);
	COMMANDER_STAT_TITLE(MANEUVER, "Maneuver", 0, 40);
	COMMANDER_STAT_TITLE(DETECTION, "Detection", 0, 40);
	//COMMANDER_STAT_TITLE(INTERPRETATION, "Interpretation", 0, 44);
	COMMANDER_STAT_TITLE(ARMADA_CLASS, "Armada<BR>Class", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Common<BR>Ability", 1, 44);
	COMMANDER_STAT_TITLE(ABILITY, "Racial<BR>Ability", 1, 44);
#undef COMMANDER_STAT_TITLE

	Info += "PooledAdmiralTable.addRows(\"|\", new Array(";

	for (int i = 0 ; i < length() ; i++)
	{
		CAdmiral *
		Admiral = (CAdmiral *)get(i);
		Info.format("\"%d|%d|%d\\\">#SORT#%s#SORT#",
		Admiral->get_id(), i, Admiral->get_id(), Admiral->get_name());

#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%d",\
					Admiral->function());\
		}

		COMMANDER_STAT(LEVEL, get_level);
		COMMANDER_STAT(EXP, get_exp);
		COMMANDER_STAT(FLEET_COMMANDING, get_fleet_commanding);
		COMMANDER_STAT(EFFICIENCY, get_real_efficiency);
		/*COMMANDER_STAT(SIEGE_PLANET, get_siege_planet_level);
		COMMANDER_STAT(BLOCKADE, get_blockade_level);
		COMMANDER_STAT(RAID, get_raid_level);
		COMMANDER_STAT(PRIVATEER, get_privateer_level);
		COMMANDER_STAT(SIEGE_REPELLING, get_siege_repelling_level);
		COMMANDER_STAT(BREAK_BLOCKADE, get_break_blockade_level);
		COMMANDER_STAT(PREVENT_RAID, get_prevent_raid_level);*/
		COMMANDER_STAT(OFFENSE, get_overall_attack);
		COMMANDER_STAT(DEFENSE, get_overall_defense);
		COMMANDER_STAT(MANEUVER, get_maneuver_level);
		COMMANDER_STAT(DETECTION, get_detection_level);
		//COMMANDER_STAT(INTERPRETATION, get_interpretation_level);
#undef COMMANDER_STAT
#define COMMANDER_STAT(enum, function) \
		if (aPreference->hasCommanderStat(CPreference::enum))\
		{\
			Info.format("|%s",\
					Admiral->function());\
		}
		COMMANDER_STAT(ARMADA_CLASS, get_armada_commanding_name);
		COMMANDER_STAT(ABILITY, get_special_ability_name);
		COMMANDER_STAT(ABILITY, get_racial_ability_name);
#undef COMMANDER_STAT

		Info.format("|%d\"", i);
		if (i + 1 >= length()) Info += ")";
		else Info += ",\n\t";
	}
	Info += ");\n";

	return (char *)Info;
}

