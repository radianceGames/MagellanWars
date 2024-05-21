#include "council.h"
#include "archspace.h"
#include "game.h"
#include <cstdio>
#include <libintl.h>
#include "admin.h"

TZone gCouncilTableZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CCouncilTable),
	0,
	0,
	NULL,
	"Zone CCouncilTable"
};

CCouncilTable::CCouncilTable():
	CSortedList(COUNCIL_TABLE_LIMIT, COUNCIL_TABLE_BLOCK)
{
	mOverallRankTable.set_rank_type(CRank::COUNCIL_OVERALL);
	mFleetRankTable.set_rank_type(CRank::COUNCIL_FLEET);
	mPlanetRankTable.set_rank_type(CRank::COUNCIL_PLANET);
	mDiplomaticRankTable.set_rank_type(CRank::COUNCIL_DIPLOMATIC);
}

CCouncilTable::~CCouncilTable()
{
	remove_all();
}

bool
CCouncilTable::free_item(TSomething aItem)
{
	CCouncil
		*Council = (CCouncil *)aItem;
	
	if (!Council) return false;

	delete Council;

	return true;
}

int
CCouncilTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CCouncil
		*Council1 = (CCouncil*)aItem1,
		*Council2 = (CCouncil*)aItem2;

	if (Council1->get_id() > Council2->get_id()) return 1;
	if (Council1->get_id() < Council2->get_id()) return -1;
	return 0;
}

