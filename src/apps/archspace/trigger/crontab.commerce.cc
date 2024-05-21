#include <libintl.h>
#include "../triggers.h"
#include "../archspace.h"
#include "../council.h"
#include "../player.h"
#include "../relation.h"
#include "../define.h"

void
CCronTabCommerce::handler()
{
	SLOG( "Commerce Crontab Running Start" );

	if (EMPIRE->is_dead() == false)
	{
	    // Player Loop
	    SLOG("Start Player Loop");
	    for (int i=EMPIRE_GAME_ID+1; i < PLAYER_TABLE->length(); i++)
	    {
	        CPlayer *CurrentPlayer = (CPlayer *)PLAYER_TABLE->get(i);
	        if (CurrentPlayer == NULL || CurrentPlayer->is_dead())
	                 continue;
	        CPlayerList *CouncilMembers = CurrentPlayer->get_council()->get_members();

	        // Note: Could use relation list as well (more efficient) but less secure?
	        for (int j=0; j < CouncilMembers->length(); j++)
	        {
	           CPlayer *aPlayer = (CPlayer *)CouncilMembers->get(j);
	           if (aPlayer == NULL || aPlayer->get_game_id() == CurrentPlayer->get_game_id())
	              continue;
	           CPlayerRelation *aRelation = CurrentPlayer->get_relation(aPlayer);
	           if (aRelation != NULL && 
                        (aRelation->get_relation() == CRelation::RELATION_PEACE 
                        || aRelation->get_relation() == CRelation::RELATION_ALLY))
                  CurrentPlayer->set_commerce_with(aPlayer);      
            	}	        
            }
	    SLOG("End Player Loop");        
	    // Council Loop
	    SLOG("Start Council Loop");        
	    for (int i=0; i < COUNCIL_TABLE->length(); i++)
	    {
	        CCouncil *CurrentCouncil = (CCouncil *)COUNCIL_TABLE->get(i);
	        if (CurrentCouncil == NULL)
                continue;   
	        CCouncilRelationList *RelationList = CurrentCouncil->get_relation_list();
	        if (RelationList == NULL)
                continue;
	        // Note: Could use relation list as well (more efficient) but less secure?
	        for (int j=0; j < RelationList->length(); j++)
	        {
	           CCouncilRelation *aRelation = (CCouncilRelation *)RelationList->get(j);
	           CCouncil *aCouncil = NULL;
	           if (aRelation == NULL)
	               continue;
	           if (aRelation->get_council1() == CurrentCouncil)
	              aCouncil = aRelation->get_council2();
	           else
                  aCouncil = aRelation->get_council1();   
	           
	           if (aRelation->get_relation() == CRelation::RELATION_PEACE 
                        || aRelation->get_relation() == CRelation::RELATION_ALLY)
               {
//                  CurrentCouncil->set_commerce_with(aCouncil);      // is a protected function
                    CPlayerList *aMemberList = aCouncil->get_members();
                    CPlayerList *CurrentMemberList = CurrentCouncil->get_members();
                    for (int j=0; j < CurrentMemberList->length(); j++)
                    {
                     CPlayer *Member1 = (CPlayer *)CurrentMemberList->get(j);
                     for (int k=0; k < aMemberList->length(); k++)
                     {
                      CPlayer *Member2 = (CPlayer *)aMemberList->get(k);
                      Member1->set_commerce_with(Member2);   
                     }                        
                    }
               }    
            }	        
 	    }    
	    SLOG("End Council Loop");         	    
	}

	SLOG( "Commerce Crontab Running End" );
}

