#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageAnswerEvent::handler(CPlayer *aPlayer)
{
	QUERY("EVENT_ID", EventIDStr);
	QUERY("EVENT_TYPE", EventType);
	QUERY("EVENT_ANSWER", EventAnswer);

	if (!EventIDStr || !EventType || !EventAnswer)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("No event was selected or a type or an answer was missing."));
		return output("event_error.html");
	}
	if( strcasecmp(EventAnswer,"yes") && strcasecmp(EventAnswer,"no") )
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You did not answer this event."));
		return output("event_error.html");
	}

	int
		EventID = as_atoi(EventIDStr);

	CEventInstance *
		Event = NULL;
	if( strcasecmp( EventType, "Galactic" ) == 0 )
	{
		Event = GALACTIC_EVENT_LIST->get_by_id(EventID);
	}
	else if( strcasecmp( EventType, "Cluster" ) == 0 )
	{
		CCluster *
			Cluster = UNIVERSE->get_by_id(aPlayer->get_home_cluster_id());
		if( Cluster ) Event = Cluster->get_event_list()->get_by_id(EventID);
	}
	else
	{
		Event = aPlayer->get_event_list()->get_by_id(EventID);
	}

	CString
		Message;

	if (Event == NULL)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("There is no such an event."));
		return output("event_error.html");
	}

	if (Event->is_answered())
	{
		Message.format(GETTEXT("The event %1$s is already answered."),
						Event->get_name());
		ITEM("RESULT_MESSAGE", (char *)Message);
		return output("event_error.html");
	}

	Message.format( "<B>%s</B><BR>\nTYPE : %s<BR>\n%s<BR><BR>\n", Event->get_name(), Event->get_type_str(), Event->get_description() );

	if( strcasecmp(EventAnswer,"yes") == 0 )
	{

		if(Event != NULL)
		{
			if(aPlayer->can_buy_event(Event))
			{
				Message += "<BR>\n";
				Message.format(GETTEXT("You accepted the offer."));
			}
			else
			{
				ITEM("RESULT_MESSAGE",
					GETTEXT("You do not have enough PP to answer the event."));

				return output("event_error.html");
			}			
		}
	}
	else
	{
		Message += "<BR>\n";
		Message.format(GETTEXT("You rejected the offer."));
	}

	Message += aPlayer->activate_event( Event, EventAnswer );
	Event->set_answered();

	ITEM("IMAGE", Event->get_image());
	ITEM("RESULT_MESSAGE", (char*)Message);

	return output("answer_event.html");
}
