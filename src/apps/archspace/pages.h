#if !defined(__ARCHSPACE_PAGES_H__)
#define __ARCHSPACE_PAGES_H__

#include "admin.h"

/**
*/
class CPageNotFound: public CPage
{
	public:
		CPageNotFound():CPage() {}
		virtual ~CPageNotFound() {}

		virtual const char *get_name();
		virtual bool handle(CConnection &aConnection);
};


//--------------------------------------------------    BASE

#define CONNECTION ((CArchspaceConnection*)mConnection)

#define QUERY_STRING \
			((CArchspaceConnection*)mConnection)->get_query()

#define ITEM(aName, aData) \
			mConversion.set_value(aName, aData);

#define REFRESH(aName, aData) \
			mConversion.replace_value(aName, aData);

#define QUERY(aName, aValue) \
			const char  \
				*aValue = mQuery.get_value(aName);

#define CHECK(aExpress, aMessage) \
			if (aExpress) \
			{ \
				message_page(aMessage); \
				return true; \
			}

/**
*/
class CPageCommon: public CPage
{
	public:
		CPageCommon():CPage() { mConnection = NULL; }
		virtual ~CPageCommon() {}

		virtual const char *get_name() { return "common"; }
//		virtual const char *get_referrer() { return NULL; }
		virtual const char *allow_method() { return "GET"; }

		virtual bool handle(CConnection &aConnection);
	protected:
		inline bool output(const char *aWebFile);
		virtual bool check_referrer();
		virtual void message_page(const char *aMessage);
		virtual bool get_conversion();

		CConnection
			*mConnection;
		CQueryList
			mConversion;
		CQueryList
			mQuery;
};


inline bool
CPageCommon::output(const char *aWebFile)
{
	if (!mHTMLStation->get_html(*this, aWebFile, &mConversion))
		return false;
	mConnection = NULL;
	return true;
}

/**
*/
class CBotPage: public CPage
{
	public:
		CBotPage():CPage() { mConnection = NULL; }
		virtual ~CBotPage() {}

		virtual const char *get_name() { return "bot"; }
		virtual const char *allow_method() { return "GET"; }

		virtual bool handle(CConnection &aConnection);
	protected:
		inline bool output(const char *aWebFile);
        inline bool output_text(const char *aString);
		virtual void message_page(const char *aMessage);
		virtual bool get_conversion();
		CConnection
			*mConnection;
		CQueryList
			mConversion;
		CQueryList
			mQuery;
};

inline bool
CBotPage::output(const char *aWebFile)
{
	if (!mHTMLStation->get_html(*this, aWebFile, &mConversion))
		return false;
	mConnection = NULL;
	return true;
}

inline bool
CBotPage::output_text(const char *aString)
{
    mOutput = aString;
	mConnection = NULL;
	return true;
}

/**
*/
class CPageGame: public CPageCommon
{
	public:
		CPageGame():CPageCommon() {}
		virtual ~CPageGame() {}

		virtual const char *get_name() { return "game"; }

		virtual bool handle(CConnection &aConnection);
	protected:
		int get_game_id();
		CPlayer* get_player();

		virtual bool handler(CPlayer *aPlayer) = 0;
};

//--------------------------------------------------    SYSTEM
/**
*/
class CPageCreate: public CPageCommon
{
	public:
		CPageCreate():CPageCommon() {}
		virtual ~CPageCreate() {}
		virtual const char *get_name() { return "/archspace/create.as"; }

		virtual bool handle(CConnection &aConnection);
	protected:
		const char* race_html();
};

class CPageCreate2: public CPageCommon
{
	public:
		CPageCreate2():CPageCommon() {}
		virtual ~CPageCreate2() {}
		virtual const char *get_name()
			{ return "/archspace/create2.as"; }
		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/create.as"; }
		virtual bool handle(CConnection &aConnection);
};

/**
*/
class CPageLogin: public CPageCommon
{
	public:
		CPageLogin():CPageCommon() {}
		virtual ~CPageLogin() {}
		virtual const char *get_name() { return "/archspace/login.as"; }
		virtual const char *allow_method() { return "GET"; }
		virtual bool handle(CConnection &aConnection);
};

/**
*/
class CPageLogout: public CPageCommon
{
	public:
		CPageLogout():CPageCommon() {}
		virtual ~CPageLogout() {}
		virtual const char *get_name() { return "/archspace/logout.as"; }
		virtual const char *allow_method() { return "GET"; }

		virtual bool handle();
};

/**
*/
class CPageDeathMenu: public CPageCommon
{
	public:
		CPageDeathMenu():CPageCommon() {}
		virtual ~CPageDeathMenu() {}
		virtual const char *get_name()
				{ return "/archspace/death_menu.as"; }
		virtual const char *allow_method() { return "GET"; }

		virtual bool handle(CConnection &aConnection);
};

/**
*/
class CPageDeathMain: public CPageCommon
{
	public:
		CPageDeathMain():CPageCommon() {}
		virtual ~CPageDeathMain() {}
		virtual const char *get_name()
				{ return "/archspace/death_main.as"; }
		virtual const char *allow_method() { return "GET"; }

		virtual bool handle(CConnection &aConnection);
};

//------------------------------------------------     BOT
/**
*/
class CBotPagePlayerPower: public CBotPage
{
   public:
    CBotPagePlayerPower():CBotPage() {}
    virtual ~CBotPagePlayerPower() {}
	virtual const char *get_name() { return "/archspace/a0s/player_power.as"; }
	virtual const char *allow_method() { return "GET"; }
	virtual bool handle(CConnection &aConnection);
};

class CBotPageEndScore: public CBotPage
{
  public:
   CBotPageEndScore():CBotPage() {}
   virtual ~CBotPageEndScore() {}
   virtual const char *get_name() { return "/archspace/a0s/end_score.as"; }
   virtual bool handle(CConnection &aConnection);
};

class CBotPageDeletePlayer: public CBotPage
{
   public:
    CBotPageDeletePlayer():CBotPage() {}
    virtual ~CBotPageDeletePlayer() {}
	virtual const char *get_name() { return "/archspace/a0s/delete_player.as"; }
	virtual const char *allow_method() { return "GET"; }
	virtual bool handle(CConnection &aConnection);
};

//--------------------------------------------------    GAME
/**
*/
class CPageMenu: public CPageGame
{
	public:
		CPageMenu():CPageGame() {}
		virtual ~CPageMenu() {}

