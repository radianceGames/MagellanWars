#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include "../player.h"
#include "../event.h"
#include "../effect.h"
bool
CPageShowEvent::handler(CPlayer *aPlayer)
{
	QUERY("EVENT_ID", EventIDStr);
	QUERY("EVENT_TYPE", EventType);

	if (!EventIDStr || !EventType)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("No event was selected or a type was missing."));
		return output("event_error.html");
	}

	int
		EventID = as_atoi(EventIDStr);

	CEventInstance
		*Event = NULL;
	if( strcasecmp( EventType, "Galactic" ) == 0 ){
		Event = GALACTIC_EVENT_LIST->get_by_id(EventID);
	} else if( strcasecmp( EventType, "Cluster" ) == 0 ){
		CCluster
			*Cluster = UNIVERSE->get_by_id(aPlayer->get_home_cluster_id());
		if( Cluster ) Event = Cluster->get_event_list()->get_by_id(EventID);
	} else {
		Event = aPlayer->get_event_list()->get_by_id(EventID);
	}
//	CEventEffectList *EventEffectList = Event->get_event()->get_effect_list(); // no longer used
	CString
		Message;

	if (Event == NULL)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("There is no such event."));
		return output("event_error.html");
	}

	Message.format( "<B>%s</B><BR>\nTYPE : %s<BR>\n%s<BR><BR>\n", Event->get_name(), Event->get_type_str(), Event->get_description() );
/*	if (EventEffectList->length() > 0)
	    Message.format("<BR><B>Effects:</B><BR>");
	for (int i=0; i < EventEffectList->length(); i++)
	{
		 CEventEffect *EventEffect = (CEventEffect *)EventEffectList->get(i);
         if (EventEffect->get_type() != CPlayerEffect::PA_CHANGE_CONTROL_MODEL)
                  Message.format("ID: %d TYPE: %s ARG1: %d ARG2: %d TARGET: %d LIFE: %d DURATION: %d<BR>", 
                                    EventEffect->get_id(), EventEffect->get_type_name(EventEffect->get_type()), 
                                    EventEffect->get_argument1(), EventEffect->get_argument2(), EventEffect->get_target(),
                                    EventEffect->get_life(), EventEffect->get_duration());
         else
                  Message.format("ID: %d TYPE: %s CM: %s EFFECT: %d LIFE: %d DURATION: %d<BR>", 
                                    EventEffect->get_id(), EventEffect->get_type_name(EventEffect->get_type()), 
                                    CControlModel::get_cm_description(EventEffect->get_target()), EventEffect->get_argument1(), 
                                    EventEffect->get_life(), EventEffect->get_duration());

	}*/
	if (Event->has_type(CEvent::EVENT_ANSWER))
	{
		if(Event->is_answered() == false)
		{
			Message.format("<A HREF=\"answer_event.as?EVENT_ID=%s&EVENT_TYPE=%s&EVENT_ANSWER=yes\"><IMG SRC=%s/image/as_game/bu_accept.gif BORDER = 0></A>\n<A HREF=\"answer_event.as?EVENT_ID=%s&EVENT_TYPE=%s&EVENT_ANSWER=no\"><IMG SRC=%s/image/as_game/bu_decline.gif BORDER=0></A>\n",
							EventIDStr,
							EventType,
							(char *)CGame::mImageServerURL,
							EventIDStr,
							EventType,
							(char *)CGame::mImageServerURL);
		}
	}

	ITEM("IMAGE", Event->get_image());
	ITEM("RESULT_MESSAGE", (char*)Message);

	return output("show_event.html");
}
