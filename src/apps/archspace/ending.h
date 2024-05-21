
#if !defined(__ARCHSPACE_ENDING_H__)
#define __ARCHSPACE_ENDING_H__

#include "rank.h"

class CRank;
class CRankTable;
class CProject;
class CRace;

class CGlobalEnding
{
	public :
		static int
			mScorePerPlanet,
			mPopulationPerScore,
			mScorePerTechLevel,
			mAdmiralExpPerScore,
			mProjectPricePerScore,
			mScorePerSecretProject,
			mScorePerUsedTurn,

			mMultiplierForPersonalEnding,
			mMultiplierForAllKnownTechs,
			mMultiplierForTitle,
			mMultiplierForSpeaker,
			mMultiplierForHonor1,
			mMultiplierForHonor2,

			mScorePerFortressForPlayer,
			mScorePerFortressForCouncilLayer1,
			mScorePerFortressForCouncilLayer2,
			mScorePerFortressForCouncilLayer3,
			mScorePerFortressForCouncilLayer4,
			mScorePerEmpireCapitalPlanet;

	public :
		CGlobalEnding();
		~CGlobalEnding();

		bool add_player_score(CRank *aScore);
		CRankTable *get_player_score_list() { return &mPlayerScoreList; }

		bool add_council_score(CRank *aScore);
		CRankTable *get_council_score_list() { return &mCouncilScoreList; }

		bool is_final_score() { return mIsFinalScore; }
		void set_final_score();

	public :
		char *get_score_html(CPlayer *aPlayer);

	private :
		CRankTable
			mPlayerScoreList,
			mCouncilScoreList;

		bool
			mIsFinalScore;
};

#endif