int
CCouncilTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CCouncil
		*Council = (CCouncil*)aItem;

	if (Council->get_id() > (int)aKey) return 1;
	if (Council->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CCouncilTable::remove_council(int aID)
{
	int 
		Index = find_sorted_key((void*)aID);	
	if (Index < 0) return false;

	CCouncil *
		Council = (CCouncil *)get(Index);
	CClusterList *
		ClusterList = Council->get_cluster_list();

	for (int i=0 ; i<ClusterList->length() ; i++)
	{
		CCluster *
			Cluster = (CCluster *)ClusterList->get(i);
		Cluster->remove_council(aID);
	}

	mOverallRankTable.remove_rank(aID);
	mFleetRankTable.remove_rank(aID);
	mPlanetRankTable.remove_rank(aID);
	mDiplomaticRankTable.remove_rank(aID);

	CPurchasedProjectList *
		PurchasedProjectList = Council->get_purchased_project_list();
	for (int i=PurchasedProjectList->length()-1 ; i>=0 ; i--)
	{
		CPurchasedProject *
			Project = (CPurchasedProject *)PurchasedProjectList->get(i);

		Project->type(QUERY_DELETE);
		STORE_CENTER->store(*Project);

		PurchasedProjectList->remove_project(Project->get_id());
	}

 	CMySQL
		MySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
		*User = CONFIG->get_string("Database", "User", NULL),
		*Password = CONFIG->get_string("Database", "Password", NULL),
		*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if ( (Host != NULL && User != NULL && Password != NULL && Database != NULL) && MySQL.open(Host, User, Password, Database))	
    {
	   CString
		      Query;	
       Query.clear();           
       Query.format( "SELECT group_id FROM %s.asbb_groups WHERE group_name='%d'", 
                     Database, Council->get_id());
       MySQL.query( (char*)Query );
       MySQL.use_result();
       if (MySQL.fetch_row())
       {
        MYSQL_ROW aRow = MySQL.row();
        char *aGroupIDString = aRow[0];
        int aGroupID = -1;
        if (aGroupIDString && (aGroupID = atoi(aGroupIDString)))
        {
         MySQL.free_result();
         Query.clear();
         Query.format( "DELETE FROM %s.asbb_groups WHERE group_name='%d'",
                       Database, Council->get_id());
         MySQL.query( (char*)Query );
         MySQL.use_result();
         MySQL.free_result();
         Query.clear();
         Query.format( "DELETE FROM %s.asbb_auth_access WHERE group_id=%d",
                        Database, aGroupID);
         MySQL.query( (char*)Query );
         MySQL.use_result();
         MySQL.free_result();
         Query.format( "DELETE FROM asbb_forums WHERE forum_name='%d'",
                       Council->get_id());
         MySQL.query( (char*)Query );
         MySQL.use_result();
         MySQL.free_result();
         Query.clear();
        }
       }

       MySQL.close();
    }

	//added by zbyte - 2004-03-26
/*	CCouncilActionMergingOffer
		*MyMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(Council, CAction::ACTION_COUNCIL_MERGING_OFFER);
	if (MyMergingOffer != NULL ) {
		CCouncil *SenderCouncil = COUNCIL_TABLE->get_by_id(MyMergingOffer->get_argument());
		if (SenderCouncil != NULL) {
			CCouncilActionMergingOffer
				*TargetMergingOffer = (CCouncilActionMergingOffer *)COUNCIL_ACTION_TABLE->get_by_owner_type(SenderCouncil, CAction::ACTION_COUNCIL_MERGING_OFFER);
		
			TargetMergingOffer->type(QUERY_DELETE);
	                STORE_CENTER->store(*TargetMergingOffer);

			COUNCIL_ACTION_TABLE->remove_action(TargetMergingOffer->get_id());
	                SenderCouncil->remove_action(TargetMergingOffer->get_id());
		}
	
		MyMergingOffer->type(QUERY_DELETE);
		STORE_CENTER->store(*MyMergingOffer);
	}*/
	//end of added code

	CCouncilRelationList *
		CouncilRelationList = Council->get_relation_list();
	for (int i=CouncilRelationList->length()-1 ; i>=0 ; i--)
	{
		CCouncilRelation *
			Relation = (CCouncilRelation *)CouncilRelationList->get(i);

		Relation->type(QUERY_DELETE);
		STORE_CENTER->store(*Relation);

		CCouncil
			*Council1 = Relation->get_council1(),
			*Council2 = Relation->get_council2();

		Council1->remove_relation(Relation->get_id());
		Council2->remove_relation(Relation->get_id());

		COUNCIL_RELATION_TABLE->remove_relation(Relation->get_id());
	}

	CCouncilMessageBox *
		MessageBox = Council->get_council_message_box();
	for (int i=MessageBox->length()-1 ; i>=0 ; i--)
	{
		CCouncilMessage *
			CouncilMessage = (CCouncilMessage *)MessageBox->get(i);

		CouncilMessage->type(QUERY_DELETE);
		STORE_CENTER->store(*CouncilMessage);

		MessageBox->remove_council_message(CouncilMessage->get_id());
	}

	CCouncilActionList *
		CouncilActionList = Council->get_council_action_list();
	for (int i=CouncilActionList->length()-1 ; i>=0 ; i--)
	{
		CCouncilAction *
			CouncilAction = (CCouncilAction *)CouncilActionList->get(i);
		if (CouncilAction->get_type() == CCouncilAction::ACTION_COUNCIL_MERGING_OFFER)
		{
			int
				TargetCouncilID = CouncilAction->get_argument();
			CCouncil *
				TargetCouncil = COUNCIL_TABLE->get_by_id(TargetCouncilID);
			if (TargetCouncil == NULL)
			{
				SLOG("ERROR : TargetCouncil is NULL in CCouncilTable::remove_council(), TargetCouncilID = %d, CouncilAction->get_id() = %d", TargetCouncilID, CouncilAction->get_id());
			}
			else
			{
				CCouncilActionList *
					TargetCouncilActionList = TargetCouncil->get_council_action_list();
				for (int j=CouncilActionList->length()-1 ; j>=0 ; j--)
				{
					CCouncilAction *
						TargetCouncilAction = (CCouncilAction *)TargetCouncilActionList->get(j);
					if (TargetCouncilAction == NULL)
                    {
                        SLOG("ERROR : TargetCouncilAction is NULL in CCouncilTable::remove_council(), TargetCouncilID = %d, CouncilAction->get_id() = %d", TargetCouncilID, CouncilAction->get_id());                        
                    }    
					else if (TargetCouncilAction->get_type() == CCouncilAction::ACTION_COUNCIL_MERGING_OFFER &&
						TargetCouncilAction->get_argument() == Council->get_id())
					{
						TargetCouncilAction->type(QUERY_DELETE);
						STORE_CENTER->store(*TargetCouncilAction);

						TargetCouncilActionList->remove_action(TargetCouncilAction->get_id());
						COUNCIL_ACTION_TABLE->remove_action(TargetCouncilAction->get_id());
					}
				}
			}
		}

		CouncilAction->type(QUERY_DELETE);
		STORE_CENTER->store(*CouncilAction);

		CouncilActionList->remove_action(CouncilAction->get_id());
		COUNCIL_ACTION_TABLE->remove_action(CouncilAction->get_id());
	}

	CAdmissionList *
		AdmissionList = Council->get_admission_list();
	for (int i=AdmissionList->length()-1 ; i>=0 ; i--)
	{
		CAdmission *
			Admission = (CAdmission *)AdmissionList->get(i);
		CPlayer *
			Owner = Admission->get_player();
		Owner->set_admission(NULL);
		Owner->time_news((char *)format(GETTEXT("Your admission request has been canceled because the council %1$s disappeared."), Council->get_nick()));

		Admission->type(QUERY_DELETE);
		STORE_CENTER->store(*Admission);

		AdmissionList->remove_admission(Owner->get_game_id());
	}

	DETACHMENT_PLAYER_COUNCIL_TABLE->remove_from_db_memory(Council);
	DETACHMENT_COUNCIL_COUNCIL_TABLE->remove_from_db_memory(Council);

	Council->type(QUERY_DELETE);
	STORE_CENTER->store(*Council);

	// added by thedaz for delete council forum ->
	/*
		DELETE catagories/forums/forum_mods/users
	*/
/*	SLOG("THEDAZ: Delete Council Forum");

	char query[256];
	char str[256];
	char *council_name;

	council_name = add_slashes(Council->get_name());

	MYSQL db;

	mysql_init(&db);

	if (!mysql_real_connect(&db, "localhost", "space", "fa75L5oC", "CouncilForum", 0, NULL, 0))
	{
		sprintf(str, "THEDAZ: Failed to connect to database: Error: %s\n", mysql_error(&db));
		SLOG(str); 
	}

	sprintf(query, "DELETE FROM catagories WHERE cat_id=%d", Council->get_id());
*/
	/* ## DELETE catagories ## */
/*	if (mysql_query(&db, query) == -1)
	{ 
		sprintf(str, "THEDAZ: Failed to delete database: Error: %s\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to delete cat database (council) -\t\t%d %s", Council->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "DELETE FROM forums WHERE cat_id=%d", Council->get_id());

	/* ## DELETE forums ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to delete database: Error: %s\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to delete forums database (council) -\t\t%d %s", Council->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "DELETE FROM forum_mods WHERE cat_id=%d", Council->get_id());

	/* ## DELETE forum_mods ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to delete database: Error: %s\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to delete forum_mods database (council) -\t%d %s", Council->get_id(), council_name);
		SLOG(str);
	}

	sprintf(query, "DELETE FROM users WHERE cat_id=%d", Council->get_id());

	/* ## DELETE users ## */
/*	if (mysql_query(&db, query) == -1)
	{
		sprintf(str, "THEDAZ: Failed to delete database: Error: %s\n", mysql_error(&db));
		SLOG(str);
	}
	else
	{
		sprintf(str, "THEDAZ: Successed to delete users database (council) -\t\t%d %s", Council->get_id(), council_name);
		SLOG(str);
	}

	mysql_close(&db);
	// added by thedaz for delete council forum <-
*/
	return CSortedList::remove(Index);
}

int 
CCouncilTable::add_council(CCouncil* aCouncil)
{
	if (!aCouncil) return 0;

	if (find_sorted_key((TConstSomething)aCouncil->get_id()) >= 0) 
		return 0;
	insert_sorted(aCouncil);

	return aCouncil->get_id();
}

CCouncil*
CCouncilTable::get_by_id(int aID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aID);

	if (Index < 0) return NULL;

	return (CCouncil*)get(Index);
}

