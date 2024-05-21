#include <libintl.h>
#include "../triggers.h"
#include "../archspace.h"
#include "../council.h"



void
CCronTabEmpireDatabaseSave::handler()
{
   	if (!CGame::mUpdateTurn) return;
	SLOG("Empire Crontab Database Save Start");
	EMPIRE->update_turn();
	SLOG("Empire Crontab Database Save End");
}

void
CCronTabEmpireFleetRegeneration::handler()
{
   	if (!CGame::mUpdateTurn) return;
	SLOG("Empire Crontab Fleet Regeneration Start");

	if (EMPIRE->is_dead() == false)
	{
		CMagistrateList *
			MagistrateList = EMPIRE->get_magistrate_list();
		CFortressList *
			FortressList = EMPIRE->get_fortress_list();
		CEmpireCapitalPlanet *
			EmpireCapitalPlanet = EMPIRE->get_empire_capital_planet();

		for (int i=0 ; i<MagistrateList->length() ; i++)
		{
			CMagistrate *
				Magistrate = (CMagistrate *)MagistrateList->get(i);
			if (Magistrate->is_dead()) continue;
			if (Magistrate->get_fleet_list()->length() < Magistrate->get_number_of_planets() * 20)
			{
				Magistrate->regen_empire_fleets();
			}
		}

		for (int i=0 ; i<FortressList->length() ; i++)
		{
			CFortress *
				Fortress = (CFortress *)FortressList->get(i);
			if (Fortress->is_dead()) continue;
			if (Fortress->get_fleet_list()->length() < 20)
			{
				Fortress->regen_empire_fleets();
			}
		}
//        if (EmpireCapitalPlanet->get_fleet_list()->length() < 20)
		    EmpireCapitalPlanet->regen_empire_fleets();
	}

	SLOG("Empire Crontab Fleet Regeneration End");
}

void
CCronTabEmpireRelation::handler()
{
   	if (!CGame::mUpdateTurn) return;
	SLOG( "Empire Relation Crontab Running Start" );

	if (EMPIRE->is_dead() == false)
	{
		static int
			two_days = 2*24*60*60;

		for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
		{
			CPlayer *
				Player = (CPlayer *)PLAYER_TABLE->get(i);
			if (Player == EMPIRE) continue;
			if (Player->is_dead()) continue;

			Player->change_empire_relation(Player->get_relation_lost_per_day_by_rank());
			Player->expire_empire_demand( two_days );
		}
	}
	
	//this is a good place for bounties... right?
//	OFFERED_BOUNTY_TABLE->refresh_bounties();

	SLOG( "Empire Relation Crontab Running End" );
}

