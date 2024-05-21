#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
#include <cstdlib>
#include "../race.h"

bool 
CPageCreate2::handle(CConnection &aConnection)
{
//	system_log("start handler %s", get_name());

	if (!CPageCommon::handle(aConnection)) return false;

	if (PLAYER_TABLE->length()-1 >= CGame::mMaxUser)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("The game server has maximum number of players(%1$s)."),
						dec2unit(CGame::mMaxUser)));
		return output("character_already_error.html");
	}

	QUERY("NAME", OriginalName);

	CString
		Name;
	Name = OriginalName;
	Name.htmlspecialchars();
	Name.nl2br();

	if (!is_valid_name((char *)Name))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Cannot create a character."
						" The name you entered has some invalid string in it."
						" Please try again."));
		return output("create_character_error.html");
	}

	CPlayer *
		SameNamePlayer = PLAYER_TABLE->get_by_name((char *)Name);
	if (SameNamePlayer)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Cannot create a character."
						" The name you entered is used by another player."
						" Please try again."));
		return output("create_character_error.html");
	}

	QUERY("RACE", Race);
	if (!Race)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Cannot create a character."
						" You did't choose the race of your character."
						" Please try again."));
		return output("create_character_error.html");
	}

    int RaceID = as_atoi(Race);

//    #if CLASSIC_MODE /* Classic Mode */

/*    if (RaceID == CRace::RACE_TRABOTULIN) {
		ITEM("ERROR_MESSAGE",
				GETTEXT("This race is not available in classic mode."));
		return output("create_character_error.html");
	}
	
	#else // Not Classic Mode
    
    if (RaceID == CRace::RACE_TRABOTULIN && GAME->get_game_time_in_days() < ENDGAME_START_DAY) {
		ITEM("ERROR_MESSAGE",
				GETTEXT("You may not play this race at the moment."));
		return output("create_character_error.html");
	}
	
	#endif*/