CCouncil *
CCouncilTable::get_by_name(char *aName)
{
	if (!aName) return NULL;
	if (strlen(aName) == 0) return NULL;

	for (int i=0 ; i<length() ; i++)
	{
		CCouncil *
			Council = (CCouncil *)get(i);
		if (!strcmp(Council->get_name(), aName)) return Council;
	}

	return NULL;
}

bool
CCouncilTable::load(CMySQL &aMySQL)
{
	SLOG( "Council loading" );

	//aMySQL.query( "LOCK TABLE council READ" );
	aMySQL.query( "SELECT "
					"id, "
					"speaker, "
					"name, "
					"slogan, "
					"production, "
					"honor, "
					"auto_assign, "
					"home_cluster_id, "
					"merge_penalty_time "
					"FROM council" );

	aMySQL.use_result();

	while(aMySQL.fetch_row())
		add_council(new CCouncil(aMySQL.row()));

	aMySQL.free_result();
	//aMySQL.query( "UNLOCK TABLES" );

	refresh_rank_table();

	SLOG("%d councils are loaded", length());

	return true;
}

/*
CCouncil *
CCouncilTable::get_random_council()
{
	return (CCouncil *)get(number(length())-1);
}*/

const char *
CCouncilTable::html_select_council_except( CCouncil *aCouncil )
{
	static CString
		Buf;

	Buf.clear();

	Buf = "<SELECT NAME = \"COUNCIL_SELECT\">\n";

	bool
		AnyCouncil = false;

	for(int i=0; i<length(); i++)
	{
		CCouncil *
			Council = (CCouncil *)get(i);

		if (Council->get_id() == aCouncil->get_id()) continue; 	
		if (Council->get_speaker() == NULL) continue; 

		Buf.format( "<OPTION SELECTED VALUE = \"%d\"> %s </OPTION>\n", 
				Council->get_id(), Council->get_nick() );

		AnyCouncil = true;
	}

	Buf += "</SELECT>\n";

	if (AnyCouncil)
	{
		return (char *)Buf;
	} else
	{
		return NULL;
	}
}