		virtual const char *get_name() { return "/archspace/menu.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMain: public CPageGame
{
	public:
		CPageMain():CPageGame() {}
		virtual ~CPageMain() {}

		virtual const char *get_name() { return "/archspace/main.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageHeadTitle: public CPageGame
{
	public:
		virtual ~CPageHeadTitle() {}

		virtual const char *get_name() { return "head_title"; }

		void refresh_product_point_item();
		void refresh_planet_count_item();
		void refresh_power_item();
	protected:
		virtual bool get_conversion();
};

/**
*/
class CPagePreference: public CPageHeadTitle
{
	public:
		CPagePreference():CPageHeadTitle() {}
		virtual ~CPagePreference() {}

		virtual const char *allow_method() { return "GET POST"; }
		virtual const char *get_name() { return "/archspace/preference.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};


class CPageShowEvent: public CPageHeadTitle
{
	public:
		CPageShowEvent() {}
		virtual ~CPageShowEvent() {}

		virtual const char *get_name() { return "/archspace/show_event.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageAnswerEvent: public CPageHeadTitle
{
	public:
		CPageAnswerEvent() {}
		virtual ~CPageAnswerEvent() {}

		virtual const char *get_name() { return "/archspace/answer_event.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//------------------------------------------------  Domestic
/**
*/
class CPageDomesticLink: public CPageHeadTitle
{
	public:
		virtual ~CPageDomesticLink() {}

		virtual const char *get_name() { return "domestic_link"; }
	protected:
		virtual bool get_conversion();
};

/**
*/
class CPageDomestic: public CPageDomesticLink
{
	public:
		CPageDomestic():CPageDomesticLink() {}
		virtual ~CPageDomestic() {}

		virtual const char *get_name() { return "/archspace/domestic/domestic.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageConcentrationMode: public CPageDomesticLink
{
	public:
		virtual ~CPageConcentrationMode() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/concentration_mode.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageConcentrationModeResult: public CPageDomesticLink
{
	public:
		virtual ~CPageConcentrationModeResult() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/concentration_mode.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/concentration_mode_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

// ----- RESEARCH
/**
*/
class CPageResearch: public CPageDomesticLink
{
	public:
		virtual ~CPageResearch() {}

		virtual const char *get_name() { return "/archspace/domestic/research.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageResearchInvestResult: public CPageDomesticLink
{
	public:
		virtual ~CPageResearchInvestResult() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/research_invest_result.as"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/research.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageResearchTechDetail: public CPageDomesticLink
{
	public:
		virtual ~CPageResearchTechDetail() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/research_tech_detail.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageResearchTechAchievement: public CPageDomesticLink
{
	public:
		virtual ~CPageResearchTechAchievement() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/research_tech_achievement.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageResearchResult: public CPageDomesticLink
{
	public:
		virtual ~CPageResearchResult() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/research_tech_detail.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/research_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageTechTree: public CPageDomesticLink
{
	public:
		virtual ~CPageTechTree() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/tech_tree.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//----  PROJECT
/**
*/
class CPageProject: public CPageDomesticLink
{
	public:
		virtual ~CPageProject() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/project.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageProjectConfirm: public CPageDomesticLink
{
	public:
		virtual ~CPageProjectConfirm() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/project.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/project_confirm.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageProjectResult: public CPageDomesticLink
{
	public:
		virtual ~CPageProjectResult() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/project_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/project_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};


//---- PLANET MANAGEMENT
/**
*/
class CPagePlanetManagement: public CPageDomesticLink
{
	public:
		virtual ~CPagePlanetManagement() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_management.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlanetDetail: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetDetail() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_detail.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlanetInvestPool: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetInvestPool() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_invest_pool.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlanetInvestPoolResult: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetInvestPoolResult() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/planet_invest_pool.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/planet_invest_pool_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlanetInvestPoolEnable: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetInvestPoolEnable() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/planet_invest_pool_enable.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlanetInvestPoolEnableResult: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetInvestPoolEnableResult() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/planet_invest_pool_enable_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlanetManagementBuildingResult: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetManagementBuildingResult() {}
		virtual const char *allow_method() { return "POST"; }

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_management_building_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlanetManagementInvestResult: public CPagePlanetManagement
{
	public:
		virtual ~CPagePlanetManagementInvestResult() {}
		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/domestic/planet_detail.as"; }
		virtual const char *get_name()
			{ return "/archspace/domestic/planet_management_invest_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlanetDestroyConfirm: public CPageDomesticLink
{
	public:
		virtual ~CPagePlanetDestroyConfirm() {}
		virtual const char *allow_method() { return "POST"; }

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_destroy_confirm.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlanetDestroyCancel: public CPageDomesticLink
{
	public:
		virtual ~CPagePlanetDestroyCancel() {}
		virtual const char *allow_method() { return "POST"; }

		virtual const char *get_name()
			{ return "/archspace/domestic/planet_destroy_cancel.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebook: public CPageDomesticLink
{
	public:
		CPageWhitebook():CPageDomesticLink() {}
		virtual ~CPageWhitebook() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebookControlModel: public CPageDomesticLink
{
	public:
		CPageWhitebookControlModel():CPageDomesticLink() {}
		virtual ~CPageWhitebookControlModel() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook_control_model.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebookControlModelDetail: public CPageDomesticLink
{
	public:
		CPageWhitebookControlModelDetail():CPageDomesticLink() {}
		virtual ~CPageWhitebookControlModelDetail() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook_control_model_detail.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebookControlModelTech: public CPageDomesticLink
{
	public:
		CPageWhitebookControlModelTech():CPageDomesticLink() {}
		virtual ~CPageWhitebookControlModelTech() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook_control_model_tech.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebookControlModelProject: public CPageDomesticLink
{
	public:
		CPageWhitebookControlModelProject():CPageDomesticLink() {}
		virtual ~CPageWhitebookControlModelProject() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook_control_model_project.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageWhitebookControlModelPlanet: public CPageDomesticLink
{
	public:
		CPageWhitebookControlModelPlanet():CPageDomesticLink() {}
		virtual ~CPageWhitebookControlModelPlanet() {}

		virtual const char *get_name()
			{ return "/archspace/domestic/whitebook_control_model_planet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//---------------------------------------------------------------- DIPLOMACY
/**
*/
class CPageDiplomacyLink: public CPageHeadTitle
{
	public:
		virtual ~CPageDiplomacyLink() {}

		virtual const char *get_name() { return "diplomacy_link"; }
	protected:
		virtual bool get_conversion();
};

/**
*/
class CPageDiplomacy: public CPageDiplomacyLink
{
	public:
		CPageDiplomacy():CPageDiplomacyLink() {}
		virtual ~CPageDiplomacy() {}

		virtual const char *get_name() { return "/archspace/diplomacy/diplomacy.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
		virtual void message_page(const char *aMessage);
};

/**
*/
class CPagePlayerDiplomacyStatusList: public CPageDiplomacyLink
{
	public:
		CPagePlayerDiplomacyStatusList():CPageDiplomacyLink() {}
		virtual ~CPagePlayerDiplomacyStatusList() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/diplomacy_status_list.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerDiplomacyManagement: public CPageDiplomacyLink
{
	public:
		CPagePlayerDiplomacyManagement():CPageDiplomacyLink() {}
		virtual ~CPagePlayerDiplomacyManagement() {}
		virtual const char *get_name()
			{ return "/archspace/diplomacy/diplomacy_management.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReadDiplomaticMessage: public CPageDiplomacyLink
{
	public:
		virtual ~CPageReadDiplomaticMessage() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/read_diplomatic_message.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReadDiplomaticMessageUnanswered: public CPageDiplomacyLink
{
	public:
		CPageReadDiplomaticMessageUnanswered():CPageDiplomacyLink() {}
		virtual ~CPageReadDiplomaticMessageUnanswered() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/read_diplomatic_message_unanswered.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReadDiplomaticMessageDelete: public CPageDiplomacyLink
{
	public:
		CPageReadDiplomaticMessageDelete():CPageDiplomacyLink() {}
		virtual ~CPageReadDiplomaticMessageDelete() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()


			{ return "/archspace/diplomacy/read_diplomatic_message_delete.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerDiplomaticMessageAccuse: public CPageDiplomacyLink
{
	public:
		CPagePlayerDiplomaticMessageAccuse():CPageDiplomacyLink() {}
		virtual ~CPagePlayerDiplomaticMessageAccuse() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/diplomatic_message_accuse.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerReadDiplomaticMessageResult: public CPageDiplomacyLink
{
	public:
		CPagePlayerReadDiplomaticMessageResult():CPageDiplomacyLink() {}
		virtual ~CPagePlayerReadDiplomaticMessageResult() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/read_diplomatic_message_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerSendDiplomaticMessage: public CPageDiplomacyLink
{
	public:
		virtual ~CPagePlayerSendDiplomaticMessage() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/send_diplomatic_message.as"; }
	protected:
		virtual void message_page(const char* aMessage);
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerSendDiplomaticMessageWrite:
		public CPagePlayerSendDiplomaticMessage
{
	public:
		virtual ~CPagePlayerSendDiplomaticMessageWrite() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/send_diplomatic_message_write.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerSendDiplomaticMessageResult:
		public CPagePlayerSendDiplomaticMessage
{
	public:
		virtual ~CPagePlayerSendDiplomaticMessageResult() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/send_diplomatic_message_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageInspectOtherPlayer: public CPageDiplomacyLink
{
	public:
		virtual ~CPageInspectOtherPlayer() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/inspect_other_player.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
		virtual void message_page(const char* aMessage);
};

/**
*/
class CPageInspectOtherPlayerResult: public CPageInspectOtherPlayer
{
	public:
		virtual ~CPageInspectOtherPlayerResult() {}

//		virtual const char *get_referrer() { return "/archspace/diplomacy/inspect_other_player.as"; }
		virtual const char *get_name()
			{ return "/archspace/diplomacy/inspect_other_player_result.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageSpy: public CPageDiplomacyLink
{
	public:
		CPageSpy():CPageDiplomacyLink() {}
		virtual ~CPageSpy() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/spy.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageSpyChangeSecurityLevel: public CPageDiplomacyLink
{
	public:
		CPageSpyChangeSecurityLevel():CPageDiplomacyLink() {}
		virtual ~CPageSpyChangeSecurityLevel() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/spy_change_security_level.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageSpyChangeSecurityLevelResult: public CPageDiplomacyLink
{
	public:
		CPageSpyChangeSecurityLevelResult():CPageDiplomacyLink() {}
		virtual ~CPageSpyChangeSecurityLevelResult() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/spy_change_security_level_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageSpyConfirm: public CPageDiplomacyLink
{
	public:
		CPageSpyConfirm():CPageDiplomacyLink() {}
		virtual ~CPageSpyConfirm() {}

		virtual const char *get_name()
			{ return "/archspace/diplomacy/spy_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageSpyResult: public CPageDiplomacyLink
{
	public:
		CPageSpyResult():CPageDiplomacyLink() {}
		virtual ~CPageSpyResult() {}

//		virtual const char *get_referrer() { return "/archspace/diplomacy/spy_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/diplomacy/spy_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//---------------------------------------------------------------- COUNCIL
/**
*/
class CPageCouncilLink: public CPageHeadTitle
{
	public:
		virtual ~CPageCouncilLink() {}

		virtual const char *get_name() { return "council_link"; }
	protected:
		virtual bool get_conversion();
};

/**
*/
class CPageCouncil: public CPageCouncilLink
{
	public:
		virtual ~CPageCouncil() {}

		virtual const char *get_name() { return "/archspace/council/council.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilVote: public CPageCouncilLink
{
	public:
		virtual ~CPageCouncilVote() {}

		virtual const char *get_name() { return "/archspace/council/council_vote.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilVoteResult: public CPageCouncilLink
{
	public:
		virtual ~CPageCouncilVoteResult() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name() { return "/archspace/council/council_vote_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDonation: public CPageCouncilLink
{
	public:
		virtual ~CPageDonation() {}

		virtual const char *get_name() { return "/archspace/council/donation.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDonationResult: public CPageCouncilLink
{
	public:
		virtual ~CPageDonationResult() {}

//		virtual const char *get_referrer() { return "/archspace/council/donation.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name() { return "/archspace/council/donation_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

#define ARE_YOU_SPEAKER(x) \
	if (!are_you_speaker(x)) return false;

#define ARE_YOU_LEAD_SPEAKER(x) \
	if (!are_you_lead_speaker(x)) return false;

/**
*/
class CPageSpeakerMenu: public CPageCouncilLink
{
	public:
		virtual ~CPageSpeakerMenu() {}

		virtual const char *get_name()
			{ return "/archspace/council/speaker_menu.as"; }
	protected:
		virtual bool are_you_speaker(CPlayer *aPlayer);
		virtual bool are_you_lead_speaker(CPlayer *aPlayer);
		virtual void message_page(const char *aMessage);
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageTeamSpirit: public CPageCouncilLink
{
	public:
		virtual ~CPageTeamSpirit() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/team_spirit.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
class CPageSetSecondarySpeaker: public CPageSpeakerMenu
{
	public:
		virtual ~CPageSetSecondarySpeaker() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name() { return "/archspace/council/set_secondary_speaker.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
/**
*/
class CPageDistribute: public CPageSpeakerMenu
{
	public:
		virtual ~CPageDistribute() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name() { return "/archspace/council/distribute.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilProject: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilProject() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name() { return "/archspace/council/project.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFinancialAid: public CPageSpeakerMenu
{
	public:
		virtual ~CPageFinancialAid() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/financial_aid.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageChangeCouncilName: public CPageSpeakerMenu
{
	public:
		virtual ~CPageChangeCouncilName() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/change_council_name.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageChangeCouncilSlogan: public CPageSpeakerMenu
{
	public:
		virtual ~CPageChangeCouncilSlogan() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/change_council_slogan.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageIndependenceDeclaration: public CPageCouncilLink
{
	public:
		virtual ~CPageIndependenceDeclaration() {}

		virtual const char *get_name()
			{ return "/archspace/council/independence_declaration.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageIndependenceDeclarationResult: public CPageCouncilLink
{
	public:
		virtual ~CPageIndependenceDeclarationResult() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/independence_declaration_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageAdmissionRequest: public CPageCouncilLink
{
	public:
		CPageAdmissionRequest():CPageCouncilLink() {}
		virtual ~CPageAdmissionRequest() {}

		virtual const char *get_name()
			{ return "/archspace/council/admission_request.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageAdmissionRequestResult: public CPageCouncilLink
{
	public:
		CPageAdmissionRequestResult():CPageCouncilLink() {}
		virtual ~CPageAdmissionRequestResult() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/council/admission_request.as"; }
		virtual const char *get_name()
			{ return "/archspace/council/admission_request_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDeleteAdmissionRequestResult: public CPageCouncilLink
{
	public:
		CPageDeleteAdmissionRequestResult():CPageCouncilLink() {}
		virtual ~CPageDeleteAdmissionRequestResult() {}

//		virtual const char *get_referrer() { return "/archspace/council/admission_request.as"; }
		virtual const char *get_name()
			{ return "/archspace/council/delete_admission_request_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerAdmission: public CPageSpeakerMenu
{
	public:
		virtual ~CPagePlayerAdmission() {}

		virtual const char *get_name()
			{ return "/archspace/council/player_admission.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerAdmissionRead: public CPageSpeakerMenu
{
	public:
		virtual ~CPagePlayerAdmissionRead() {}

		virtual const char *get_name()
			{ return "/archspace/council/player_admission_read.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerAdmissionReply: public CPageSpeakerMenu
{
	public:
		virtual ~CPagePlayerAdmissionReply() {}

		virtual const char *allow_method() { return "POST"; }
//		virtual const char *get_referrer() { return "/archspace/council/player_admission_read.as"; }
		virtual const char *get_name()
			{ return "/archspace/council/player_admission_reply.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPagePlayerAdmissionAccuse: public CPageSpeakerMenu
{
	public:
		virtual ~CPagePlayerAdmissionAccuse() {}

		virtual const char *get_name()
			{ return "/archspace/council/player_admission_accuse.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageNewplayerAssignmentOption: public CPageSpeakerMenu
{
	public:
		virtual ~CPageNewplayerAssignmentOption() {}

		virtual const char *allow_method() { return "POST"; }
		virtual const char *get_name()
			{ return "/archspace/council/newplayer_assignment_option.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilDiplomacy: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilDiplomacy() {}

		virtual const char *get_name()
			{ return "/archspace/council/council_diplomacy.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDiplomaticMessageRead: public CPageSpeakerMenu
{
    public:
        virtual ~CPageDiplomaticMessageRead() {}

        virtual const char *get_name()
            { return "/archspace/council/diplomatic_message_read.as"; }
    protected:
        virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDiplomaticMessageReply: public CPageSpeakerMenu
{
	public:
		virtual ~CPageDiplomaticMessageReply() {}

		virtual const char *get_name()
			{ return "/archspace/council/diplomatic_message_reply.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilDiplomaticMessageAccuse: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilDiplomaticMessageAccuse() {}

		virtual const char *get_name()
			{ return "/archspace/council/diplomatic_message_accuse.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilDiplomacyStatusList: public CPageCouncilLink
{
	public:
	CPageCouncilDiplomacyStatusList():CPageCouncilLink() {}

		virtual ~CPageCouncilDiplomacyStatusList() {}

		virtual const char *get_name()
			{ return "/archspace/council/diplomacy_status_list.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageInspectOtherCouncil: public CPageSpeakerMenu
{
	public:
		virtual ~CPageInspectOtherCouncil() {}

		virtual const char *get_name()
			{ return "/archspace/council/inspect_other_council.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageInspectOtherCouncilResult: public CPageSpeakerMenu
{
	public:
		virtual ~CPageInspectOtherCouncilResult() {}

		virtual const char *get_name()
			{ return "/archspace/council/inspect_other_council_result.as"; }
		virtual const char *allow_method() { return "GET POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDiplomaticMessage: public CPageSpeakerMenu
{
    public:
        virtual ~CPageDiplomaticMessage() {}

        virtual const char *get_name()
            { return "/archspace/council/diplomatic_message.as"; }
    protected:
        virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilSendDiplomaticMessage: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilSendDiplomaticMessage() {}

		virtual const char *get_name()
			{ return "/archspace/council/send_diplomatic_message.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilSendDiplomaticMessageWrite: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilSendDiplomaticMessageWrite() {}

		virtual const char *get_name()
			{ return "/archspace/council/send_diplomatic_message_write.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageCouncilSendDiplomaticMessageResult: public CPageSpeakerMenu
{
	public:
		virtual ~CPageCouncilSendDiplomaticMessageResult() {}

		virtual const char *get_name()
			{ return "/archspace/council/send_diplomatic_message_result.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageInfoLink: public CPageHeadTitle
{
	public:
		virtual ~CPageInfoLink() {}

		virtual const char *get_name() { return "info_link"; }
	protected:
		virtual bool get_conversion();
};

class CPageInfo: public CPageInfoLink
{
	public:
		CPageInfo():CPageInfoLink() {}
		virtual ~CPageInfo() {}

		virtual const char *get_name() { return "/archspace/info/info.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerSearch: public CPageInfoLink
{
	public:
		CPagePlayerSearch():CPageInfoLink() {}
		virtual ~CPagePlayerSearch() {}

		virtual const char *get_name()
			{ return "/archspace/info/player_search.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerSearchResult: public CPageInfoLink
{
	public:
		CPagePlayerSearchResult():CPageInfoLink() {}
		virtual ~CPagePlayerSearchResult() {}

		virtual const char *get_name()
			{ return "/archspace/info/player_search_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilSearch: public CPageInfoLink
{
	public:
		CPageCouncilSearch():CPageInfoLink() {}
		virtual ~CPageCouncilSearch() {}

		virtual const char *get_name() { return "/archspace/info/council_search.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilSearchResult: public CPageInfoLink
{
	public:
		CPageCouncilSearchResult():CPageInfoLink() {}
		virtual ~CPageCouncilSearchResult() {}

		virtual const char *get_name()
			{ return "/archspace/info/council_search_result.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageClusters: public CPageInfoLink
{
	public:
		CPageClusters():CPageInfoLink() {}
		virtual ~CPageClusters() {}

		virtual const char *get_name() { return "/archspace/info/clusters.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageClustersCouncil: public CPageInfoLink
{
	public:
		CPageClustersCouncil():CPageInfoLink() {}
		virtual ~CPageClustersCouncil() {}

		virtual const char *get_name() { return "/archspace/info/clusters_council.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankPlayerOverall: public CPageInfoLink
{
	public:
		CPageRankPlayerOverall():CPageInfoLink() {}
		virtual ~CPageRankPlayerOverall() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_player_overall.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankPlayerFleet: public CPageInfoLink
{
	public:
		CPageRankPlayerFleet():CPageInfoLink() {}
		virtual ~CPageRankPlayerFleet() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_player_fleet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankPlayerPlanet: public CPageInfoLink
{
	public:
		CPageRankPlayerPlanet():CPageInfoLink() {}
		virtual ~CPageRankPlayerPlanet() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_player_planet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankPlayerTech: public CPageInfoLink
{
	public:
		CPageRankPlayerTech():CPageInfoLink() {}
		virtual ~CPageRankPlayerTech() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_player_tech.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankCouncilOverall: public CPageInfoLink
{
	public:
		CPageRankCouncilOverall():CPageInfoLink() {}
		virtual ~CPageRankCouncilOverall() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_council_overall.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankCouncilFleet: public CPageInfoLink
{
	public:
		CPageRankCouncilFleet():CPageInfoLink() {}
		virtual ~CPageRankCouncilFleet() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_council_fleet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankCouncilPlanet: public CPageInfoLink
{
	public:
		CPageRankCouncilPlanet():CPageInfoLink() {}
		virtual ~CPageRankCouncilPlanet() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_council_planet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRankCouncilDiplomatic: public CPageInfoLink
{
	public:
		CPageRankCouncilDiplomatic():CPageInfoLink() {}
		virtual ~CPageRankCouncilDiplomatic() {}

		virtual const char *get_name()
			{ return "/archspace/info/rank_council_diplomatic.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/* Special Event Pages */
class CPageSpecialLink: public CPageHeadTitle
{
	public:
		CPageSpecialLink():CPageHeadTitle() {}
		virtual ~CPageSpecialLink() {}

		virtual const char *get_name() { return "special_link"; }
	protected:
		virtual bool get_conversion();
};

class CPageSpecialEventSummary: public CPageSpecialLink
{
    public:
        CPageSpecialEventSummary():CPageSpecialLink() {}
        virtual ~CPageSpecialEventSummary() {}
        virtual const char *get_name() { return "/archspace/special/special_event_summary.as"; }
    protected:
        virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetLink: public CPageHeadTitle
{
	public:
		CPageFleetLink():CPageHeadTitle() {}
		virtual ~CPageFleetLink() {}

		virtual const char *get_name() { return "fleet_link"; }
	protected:
		virtual bool get_conversion();
};

/* start telecard 2000/10/09 */
/**
*/
class CPageShipDesign: public CPageFleetLink
{
	public:
		CPageShipDesign():CPageFleetLink() {}
		virtual ~CPageShipDesign() {}
		virtual const char *get_name() { return "/archspace/fleet/ship_design.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
/* end telecard 2000/10/09 */

/* start telecard 2000/10/10 */
/**
*/
class CPageShipDesign2: public CPageFleetLink
{
	public:
		CPageShipDesign2():CPageFleetLink() {}
		virtual ~CPageShipDesign2() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_design.as"; }
		virtual const char *get_name() { return "/archspace/fleet/ship_design2.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};
/* end telecard 2000/10/10 */

/* start telecard 2000/10/11 */
/**
*/
class CPageShipDesignConfirm: public CPageFleetLink
{
	public:
		CPageShipDesignConfirm():CPageFleetLink() {}
		virtual ~CPageShipDesignConfirm() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_design2.as"; }
		virtual const char *get_name() { return "/archspace/fleet/ship_design_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipDesignRegister: public CPageFleetLink
{
	public:
		CPageShipDesignRegister():CPageFleetLink() {}
		virtual ~CPageShipDesignRegister() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_design_confirm.as"; }
		virtual const char *get_name() { return "/archspace/fleet/ship_design_register.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};
/* end telecard 2000/10/11 */

/**
*/
class CPageShipDesignNotregister: public CPageFleetLink
{
	public:
		CPageShipDesignNotregister():CPageFleetLink() {}
		virtual ~CPageShipDesignNotregister() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_design_confirm.as"; }
		virtual const char *get_name() { return "/archspace/fleet/ship_design_notregister.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipDesignDeleteConfirm: public CPageFleetLink
{
	public:
		CPageShipDesignDeleteConfirm():CPageFleetLink() {}
		virtual ~CPageShipDesignDeleteConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_design_delete_confirm.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipDesignDeleteResult: public CPageFleetLink
{
	public:
		CPageShipDesignDeleteResult():CPageFleetLink() {}
		virtual ~CPageShipDesignDeleteResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_design_delete_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_design_delete_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipBuilding: public CPageFleetLink
{
	public:
		CPageShipBuilding():CPageFleetLink() {}
		virtual ~CPageShipBuilding() {}
		virtual const char *get_name() { return "/archspace/fleet/ship_building.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipBuildingInvest: public CPageFleetLink
{
	public:
		CPageShipBuildingInvest():CPageFleetLink() {}
		virtual ~CPageShipBuildingInvest() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_building.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_building_invest.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipBuildingInsert: public CPageFleetLink
{
	public:
		CPageShipBuildingInsert():CPageFleetLink() {}
		virtual ~CPageShipBuildingInsert() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_building.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_building_insert.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipBuildingDelete: public CPageFleetLink
{
	public:
		CPageShipBuildingDelete():CPageFleetLink() {}
		virtual ~CPageShipBuildingDelete() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_building_delete.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipPool: public CPageFleetLink
{
	public:
		CPageShipPool():CPageFleetLink() {}
		virtual ~CPageShipPool() {}
		virtual const char *get_name() { return "/archspace/fleet/ship_pool.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipPoolScrapConfirm: public CPageFleetLink
{
	public:
		CPageShipPoolScrapConfirm():CPageFleetLink() {}
		virtual ~CPageShipPoolScrapConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_pool_scrap_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageShipPoolScrapResult: public CPageFleetLink
{
	public:
		CPageShipPoolScrapResult():CPageFleetLink() {}
		virtual ~CPageShipPoolScrapResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/ship_pool_scrap_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/ship_pool_scrap_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleet: public CPageFleetLink
{
	public:
		CPageFleet():CPageFleetLink() {}
		virtual ~CPageFleet() {}
		virtual const char *get_name() { return "/archspace/fleet/fleet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFormNewFleet: public CPageFleetLink
{
	public:
		CPageFormNewFleet():CPageFleetLink() {}
		virtual ~CPageFormNewFleet() {}
		virtual const char *get_name() { return "/archspace/fleet/form_new_fleet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFormNewFleetConfirm: public CPageFleetLink
{
	public:
		CPageFormNewFleetConfirm():CPageFleetLink() {}
		virtual ~CPageFormNewFleetConfirm() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/form_new_fleet.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/form_new_fleet_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFormNewFleetResult: public CPageFleetLink
{
	public:
		CPageFormNewFleetResult():CPageFleetLink() {}
		virtual ~CPageFormNewFleetResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/form_new_fleet_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/form_new_fleet_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignment: public CPageFleetLink
{
	public:
		CPageReassignment():CPageFleetLink() {}
		virtual ~CPageReassignment() {}
		virtual const char *get_name() { return "/archspace/fleet/reassignment.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignment2: public CPageFleetLink
{
	public:
		CPageReassignment2():CPageFleetLink() {}
		virtual ~CPageReassignment2() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment.as"; }
		virtual const char *get_name() { return "/archspace/fleet/reassignment2.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageExpedition: public CPageFleetLink
{
	public:
		CPageExpedition():CPageFleetLink() {}
		virtual ~CPageExpedition() {}
		virtual const char *get_name() { return "/archspace/fleet/expedition.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageExpeditionConfirm: public CPageFleetLink
{
	public:
		CPageExpeditionConfirm():CPageFleetLink() {}
		virtual ~CPageExpeditionConfirm() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/expedition.as"; }
		virtual const char *get_name() { return "/archspace/fleet/expedition_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageExpeditionResult: public CPageFleetLink
{
	public:
		CPageExpeditionResult():CPageFleetLink() {}
		virtual ~CPageExpeditionResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/expedition_confirm.as"; }
		virtual const char *get_name() { return "/archspace/fleet/expedition_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMission: public CPageFleetLink
{
	public:
		CPageMission():CPageFleetLink() {}
		virtual ~CPageMission() {}
		virtual const char *get_name() { return "/archspace/fleet/mission.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMission2: public CPageFleetLink
{
	public:
		CPageMission2():CPageFleetLink() {}
		virtual ~CPageMission2() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/mission.as"; }
		virtual const char *get_name() { return "/archspace/fleet/mission2.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetCommander: public CPageFleetLink
{
	public:
		CPageFleetCommander():CPageFleetLink() {}
		virtual ~CPageFleetCommander() {}
		virtual const char *get_name() { return "/archspace/fleet/fleet_commander.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetCommanderInformation: public CPageFleetLink
{
	public:
		CPageFleetCommanderInformation():CPageFleetLink() {}
		virtual ~CPageFleetCommanderInformation() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/fleet_commander_information.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetCommanderViewOptions: public CPageFleetLink
{
	public:
		CPageFleetCommanderViewOptions():CPageFleetLink() {}
		virtual ~CPageFleetCommanderViewOptions() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/fleet_commander_view_options.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetCommanderDismissConfirm: public CPageFleetLink
{
	public:
		CPageFleetCommanderDismissConfirm():CPageFleetLink() {}
		virtual ~CPageFleetCommanderDismissConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/fleet_commander_dismiss_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageFleetCommanderDismissResult: public CPageFleetLink
{
	public:
		CPageFleetCommanderDismissResult():CPageFleetLink() {}
		virtual ~CPageFleetCommanderDismissResult() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/fleet_commander_dismiss_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMissionTrainResult: public CPageFleetLink
{
	public:
		CPageMissionTrainResult():CPageFleetLink() {}
		virtual ~CPageMissionTrainResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/mission2.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/mission_train_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMissionStationResult: public CPageFleetLink
{
	public:
		CPageMissionStationResult():CPageFleetLink() {}
		virtual ~CPageMissionStationResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/mission2.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/mission_station_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMissionPatrolResult: public CPageFleetLink
{
	public:
		CPageMissionPatrolResult():CPageFleetLink() {}
		virtual ~CPageMissionPatrolResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/mission2.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/mission_patrol_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageMissionDispatchResult: public CPageFleetLink
{
	public:
		CPageMissionDispatchResult():CPageFleetLink() {}
		virtual ~CPageMissionDispatchResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/mission2.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/mission_dispatch_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageRecall: public CPageFleetLink
{
	public:
		CPageRecall():CPageFleetLink() {}
		virtual ~CPageRecall() {}
		virtual const char *get_name() { return "/archspace/fleet/recall.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageRecallConfirm: public CPageFleetLink
{
	public:
		CPageRecallConfirm():CPageFleetLink() {}
		virtual ~CPageRecallConfirm() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/recall.as"; }
		virtual const char *get_name() { return "/archspace/fleet/recall_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageRecallResult: public CPageFleetLink
{
	public:
		CPageRecallResult():CPageFleetLink() {}
		virtual ~CPageRecallResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/recall_confirm.as"; }
		virtual const char *get_name() { return "/archspace/fleet/recall_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDisband: public CPageFleetLink
{
	public:
		CPageDisband():CPageFleetLink() {}
		virtual ~CPageDisband() {}
		virtual const char *get_name() { return "/archspace/fleet/disband.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDisbandConfirm: public CPageFleetLink
{
	public:
		CPageDisbandConfirm():CPageFleetLink() {}
		virtual ~CPageDisbandConfirm() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/disband.as"; }
		virtual const char *get_name() { return "/archspace/fleet/disband_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDisbandResult: public CPageFleetLink
{
	public:
		CPageDisbandResult():CPageFleetLink() {}
		virtual ~CPageDisbandResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/disband_confirm.as"; }
		virtual const char *get_name() { return "/archspace/fleet/disband_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentCommanderChangeConfirm: public CPageFleetLink
{
	public:
		CPageReassignmentCommanderChangeConfirm():CPageFleetLink() {}
		virtual ~CPageReassignmentCommanderChangeConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_commander_change_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentRemoveAddShipConfirm: public CPageFleetLink
{
	public:
		CPageReassignmentRemoveAddShipConfirm():CPageFleetLink() {}
		virtual ~CPageReassignmentRemoveAddShipConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_removeadd_ship_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentShipReassignmentConfirm: public CPageFleetLink
{
	public:
		CPageReassignmentShipReassignmentConfirm():CPageFleetLink() {}
		virtual ~CPageReassignmentShipReassignmentConfirm() {}
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_ship_reassignment_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentShipReassignmentResult: public CPageFleetLink
{
	public:
		CPageReassignmentShipReassignmentResult():CPageFleetLink() {}
		virtual ~CPageReassignmentShipReassignmentResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment_ship_reassignment_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_ship_reassignment_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentRefillFleetResult: public CPageFleetLink
{
	public:
		CPageReassignmentRefillFleetResult():CPageFleetLink() {}
		virtual ~CPageReassignmentRefillFleetResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment_ship_reassignment_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_refill_fleet_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentMaximizeFleetResult: public CPageFleetLink
{
	public:
		CPageReassignmentMaximizeFleetResult():CPageFleetLink() {}
		virtual ~CPageReassignmentMaximizeFleetResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment_ship_reassignment_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_maximize_fleet_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentCommanderChangeResult: public CPageFleetLink
{
	public:
		CPageReassignmentCommanderChangeResult():CPageFleetLink() {}
		virtual ~CPageReassignmentCommanderChangeResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment_commander_change_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_commander_change_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageReassignmentRemoveAddShipResult: public CPageFleetLink
{
	public:
		CPageReassignmentRemoveAddShipResult():CPageFleetLink() {}
		virtual ~CPageReassignmentRemoveAddShipResult() {}
//		virtual const char *get_referrer() { return "/archspace/fleet/reassignment_removeadd_ship_confirm.as"; }
		virtual const char *get_name()
			{ return "/archspace/fleet/reassignment_removeadd_ship_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//---------------------------------------------------------------- WAR
/**
*/
class CPageWarLink: public CPageHeadTitle
{
	public:
		virtual ~CPageWarLink() {}

		virtual const char *get_name() { return "war_link"; }
	protected:
		virtual bool get_conversion();
		virtual void message_page(const char *aMessage);
};

class CPageWar: public CPageWarLink
{
	public:
		CPageWar():CPageWarLink() {}
		virtual ~CPageWar() {}
		virtual const char *get_name() { return "/archspace/war/war.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageSiegePlanet: public CPageWarLink
{
	public:
		CPageSiegePlanet():CPageWarLink() {}
		virtual ~CPageSiegePlanet() {}
		virtual const char *get_name() { return "/archspace/war/siege_planet.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageSiegePlanetFleet: public CPageWarLink
{
	public:
		CPageSiegePlanetFleet():CPageWarLink() {}
		virtual ~CPageSiegePlanetFleet() {}
//		virtual const char *get_referrer() { return "/archspace/war/siege_planet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/siege_planet_fleet.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageSiegePlanetFleetDeployment: public CPageWarLink
{
	public:
		CPageSiegePlanetFleetDeployment():CPageWarLink() {}
		virtual ~CPageSiegePlanetFleetDeployment() {}
//		virtual const char *get_referrer() { return "/archspace/war/siege_planet_fleet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/siege_planet_fleet_deployment.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageSiegePlanetPlanet: public CPageWarLink
{
	public:
		CPageSiegePlanetPlanet():CPageWarLink() {}
		virtual ~CPageSiegePlanetPlanet() {}
//		virtual const char *get_referrer() { return "/archspace/war/siege_planet_fleet_deployment.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/siege_planet_planet.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageSiegePlanetResult: public CPageWarLink
{
	public:
		CPageSiegePlanetResult():CPageWarLink() {}
		virtual ~CPageSiegePlanetResult() {}
//		virtual const char *get_referrer() { return "/archspace/war/siege_planet_planet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/siege_planet_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlockade: public CPageWarLink
{
	public:
		CPageBlockade():CPageWarLink() {}
		virtual ~CPageBlockade() {}
		virtual const char *get_name() { return "/archspace/war/blockade.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlockadeFleet: public CPageWarLink
{
	public:
		CPageBlockadeFleet():CPageWarLink() {}
		virtual ~CPageBlockadeFleet() {}
//		virtual const char *get_referrer() { return "/archspace/war/blockade.as"; }
		virtual const char *get_name() { return "/archspace/war/blockade_fleet.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlockadeFleetDeployment: public CPageWarLink
{
	public:
		CPageBlockadeFleetDeployment():CPageWarLink() {}
		virtual ~CPageBlockadeFleetDeployment() {}
//		virtual const char *get_referrer() { return "/archspace/war/blockade_fleet.as"; }
		virtual const char *get_name()
			{ return "/archspace/war/blockade_fleet_deployment.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlockadePlanet: public CPageSiegePlanetPlanet
{
	public:
		virtual ~CPageBlockadePlanet() {}
//		virtual const char *get_referrer() { return "/archspace/war/blockade_fleet_deployment.as"; }
		virtual const char *get_name()
			{ return "/archspace/war/blockade_planet.as"; }
		virtual const char *allow_method() { return "POST GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlockadeResult: public CPageWarLink
{
	public:
		CPageBlockadeResult():CPageWarLink() {}
		virtual ~CPageBlockadeResult() {}
//		virtual const char *get_referrer() { return "/archspace/war/blockade_planet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/blockade_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRaid: public CPageWarLink
{
	public:
		CPageRaid():CPageWarLink() {}
		virtual ~CPageRaid() {}
		virtual const char *get_name() { return "/archspace/war/raid.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRaidFleet: public CPageWarLink
{
	public:
		CPageRaidFleet():CPageWarLink() {}
		virtual ~CPageRaidFleet() {}
//		virtual const char *get_referrer() { return "/archspace/war/raid.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/raid_fleet.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRaidPlanet: public CPageWarLink
{
	public:
		CPageRaidPlanet():CPageWarLink() {}
		virtual ~CPageRaidPlanet() {}
//		virtual const char *get_referrer() { return "/archspace/war/raid_fleet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/raid_planet.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRaidResult: public CPageWarLink
{
	public:
		CPageRaidResult():CPageWarLink() {}
		virtual ~CPageRaidResult() {}
//		virtual const char *get_referrer() { return "/archspace/war/raid_planet.as"; }
		virtual const char *get_name()
				{ return "/archspace/war/raid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePrivateer: public CPageWarLink
{
	public:
		CPagePrivateer():CPageWarLink() {}
		virtual ~CPagePrivateer() {}
		virtual const char *get_name() { return "/archspace/war/privateer.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePrivateerFleet: public CPageWarLink
{
	public:
		CPagePrivateerFleet():CPageWarLink() {}
		virtual ~CPagePrivateerFleet() {}
//		virtual const char *get_referrer() { return "/archspace/war/privateer.as"; }
		virtual const char *get_name() { return "/archspace/war/privateer_fleet.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePrivateerResult: public CPageWarLink
{
	public:
		CPagePrivateerResult():CPageWarLink() {}
		virtual ~CPagePrivateerResult() {}
//		virtual const char *get_referrer() { return "/archspace/war/privateer_fleet.as"; }
		virtual const char *get_name() { return "/archspace/war/privateer_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlan: public CPageWarLink
{
	public:
		CPageDefensePlan():CPageWarLink() {}
		virtual ~CPageDefensePlan() {}
		virtual const char *get_name() { return "/archspace/war/defense_plan.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanGenericNew: public CPageWarLink
{
	public:
		CPageDefensePlanGenericNew():CPageWarLink() {}
		virtual ~CPageDefensePlanGenericNew() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_generic_new.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanGenericDeployment: public CPageWarLink
{
	public:
		CPageDefensePlanGenericDeployment():CPageWarLink() {}
		virtual ~CPageDefensePlanGenericDeployment() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_generic_deployment.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanGenericResult: public CPageWarLink
{
	public:
		CPageDefensePlanGenericResult():CPageWarLink() {}
		virtual ~CPageDefensePlanGenericResult() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_generic_result.as"; }
		virtual const char *allow_method() { return "GET POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanGenericDelete: public CPageWarLink
{
	public:
		CPageDefensePlanGenericDelete():CPageWarLink() {}
		virtual ~CPageDefensePlanGenericDelete() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_generic_delete.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialNew: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialNew():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialNew() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_new.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialNewDeployment: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialNewDeployment():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialNewDeployment() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_new_deployment.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialNewResult: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialNewResult():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialNewResult() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_new_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialOld: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialOld():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialOld() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_old.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialChangeDeployment: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialChangeDeployment():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialChangeDeployment() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_change_deployment.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialChangeResult: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialChangeResult():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialChangeResult() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_change_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageDefensePlanSpecialDeleteResult: public CPageWarLink
{
	public:
		CPageDefensePlanSpecialDeleteResult():CPageWarLink() {}
		virtual ~CPageDefensePlanSpecialDeleteResult() {}
		virtual const char *get_name()
			{ return "/archspace/war/defense_plan_special_delete_result.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageBattleReport: public CPageWarLink
{
	public:
		CPageBattleReport():CPageWarLink() {}
		virtual ~CPageBattleReport() {}
		virtual const char* get_name() { return "/archspace/war/battle_report.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

/**
*/
class CPageBattleReport2: public CPageWarLink
{
	public:
		CPageBattleReport2():CPageWarLink() {}
		virtual ~CPageBattleReport2() {}
		virtual const char* get_name() { return "/archspace/war/battle_report2.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
// start telecard 2001/01/17
class CPageBattleReportDetail: public CPageWarLink
{
	public:
		CPageBattleReportDetail():CPageWarLink() {}
		virtual ~CPageBattleReportDetail() {}
		virtual const char* get_name() { return "/archspace/war/battle_report_detail.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
// end telecard
// start telecard 2001/01/19
class CPageBattleReportSearch: public CPageWarLink
{
	public:
		CPageBattleReportSearch():CPageWarLink() {}
		virtual ~CPageBattleReportSearch() {}
//		virtual const char *get_referrer() { return "/archspace/war/battle_report.as"; }
		virtual const char* get_name() { return "/archspace/war/battle_report_search.as"; }
		virtual const char* allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
//

/**
*/
class CPageClassSpecification: public CPageFleetLink
{
	public:
		CPageClassSpecification():CPageFleetLink() {}
		virtual ~CPageClassSpecification() {}
		virtual const char *get_name() { return "/archspace/fleet/class_specification.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//---------------------------------------------------------------- DIE
/**
*/
class CPageRestart : public CPageCommon
{
	public:
		virtual ~CPageRestart() {}
		virtual const char *get_name() { return "/archspace/restart.as"; }
		virtual const char *allow_method() { return "GET"; }
		virtual bool handle(CConnection& aConnection);
};

/**
*/
class CPageQuit : public CPageCommon
{
	public:
		virtual ~CPageQuit() {}
		virtual const char *get_name() { return "/archspace/quit.as"; }
		virtual const char *allow_method() { return "GET"; }
		virtual bool handle(CConnection& aConnection);
};

class CPageRetire : public CPageCommon
{
	public :
		virtual ~CPageRetire() {}

		virtual const char *get_name() { return "/archspace/retire.as"; }
		virtual bool handle(CConnection &aConnection);
};

class CPageRetireResult : public CPageCommon
{
	public :
		virtual ~CPageRetireResult() {}

//		virtual const char *get_referrer() { return "/archspace/retire.as"; }
		virtual const char *get_name() { return "/archspace/retire_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handle(CConnection &aConnection);
};

/**
*/
class CPageBlackMarketLink: public CPageHeadTitle
{
	public:
		CPageBlackMarketLink():CPageHeadTitle() {}
		virtual ~CPageBlackMarketLink() {}

		virtual const char *get_name() { return "blackmarket_link"; }
	protected:
		virtual bool get_conversion();
};

// start telecard 2001/02/22
class CPageBlackMarket : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarket() {}
		virtual const char *get_name() { return "/archspace/black_market/black_market.as"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/02/22

// start telecard 2001/02/27
class CPageBlackMarketTechDeck : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketTechDeck() {}
		virtual const char *get_name() { return "/archspace/black_market/tech_deck.as"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/02/27

// start telecard 2001/02/28
class CPageBlackMarketTechDeckBid : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketTechDeckBid() {}
		virtual const char *get_name() { return "/archspace/black_market/tech_deck_bid.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketTechDeckBidResult : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketTechDeckBidResult() {}
		virtual const char *get_name() { return "/archspace/black_market/tech_deck_bid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/02/28

// start telecard 2001/03/03
class CPageBlackMarketFleetDeck : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketFleetDeck() {}
		virtual const char *get_name() { return "/archspace/black_market/fleet_deck.as"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/03

// start telecard 2001/03/05
class CPageBlackMarketFleetDeckAdmiral : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketFleetDeckAdmiral() {};
		virtual const char *get_name() { return "/archspace/black_market/fleet_deck_admiral.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/05

// start telecard 2001/03/13
class CPageBlackMarketFleetDeckAdmiralBid : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketFleetDeckAdmiralBid() {};
		virtual const char *get_name() { return "/archspace/black_market/fleet_deck_admiral_bid.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketFleetDeckAdmiralBidResult : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketFleetDeckAdmiralBidResult() {};
		virtual const char *get_name() { return "/archspace/black_market/fleet_deck_admiral_bid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/13

// start telecard 2001/03/06
class CPageBlackMarketOfficersLounge : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketOfficersLounge() {};
		virtual const char *get_name() { return "/archspace/black_market/officers_lounge.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketOfficersLoungeBid : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketOfficersLoungeBid() {};
		virtual const char *get_name() { return "/archspace/black_market/officers_lounge_bid.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketOfficersLoungeBidResult : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketOfficersLoungeBidResult() {};
		virtual const char *get_name() { return "/archspace/black_market/officers_lounge_bid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

//start telecard 2001/03/14
class CPageBlackMarketRareGoods : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketRareGoods() {};
		virtual const char *get_name() { return "/archspace/black_market/rare_goods.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketRareGoodsBid : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketRareGoodsBid() {};
		virtual const char *get_name() { return "/archspace/black_market/rare_goods_bid.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageBlackMarketRareGoodsBidResult : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketRareGoodsBidResult() {};
		virtual const char *get_name() { return "/archspace/black_market/rare_goods_bid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
//end telecard 2001/03/14

class CPageBlackMarketLeasersOffice : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketLeasersOffice() {}
		virtual const char *get_name() { return "/archspace/black_market/leasers_office.as"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/06

// start telecard 2001/03/07
class CPageBlackMarketLeasersOfficeBid : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketLeasersOfficeBid() {}
		virtual const char *get_name() { return "/archspace/black_market/leasers_office_bid.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/07

// start telecard 2001/03/08
class CPageBlackMarketLeasersOfficeBidResult : public CPageBlackMarketLink
{
	public:
		virtual ~CPageBlackMarketLeasersOfficeBidResult() {}
		virtual const char *get_name() { return "/archspace/black_market/leasers_office_bid_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};
// end telecard 2001/03/08

class CPageEmpireLink: public CPageHeadTitle
{
	public:
		CPageEmpireLink();
		virtual ~CPageEmpireLink() {}

		virtual const char *get_name() { return "empire_link"; }
	protected:
		virtual bool get_conversion();
		CString
			*mBribeDescription,
			*mRequestBoonDescription,
			*mRequestMediationDescription,
			*mInvadeMagistrateDominionDescription,
			*mInvadeEmpireDescription,
			*mDemandTributeDescription,
			*mDemandLeaveCouncilDescription,
			*mDemandDeclareWarDescription,
			*mDemandMakeTruceDescription,
			*mDemandFreeSubordinaryCouncilDescription,
			*mDemandArmamentReductionDescription,
			*mGrantRankDescription,
			*mGrantBoonDescription;
};

class CPageEmpireDiplomacy : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireDiplomacy() {};
		virtual const char *get_name() { return "/archspace/empire/empire_diplomacy.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireShowAction: public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireShowAction() {};
		virtual const char *get_name() { return "/archspace/empire/show_action.as"; }
		virtual const char *allow_method() { return "GET"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireAnswerAction: public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireAnswerAction() {};
		virtual const char *get_name() { return "/archspace/empire/answer_action.as"; }
		virtual const char *allow_method() { return "GET"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireBounty : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireBounty() {};
		virtual const char *get_name() { return "/archspace/empire/bounty.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireBountyConfirm : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireBountyConfirm() {};
		virtual const char *get_name() { return "/archspace/empire/bounty_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireBountyResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireBountyResult() {};
		virtual const char *get_name() { return "/archspace/empire/bounty_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireBribe : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireBribe() {};
		virtual const char *get_name() { return "/archspace/empire/bribe.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireBribeResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireBribeResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/bribe.as"; }
		virtual const char *get_name() { return "/archspace/empire/bribe_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireRequestBoon : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireRequestBoon() {};
		virtual const char *get_name() { return "/archspace/empire/request_boon.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireRequestBoonResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireRequestBoonResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/request_boon.as"; }
		virtual const char *get_name() { return "/archspace/empire/request_boon_result.as"; }

		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireRequestMediation : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireRequestMediation() {};
		virtual const char *get_name() { return "/archspace/empire/request_mediation.as"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireRequestMediationResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireRequestMediationResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/request_mediation.as"; }
		virtual const char *get_name() { return "/archspace/empire/request_mediation_result.as"; }
		virtual const char *allow_method() { return "POST"; }
		virtual bool handler( CPlayer *aPlayer );
};

class CPageEmpireInvadeEmpire : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpire() {};
		virtual const char *get_name() { return "/archspace/empire/invade_empire.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeMagistrateConfirm : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeMagistrateConfirm() {};
		virtual const char *get_name()
				{ return "/archspace/empire/invade_magistrate_confirm.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeMagistrateFleet : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeMagistrateFleet() {};
		virtual const char *get_name() { return "/archspace/empire/invade_magistrate_fleet.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeMagistrateFleetDeployment : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeMagistrateFleetDeployment() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_magistrate_fleet_deployment.as"; }
		virtual char *allow_method() { return "POST"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeMagistrateResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeMagistrateResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/invade_magistrate_fleet_deployment.as"; }
		virtual const char *get_name()
				{ return "/archspace/empire/invade_magistrate_result.as"; }
		virtual char *allow_method() { return "POST GET"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpirePlanetConfirm : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpirePlanetConfirm() {};
		virtual const char *get_name()
				{ return "/archspace/empire/invade_empire_planet_confirm.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpirePlanetFleet : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpirePlanetFleet() {};
		virtual const char *get_name()
				{ return "/archspace/empire/invade_empire_planet_fleet.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpirePlanetFleetDeployment : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpirePlanetFleetDeployment() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_empire_planet_fleet_deployment.as"; }
		virtual char *allow_method() { return "POST"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpirePlanetResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpirePlanetResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/invade_empire_planet_fleet_deployment.as"; }
		virtual const char *get_name()
				{ return "/archspace/empire/invade_empire_planet_result.as"; }
		virtual char *allow_method() { return "POST GET"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeFortressFleet : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeFortressFleet() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_fortress_fleet.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeFortressFleetDeployment : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeFortressFleetDeployment() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_fortress_fleet_deployment.as"; }
		virtual char *allow_method() { return "POST"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeFortressResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeFortressResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/invade_fortress_fleet_deployment.as"; }
		virtual char *get_name()
				{ return "/archspace/empire/invade_fortress_result.as"; }
		virtual char *allow_method() { return "POST GET"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpireCapitalPlanetConfirm : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpireCapitalPlanetConfirm() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_empire_capital_planet_confirm.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpireCapitalPlanetFleet : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpireCapitalPlanetFleet() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_empire_capital_planet_fleet.as"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpireCapitalPlanetFleetDeployment : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpireCapitalPlanetFleetDeployment() {};
		virtual char *get_name()
				{ return "/archspace/empire/invade_empire_capital_planet_fleet_deployment.as"; }
		virtual char *allow_method() { return "POST"; }
		virtual bool handler(CPlayer *aPlayer);
};

class CPageEmpireInvadeEmpireCapitalPlanetResult : public CPageEmpireLink
{
	public:
		virtual ~CPageEmpireInvadeEmpireCapitalPlanetResult() {};
//		virtual const char *get_referrer() { return "/archspace/empire/invade_capital_planets_fleet_deployment.as"; }
		virtual char *get_name()
				{ return "/archspace/empire/invade_empire_capital_planet_result.as"; }
		virtual char *allow_method() { return "POST GET"; }
		virtual bool handler(CPlayer *aPlayer);
};

//---------------------------------------------------------------- ADMIN TOOL

class CPageAdminLink: public CPageHeadTitle
{
	enum EAccessLevel
	{
		AL_SUPER_USER = 0,
		AL_MODERATOR
	};

	public:
		virtual ~CPageAdminLink() {}

		virtual const char *get_name() { return "admin_link"; }

//		virtual int access_level() = 0;

	protected:
		virtual bool get_conversion();
};

class CPageAdmin: public CPageAdminLink
{
	public:
		CPageAdmin():CPageAdminLink() {}
		virtual ~CPageAdmin() {}
		virtual const char* get_name() { return "/archspace/admin/admin.as"; }
		virtual const char *allow_method() { return "GET POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageInspect: public CPageAdminLink
{
	public:
		CPageInspect():CPageAdminLink() {}
		virtual ~CPageInspect() {}
		virtual const char* get_name() { return "/archspace/admin/inspect.as"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerConfirm: public CPageAdminLink
{
	public:
		CPagePlayerConfirm():CPageAdminLink() {}
		virtual ~CPagePlayerConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayer: public CPageAdminLink
{
	public:
		CPagePlayer():CPageAdminLink() {}
		virtual ~CPagePlayer() {}
		virtual const char* get_name() { return "/archspace/admin/player.as"; }
		virtual const char *allow_method() { return "GET POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerResearchStatus : public CPageAdminLink
{
	public :
		CPagePlayerResearchStatus():CPageAdminLink() {}
		virtual ~CPagePlayerResearchStatus() {}
		virtual const char *get_name()
			{ return "/archspace/admin/player_research_status.as"; }
		virtual const char *allow_method() { return "GET"; }

	protected :
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerSeeDomestic : public CPageAdminLink
{
	public:
		CPagePlayerSeeDomestic():CPageAdminLink() {}
		virtual ~CPagePlayerSeeDomestic() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_see_domestic.as"; }
		virtual const char *allow_method() { return "GET"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerMessage: public CPageAdminLink
{
	public:
		CPagePlayerMessage():CPageAdminLink() {}
		virtual ~CPagePlayerMessage() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_message.as"; }
		virtual const char *allow_method() { return "GET POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerMessageRead: public CPageAdminLink
{
	public:
		CPagePlayerMessageRead():CPageAdminLink() {}
		virtual ~CPagePlayerMessageRead() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_message_read.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerMessageDeleteConfirm: public CPageAdminLink
{
	public:
		CPagePlayerMessageDeleteConfirm():CPageAdminLink() {}
		virtual ~CPagePlayerMessageDeleteConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_message_delete_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerMessageDeleteResult: public CPageAdminLink
{
	public:
		CPagePlayerMessageDeleteResult():CPageAdminLink() {}
		virtual ~CPagePlayerMessageDeleteResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_message_delete_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilMessage: public CPageAdminLink
{
	public:
		CPageCouncilMessage():CPageAdminLink() {}
		virtual ~CPageCouncilMessage() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_message.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilMessageRead: public CPageAdminLink
{
	public:
		CPageCouncilMessageRead():CPageAdminLink() {}
		virtual ~CPageCouncilMessageRead() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_message_read.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilMessageDeleteConfirm: public CPageAdminLink
{
	public:
		CPageCouncilMessageDeleteConfirm():CPageAdminLink() {}
		virtual ~CPageCouncilMessageDeleteConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_message_delete_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilMessageDeleteResult: public CPageAdminLink
{
	public:
		CPageCouncilMessageDeleteResult():CPageAdminLink() {}
		virtual ~CPageCouncilMessageDeleteResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_message_delete_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerSendMessage: public CPageAdminLink
{
	public:
		CPagePlayerSendMessage():CPageAdminLink() {}
		virtual ~CPagePlayerSendMessage() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_send_message.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerSendMessageResult: public CPageAdminLink
{
	public:
		CPagePlayerSendMessageResult():CPageAdminLink() {}
		virtual ~CPagePlayerSendMessageResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_send_message_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerPunishment: public CPageAdminLink
{
	public:
		CPagePlayerPunishment():CPageAdminLink() {}
		virtual ~CPagePlayerPunishment() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_punishment.as"; }
		virtual const char *allow_method() { return "GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerPunishmentResult: public CPageAdminLink
{
	public:
		CPagePlayerPunishmentResult():CPageAdminLink() {}
		virtual ~CPagePlayerPunishmentResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_punishment_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerPunishmentDeleteCharacterConfirm: public CPageAdminLink
{
	public:
		CPagePlayerPunishmentDeleteCharacterConfirm():CPageAdminLink() {}
		virtual ~CPagePlayerPunishmentDeleteCharacterConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_punishment_delete_character_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerPunishmentDeleteCharacterResult: public CPageAdminLink
{
	public:
		CPagePlayerPunishmentDeleteCharacterResult():CPageAdminLink() {}
		virtual ~CPagePlayerPunishmentDeleteCharacterResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/player_punishment_delete_character_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageAdminCouncil: public CPageAdminLink
{
	public:
		CPageAdminCouncil():CPageAdminLink() {}
		virtual ~CPageAdminCouncil() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilControl: public CPageAdminLink
{
	public:
		CPageCouncilControl():CPageAdminLink() {}
		virtual ~CPageCouncilControl() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_control.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageCouncilControlResult: public CPageAdminLink
{
	public:
		CPageCouncilControlResult():CPageAdminLink() {}
		virtual ~CPageCouncilControlResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/council_control_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlackMarketControl: public CPageAdminLink
{
	public:
		CPageBlackMarketControl():CPageAdminLink() {}
		virtual ~CPageBlackMarketControl() {}
		virtual const char* get_name()
			{ return "/archspace/admin/black_market_control.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBlackMarketControlResult: public CPageAdminLink
{
	public:
		CPageBlackMarketControlResult():CPageAdminLink() {}
		virtual ~CPageBlackMarketControlResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/black_market_control_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageConfig: public CPageAdminLink
{
	public:
		CPageConfig():CPageAdminLink() {}
		virtual ~CPageConfig() {}
		virtual const char* get_name()
			{ return "/archspace/admin/config.as"; }
		virtual const char *allow_method() { return "GET POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRebootConfirm: public CPageAdminLink
{
	public:
		CPageRebootConfirm():CPageAdminLink() {}
		virtual ~CPageRebootConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/reboot_confirm.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageRebootResult: public CPageAdminLink
{
	public:
		CPageRebootResult():CPageAdminLink() {}
		virtual ~CPageRebootResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/reboot_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageResetConfirm: public CPageAdminLink
{
	public:
		CPageResetConfirm():CPageAdminLink() {}
		virtual ~CPageResetConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/reset_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageResetResult: public CPageAdminLink
{
	public:
		CPageResetResult():CPageAdminLink() {}
		virtual ~CPageResetResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/reset_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBroadcastConfirm: public CPageAdminLink
{
	public:
		CPageBroadcastConfirm():CPageAdminLink() {}
		virtual ~CPageBroadcastConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/admin/broadcast_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPageBroadcastResult: public CPageAdminLink
{
	public:
		CPageBroadcastResult():CPageAdminLink() {}
		virtual ~CPageBroadcastResult() {}
		virtual const char* get_name()
			{ return "/archspace/admin/broadcast_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

//start telecard 2001/04/02
class CPagePersonalEnding: public CPageHeadTitle
{
	public:
		CPagePersonalEnding():CPageHeadTitle() {}
		virtual ~CPagePersonalEnding() {}
		virtual const char* get_name() { return "/archspace/personal_ending.as"; }
		virtual const char *allow_method() { return "POST"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};
//end telecard 2001/04/02

/* thedaz for test */
class CTestPage: public CPageGame
{
	public:
		CTestPage():CPageGame() {}
		virtual ~CTestPage() {}
		virtual const char* get_name() { return "/archspace/test_page.as"; }
		virtual const char *allow_method() { return "GET"; }
	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerDismiss: public CPageSpeakerMenu
{
	public:
		//CPagePlayerDismiss() {}
		virtual ~CPagePlayerDismiss() {}
		virtual const char* get_name()
			{ return "/archspace/council/player_dismiss.as"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerDismissConfirm: public CPageSpeakerMenu
{
	public:
		//CPagePlayerDismiss() {}
		virtual ~CPagePlayerDismissConfirm() {}
		virtual const char* get_name()
			{ return "/archspace/council/player_dismiss_confirm.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

class CPagePlayerDismissResult: public CPageSpeakerMenu
{
	public:
		//CPagePlayerDismiss() {}
		virtual ~CPagePlayerDismissResult() {}
		virtual const char* get_name()
			{ return "/archspace/council/player_dismiss_result.as"; }
		virtual const char *allow_method() { return "POST"; }

	protected:
		virtual bool handler(CPlayer *aPlayer);
};

#endif