void
CCronTabEmpireDemand::handler()
{
   	if (!CGame::mUpdateTurn) return;
	SLOG( "Empire Demand Crontab Running Start" );

	if (EMPIRE->is_dead() == false)
	{
		int
			MaxRank = PLAYER_TABLE->length();

		for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
		{
			CPlayer *
				Player = (CPlayer *)PLAYER_TABLE->get(i);
			if (Player->get_game_id() == EMPIRE_GAME_ID) continue;
			if (Player->is_dead()) continue;

			CMagistrate *
				Magistrate = MAGISTRATE_LIST->get_by_cluster_id(Player->get_home_cluster_id());
			if (Magistrate == NULL)
			{
				SLOG("ERROR : Magistrate is NULL in CCronTabEmpireDemand::handler(), Player->get_game_id() = %d, Player->get_home_cluster_id() = %d", Player->get_game_id(), Player->get_home_cluster_id());
				continue;
			}
			if (Magistrate->is_dead() == true) continue;

			CCouncil *
				Council = Player->get_council();
			CPlayerList *
				Members = Council->get_members();
			CCouncilRelationList *
				CouncilRelationList = Council->get_relation_list();

			if (Player->get_empire_relation() >= 85)
			{
				CRankTable *
					PlayerRankTable = PLAYER_TABLE->get_overall_rank_table();
				int
					Rank = PlayerRankTable->get_rank_by_id(Player->get_game_id());
				int
					Price,
					NumberToWin = 0,
					NewRank,
					InRank;

				switch (Player->get_court_rank())
				{
					case CPlayer::CR_MARQUIS :
						Price = 2000000;
						NumberToWin = 5;
						NewRank = CPlayer::CR_DUKE;
						InRank = 5;
						break;
					case CPlayer::CR_EARL :
						Price = 800000;
						NumberToWin = 4;
						NewRank = CPlayer::CR_MARQUIS;
						InRank = 10;
						break;
					case CPlayer::CR_VISCOUNT :
						Price = 400000;
						NumberToWin = 3;
						NewRank = CPlayer::CR_EARL;
						InRank = 15;
						break;
					case CPlayer::CR_BARON :
						Price = 200000;
						NumberToWin = 2;
						NewRank = CPlayer::CR_VISCOUNT;
						InRank = 20;
						break;
					default :
					case CPlayer::CR_NONE :
						Price = 100000;
						NumberToWin = 1;
						NewRank = CPlayer::CR_BARON;
						InRank = 25;
						break;
				}

				if (Rank*100/MaxRank <= InRank)
				{
					bool
						Win = true;
					for (int i=0 ; i<NumberToWin ; i++)
					{
						if (Player->test_degree_of_amity() == false)
						{
							Win = false;
							break;
						}
					}
					if (Win == true)
					{
						CEmpireAction *
							Action = new CEmpireAction();
						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_GRANT_RANK );
						Action->set_amount( Price );
						Action->set_target( NewRank );
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->set_time();
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						CString
							News;
						News.format( GETTEXT("There is a letter from the Emperor offering you a title of %1$s."), Player->get_court_rank_name( NewRank ) );
						Player->time_news( (char*)News );

						continue;
					}
				}
			}

			CEmpireAction *
				Action = new CEmpireAction();
			long long int
				Amount;
			int
				DiplomacyCM = Player->get_control_model()->get_diplomacy();
			if (DiplomacyCM > 9) DiplomacyCM = 9;
			Amount = (long long int)Player->get_production()/20 + (long long int)Player->get_last_turn_production()*96/10;
			Amount = Amount*(100 + (long long int)Player->get_tribute_bonus_by_rank())/100;
			Amount = Amount*(10 - DiplomacyCM)/10;
			if (Amount > MAX_PLAYER_PP) Amount = MAX_PLAYER_PP;

			Action->set_id(Player->get_empire_action_list()->get_new_id());
			Action->set_owner(Player->get_game_id());
			Action->set_action(CEmpireAction::ED_DEMAND_TRIBUTE);
			Action->set_amount((int)Amount);
			Action->set_time();
			Action->set_answer(CEmpireAction::EA_ANSWER_WAITING);
			Action->save_new_action();
			Player->get_empire_action_list()->add_empire_action(Action);

			Player->time_news(GETTEXT("You got a letter of demand from the Emperor."));

			if( number(100) <= Player->get_empire_relation()/4 )
			{
				Player->grant_boon( CEmpireAction::ED_GRANT_BOON );
				Player->time_news( GETTEXT( "The Emperor is so pleased with your deed. He granted you a small boon." ) );
			}
			else
			{
				int
					DemandRoll = number(100);

				if (DemandRoll <= 5)		// leave council
				{
					if( Council->get_number_of_members() > 1 )
					{
						CEmpireAction *
							Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_LEAVE_COUNCIL );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					}
				}
				else if (DemandRoll <= 40)		// declare war
				{
					if (Council->get_speaker_id() == Player->get_game_id())
					{
						int
							Count = 0;
						CCouncil *
							Foe = NULL;
						for (int i=0 ; i<CouncilRelationList->length() ; i++ )
						{
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if (Relation->get_relation() != CRelation::RELATION_WAR &&
								Relation->get_relation() != CRelation::RELATION_TOTAL_WAR )
							{
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() < 0) Count -= Foe->get_empire_relation();
							}
						}
						Count = number(Count);
						for( int i = 0; i < CouncilRelationList->length(); i++ )
						{
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if( Relation->get_relation() != CRelation::RELATION_WAR &&
								Relation->get_relation() != CRelation::RELATION_TOTAL_WAR )
							{
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() < 0) Count += Foe->get_empire_relation();
								if( Count < 1 ) break;
							}
						}

						if( Foe == NULL ) continue;

						CEmpireAction *
							Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_DECLARE_WAR );
						Action->set_target(1);
						Action->set_amount( Foe->get_id() );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					}
					else
					{
						int
							Count = 0;
						CPlayer
							*Foe = NULL;
						for (int i=0 ; i<Members->length() ; i++)
						{
							CPlayer *
								Candidate = (CPlayer *)Members->get(i);
							if( Candidate->get_game_id() == Player->get_game_id() ) continue;
							if( Candidate->is_dead() ) continue;

							CPlayerRelation
								*Relation = Player->get_relation(Candidate);

							if( Relation == NULL ||
								(Relation->get_relation() != CRelation::RELATION_WAR &&
								Relation->get_relation() != CRelation::RELATION_TOTAL_WAR &&
								Candidate->get_empire_relation() < 0) ) Count -= Candidate->get_empire_relation();
						}
						Count = number(Count);
						for (int i=0 ; i<Members->length() ; i++)
						{
							CPlayer *
								Candidate = (CPlayer *)Members->get(i);
							if( Candidate->get_game_id() == Player->get_game_id() ) continue;
							if( Candidate->is_dead() ) continue;

							CPlayerRelation *
								Relation = Player->get_relation(Candidate);

							if( Relation == NULL ||
								(Relation->get_relation() != CRelation::RELATION_WAR &&
								Relation->get_relation() != CRelation::RELATION_TOTAL_WAR &&
								Candidate->get_empire_relation() < 0))
							{
								Count += Candidate->get_empire_relation();
								if( Count < 1 ){
									Foe = Candidate;
									break;
								}
							}
						}

						if( Foe == NULL ) continue;

						CEmpireAction *
							Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_DECLARE_WAR );
						Action->set_target(0);
						Action->set_amount( Foe->get_game_id() );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					}
				}
				else if (DemandRoll <= 60)		// make truce
				{
					if (Council->get_speaker_id() == Player->get_game_id())
					{
						int
							Count = 0;
						CCouncil *
							Foe = NULL;
						for( int i = 0; i < CouncilRelationList->length(); i++ ){
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if( Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_TOTAL_WAR ) 
							{
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() > 0) Count += Foe->get_empire_relation();
							}
						}
						Count = number(Count);
						for( int i = 0; i < CouncilRelationList->length(); i++ ){
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if( Relation->get_relation() == CRelation::RELATION_WAR || Relation->get_relation() == CRelation::RELATION_TOTAL_WAR ){
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() > 0) Count -= Foe->get_empire_relation();
								if( Count == 0 ) break;
							}
						}

						if( Foe == NULL ) continue;

						CEmpireAction
							*Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_MAKE_TRUCE );
						Action->set_target(1);
						Action->set_amount( Foe->get_id() );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					} else {
						int
							Count = 0;
						CPlayer
							*Foe = NULL;
						for( int i = 0; i < Members->length(); i++ ){
							CPlayer *
								Candidate = (CPlayer *)Members->get(i);
							if( Candidate->get_game_id() == Player->get_game_id() ) continue;
							if( Candidate->is_dead() ) continue;

							CPlayerRelation
								*Relation = Player->get_relation(Candidate);
							if( Relation == NULL ) continue;

							if( Relation->get_relation() == CRelation::RELATION_WAR || 
							   (Relation->get_relation() == CRelation::RELATION_TOTAL_WAR &&
							    Candidate->get_empire_relation() > 0)) Count += Candidate->get_empire_relation();
						}
						Count = number(Count);
						for( int i = 0; i < Members->length(); i++ ){
							CPlayer *
								Candidate = (CPlayer *)Members->get(i);
							if( Candidate->get_game_id() == Player->get_game_id() ) continue;
							if( Candidate->is_dead() ) continue;

							CPlayerRelation
								*Relation = Player->get_relation(Candidate);
							if( Relation == NULL ) continue;

							if( Relation->get_relation() == CRelation::RELATION_WAR || 
							   (Relation->get_relation() == CRelation::RELATION_TOTAL_WAR &&
							    Candidate->get_empire_relation() > 0)){
								Count -= Candidate->get_empire_relation();
								if( Count < 1 ){
									Foe = Candidate;
									break;
								}
							}
						}

						if( Foe == NULL ) continue;

						CEmpireAction
							*Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_MAKE_TRUCE );
						Action->set_target(0);
						Action->set_amount( Foe->get_game_id() );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					}
				}
				else if (DemandRoll <= 80)		// free subordinate council
				{
					if( Council->get_speaker_id() == Player->get_game_id() ){
						int
							Count = 0;
						CCouncil
							*Foe = NULL;
						for( int i = 0; i < CouncilRelationList->length(); i++ ){
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if( Relation->get_relation() == CRelation::RELATION_SUBORDINARY && 
								Relation->get_council1() == Council)
							{
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() > 0) Count += Foe->get_empire_relation();
							}
						}
						Count = number(Count);
						for( int i = 0; i < CouncilRelationList->length(); i++ ){
							CCouncilRelation *
								Relation = (CCouncilRelation *)CouncilRelationList->get(i);

							if( Relation->get_relation() == CRelation::RELATION_SUBORDINARY && Relation->get_council1() == Council ){
								Foe = (Relation->get_council1() == Council)?Relation->get_council2():Relation->get_council1();
								if (Foe->get_empire_relation() > 0) Count -= Foe->get_empire_relation();
								if( Count < 1 ) break;
							}
						}

						if( Foe == NULL ) continue;

						CEmpireAction
							*Action = new CEmpireAction();

						Action->set_id( Player->get_empire_action_list()->get_new_id() );
						Action->set_owner( Player->get_game_id() );
						Action->set_action( CEmpireAction::ED_DEMAND_FREE_SUBORDINARY_COUNCIL );
						Action->set_amount( Foe->get_id() );
						Action->set_time();
						Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
						Action->save_new_action();
						Player->get_empire_action_list()->add_empire_action(Action);

						Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
					}
				}
				else		// armament reduction
				{
					CEmpireAction *
						Action = new CEmpireAction();

					Action->set_id( Player->get_empire_action_list()->get_new_id() );
					Action->set_owner( Player->get_game_id() );
					Action->set_action( CEmpireAction::ED_DEMAND_ARMAMENT_REDUCTION );
					Action->set_amount( 5+number(10) );
					Action->set_time();
					Action->set_answer( CEmpireAction::EA_ANSWER_WAITING );
					Action->save_new_action();
					Player->get_empire_action_list()->add_empire_action(Action);

					Player->time_news( GETTEXT("You got a letter of demand from the Emperor.") );
				}
			}
		}
	}

	SLOG( "Empire Demand Crontab Running End" );
}