void
CCouncilTable::refresh_rank_table()
{
	mOverallRankTable.remove_all();
	mFleetRankTable.remove_all();
	mPlanetRankTable.remove_all();
	mDiplomaticRankTable.remove_all();

	CRank *
		Rank = NULL;
	for (int i=0 ; i<length() ; i++)
	{
		CCouncil *
			Council = (CCouncil *)get(i);

		Council->refresh_power();

		Rank = new CRank();
		Rank->set_id(Council->get_id());
		Rank->set_power(Council->get_power());
		mOverallRankTable.add_rank(Rank);

		CPlayerList *
			Members = Council->get_members();

		int
			FleetPower = 0,
			FleetNumber = 0,
			PlanetPower = 0,
			PlanetNumber = 0;

		for (int i=0 ; i<Members->length() ; i++)
		{
			CPlayer *
				Member = (CPlayer *)Members->get(i);
			CFleetList *
				FleetList = Member->get_fleet_list();
			CPlanetList *
				PlanetList = Member->get_planet_list();

			FleetPower += FleetList->get_power();
			FleetNumber += FleetList->length();
			PlanetPower += PlanetList->get_power();
			PlanetNumber += PlanetList->length();
		}

		Rank = new CRank();
		Rank->set_id(Council->get_id());
		Rank->set_power(FleetPower);
		Rank->set_number(FleetNumber);
		mFleetRankTable.add_rank(Rank);

		Rank = new CRank();
		Rank->set_id(Council->get_id());
		Rank->set_power(PlanetPower);
		Rank->set_number(PlanetNumber);
		mPlanetRankTable.add_rank(Rank);

		int
			DiplomaticPower = 0;
		bool
			Subordinated = false;
		CCouncilRelationList *
			RelationList = Council->get_relation_list();

		for (int i=0 ; i<RelationList->length() ; i++)
		{
			CCouncilRelation *
				Relation = (CCouncilRelation *)RelationList->get(i);
			CCouncil
				*Council1 = Relation->get_council1(),
				*Council2 = Relation->get_council2();
			CCouncil *
				TargetCouncil = NULL;

			if (Council1->get_id() == Council->get_id())
			{
				TargetCouncil = Council2;
			} else if (Council2->get_id() == Council->get_id())
			{
				TargetCouncil = Council1;
			}

			if (!TargetCouncil)
			{
				SLOG("ERROR : Wrong council relation(ID : %d)", Relation->get_id());
				continue;
			}

			switch (Relation->get_relation())
			{
				case CRelation::RELATION_ALLY :
					DiplomaticPower += TargetCouncil->get_power()/4;
					break;

				case CRelation::RELATION_PEACE :
					DiplomaticPower += TargetCouncil->get_power()/10;
					break;

				case CRelation::RELATION_SUBORDINARY :
					if (Council1->get_id() == Council->get_id())
					{
						DiplomaticPower += TargetCouncil->get_power();
					} else
					{
						Subordinated = true;
					}
					break;

				default :
					break;
			}
		}

		if (Subordinated) DiplomaticPower /= 2;

		Rank = new CRank();
		Rank->set_id(Council->get_id());
		Rank->set_power(DiplomaticPower);
		mDiplomaticRankTable.add_rank(Rank);
	}

	mOverallRankTable.set_time(time(0));
	mFleetRankTable.set_time(time(0));
	mPlanetRankTable.set_time(time(0));
	mDiplomaticRankTable.set_time(time(0));
}