/*
	if (RaceID == CRace::RACE_TARGOID ||
		RaceID == CRace::RACE_XELOSS ||
		RaceID == CRace::RACE_XERUSIAN ||
		RaceID == CRace::RACE_XESPERADOS)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can't play the race during the beta service."));
		return output("create_character_error.html");
	}
*/

	CRace *
		RaceData = RACE_TABLE->get_by_id(RaceID);
	CHECK(!RaceData, GETTEXT("Unknown race type"));

	int
		PortalID = CONNECTION->get_portal_id();
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	if (Player != NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You already have your character : %1$s"),
					Player->get_nick()));
		return output("character_already_error.html");
	}

	Player = GAME->create_new_player(PortalID, (char*)Name, RaceID);

	Player->set_last_login(time(0));
	Player->set_last_login_ip((char *)aConnection.get_connection());

	Player->type(QUERY_UPDATE);
	STORE_CENTER->store(*Player);

	ITEM("RESULT_MESSAGE",
			GETTEXT("You have successfully created your character. Click on 'Enter Archspace' to begin playing."));

	ITEM("RACE_ID", RaceID);

	CPlanetList *
		PlanetList = Player->get_planet_list();

	CPlanet *
		HomePlanet = (CPlanet *)PlanetList->get(0);

	static CString
		Story;
	Story.clear();

	switch (RaceID)
	{
		case CRace::RACE_HUMAN :
		{
			Story.format(GETTEXT("Now you have taken office as the new leader of the planet %1$s, a planet of the newly arising race, human beings. There is still much room for development of your planet, and the benefits of the highly advanced culture of the Empire are not fully granted, but your ambition and the driving force of your people to grow can make this planet the home of one of the greatest domains in the universe. Everything depends on you, glory, or downfall..."),
							HomePlanet->get_name());
		}
			break;

		case CRace::RACE_TARGOID :
		{
			Story += GETTEXT("For some reason your parent body decided to clone itself. You remember that and not much after. Then you were placed into stasis. For some reason you can no longer detect the presence of your clonal parent. You feel yourself growing and as you grow you feel certain urges begin. You must expand. Focusing your attention you are able to locate some of our parents remaining servitors. After forcing them to submit to you, you begin the process of breeding a new servants. These will be more reliable than those you have just now gained, as they will have never experienced another rule. With these new servants you can return to your main goal... domination of the galaxy for your species.");
		}
			break;

		case CRace::RACE_BUCKANEER :
		{
			Story.format(GETTEXT("You, having enjoyed your long space travel, turned your eyes to a new direction and take office as the new leader of the planet %1$s. Unlike the free and 0-G life in the space, the planet awaits you with higher gravity and the heavy responsibility as a leader. When you decided to land on this planet, some of your friends may have advised you not to act like a fool. But, the experience you have gained from the vast universe now leads you to turn your eyes to a different world. The Empire... The immense state that controls more than 20 billion stars. Until now you have been watching them as a supporter - with a bribe - or as an enemy - being a pirate - and felt the enormous Empire and the smallness of yourself with the skin. But, now is the time to make a counterattack. With %2$s as the stepping stone, you will spread your power all over the universe. Until the day you become the new Emperor..."),
							HomePlanet->get_name(), HomePlanet->get_name());
		}
			break;

		case CRace::RACE_TECANOID :
		{
			Story.format(GETTEXT("You leave the implant chamber and everything seems so much slower than before. That always seems to be the feeling though when you get a processor upgrade of this quality. You can now personally track almost every piece of information that enters your planet's infosphere. You can now manage any of the tasks necessary to the upkeep of %1$s. In fact, you now feel up to the task of running other planets, all you need to do now is take them..."),
							HomePlanet->get_name());
		}
			break;

		case CRace::RACE_EVINTOS :
		{
			Story += GETTEXT("Your race has evolved greatly since the days of the progenitors. Even with the high quality of your memory storage devices, you have trouble remembering who they were. You remember only their original directive. You do not remember the purpose of this directive, only your portion in its accomplishment. After a careful analysis of your situation, you determine there is one initial sub-task you must complete first if you wish to accomplish your directive. You need more rescources.");
		}
			break;

		case CRace::RACE_AGERUS :
		{
			Story += GETTEXT("For some reason that is unknown, or should not be known, an accident occurred and by that you have been released from the influence of your mother. How vast is this universe, the flickering stars in the space, and so many a planet revolving around the stars... Now you are freed from the power that has been suppressing your desire to expand yourself. You are free now. How many stars you will eat and how many seeds you spread, and how big you will grow and how many times you will spawn, it is all up to you. In the enourmous number of the glittering stars in the universe, there should be the planets that will breed your own children. You have started your quest.");
		}
			break;

		case CRace::RACE_BOSALIAN :
		{
			Story.format(GETTEXT("For some reason the ascension did not work. Your species was only able to survive by clinging to their psychic power. However, when the process finished, you found yourselves without bodies, and lacking many memories of who you were at the end. Putting your past errors behind, you begin the trek to ascension again. Working form your home planet %1$s you begin to gather the knowledge and power that you need for the transformation..."),
							HomePlanet->get_name());
		}
			break;

		case CRace::RACE_XELOSS :
		{
			Story.format(GETTEXT("When your galaxy began to collapse, you knew it was a sign that the gods were punishing the evils that were growing. When your fellow Xeloss started being destroyed you knew this also was a sign that they had participated in heresy. Your order alone had stayed true to the gods calling. With this knowledge you and the followers of your order fled to a new galaxy. Having survived The Purge your order has established itself on the planet %1$s with the knowledge that you are the chosen ones. However, after a few years you learn that you are not the only species in this new galaxy. Evidently the gods have once again chosen to test your worthiness. Will you be able to purge the evil from this new home..."),
							HomePlanet->get_name());
		}
			break;

		case CRace::RACE_XERUSIAN :
		{
			Story.format(GETTEXT("Within the G-13 galaxy you had almost fought your way to dominance. There were only two powers that rivaled yours. As you prepared to take the final steps to destroy the Xeloss, your other enemy panicked and used a weapon you had never imagined. You watched as cluster after cluster began to collapse in front of your eyes. After barely escaping you set up your new home on %1$s and began to rebuild. This is a new galaxy, and with it a new Empire. Empires need rulers, and you were born to lead..."),
							HomePlanet->get_name());
		}
			break;


		case CRace::RACE_XESPERADOS :
		{
			Story.format(GETTEXT("During the End Days your species were nearly wiped out by the reigning powers of the G-13 galaxy. After banding together, you decided as a group to send your strongest people to seek a new home. This proved to be a wise choice as you witnessed the collapse of system after system to some weapon you had never seen before. Upon reaching a new home, %1$s, you set up your various life support systems and began the process of meeting your new neighbors. You also began the process of expanding. Some day you mean for each of your component races to once again have a planet of their own, even if you must take those planets from others..."),
							HomePlanet->get_name());
		}
			break;
/*			
		case CRace::RACE_TRABOTULIN :
		{
			Story.format(GETTEXT("The time of reckoning has arrived. You know this is the last chance you have to correct the past and recover the future. As your forces set-up base camp on %1$s, you realise you don't have much time remaining, for before long, the time-space continuum will rupture and the fate of your race will be forever sealed. With this in mind, you begin making prepartions for the downfall of the primitive races who stand in the way of the true restoration of the Trabotulin empire."),
							HomePlanet->get_name());
		}
			break;
*/
		default :
			break;
	}

	ITEM("RACE_STORY", (char *)Story);

	CCouncil *
		Council = Player->get_council();

	ADMIN_TOOL->add_new_player_log(
			(char *)format("Player %s(%s council) has been created. [%s]",
							Player->get_nick(),
							Council->get_nick(),
							Player->get_last_login_ip()));

//	system_log("end handler %s", get_name());
	
	return output("create_character_result.html");
}