void
CCouncilTable::add_council_rank(CCouncil *aCouncil)
{
	if (aCouncil == NULL) return;

	aCouncil->refresh_power();

	CRank
		*OverallRank = new CRank(),
		*FleetRank = new CRank(),
		*PlanetRank = new CRank(),
		*DiplomaticRank = new CRank();
	OverallRank->initialize_from_council(aCouncil, CRank::COUNCIL_OVERALL);
	FleetRank->initialize_from_council(aCouncil, CRank::COUNCIL_FLEET);
	PlanetRank->initialize_from_council(aCouncil, CRank::COUNCIL_PLANET);
	DiplomaticRank->initialize_from_council(aCouncil, CRank::COUNCIL_DIPLOMATIC);

	mOverallRankTable.add_rank(OverallRank);
	mFleetRankTable.add_rank(FleetRank);
	mPlanetRankTable.add_rank(PlanetRank);
	mDiplomaticRankTable.add_rank(DiplomaticRank);
}

void
CCouncilTable::remove_council_rank(int aCouncilID)
{
	mOverallRankTable.remove_rank(aCouncilID);
	mFleetRankTable.remove_rank(aCouncilID);
	mPlanetRankTable.remove_rank(aCouncilID);
	mDiplomaticRankTable.remove_rank(aCouncilID);
}

const char*
CCouncilTable::offer_select_html(CPlayer *aPlayer)
{
	static CString HTML;

	HTML = "<SELECT NAME=\"COUNCIL_SELECT\">\n"
			"<OPTION SELECTED VALUE=0>";
	HTML +=	GETTEXT("None");
	HTML += "</OPTION>\n";

	if (aPlayer->get_council()->get_speaker_id() != aPlayer->get_game_id())
	{
		for (int i=0 ; i<length() ; i++)
		{
			CCouncil *
				Council = (CCouncil *)get(i);
			if (Council->get_id() == aPlayer->get_council_id()) continue;
			if (Council->get_speaker() == NULL) continue;

			HTML.format("<OPTION VALUE=%d>%s</OPTION>\n", 
							Council->get_id(), Council->get_nick());
		}
	}
	HTML += "</SELECT>\n";

	return (char*)HTML;
}
