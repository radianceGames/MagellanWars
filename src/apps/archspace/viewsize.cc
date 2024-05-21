#include <cstdio>
#include "archspace.h"
#include "component.h"
#include "action.h"
#include "council.h"
#include "script.h"
#include "banner.h"
#include "battle.h"
#include "encyclopedia.h"
#include "game.h"
#include "race.h"
#include "portal.h"

int main()
{
	long long int total = 0;
	printf("--------------------Game Server Class Size-----------------\n");
	printf("class CArchspace: %d Bytes\n", 
			sizeof(CArchspace));
	total += sizeof(CArchspace);
	printf("class CArchspacePageStation: %d Bytes\n", 
			sizeof(CArchspacePageStation));
	total += sizeof(CArchspacePageStation);
	printf("class CArchspaceTriggerStation: %d Bytes\n", 
			sizeof(CArchspaceTriggerStation));
	total += sizeof(CArchspaceTriggerStation);
	printf("class CArchspaceDatabase: %d Bytes\n", 
			sizeof(CArchspaceDatabase));
	total += sizeof(CArchspaceDatabase);
	printf("class CArchspaceConnection: %d Bytes\n", 
			sizeof(CArchspaceConnection));
	total += sizeof(CArchspaceConnection);


	printf("class CArmor: %d Bytes\n", 
			sizeof(CArmor));
	total += sizeof(CArmor);
	printf("class CAction: %d Bytes\n", 
			sizeof(CAction));
	total += sizeof(CAction);
	printf("class CActionList: %d Bytes\n", 
			sizeof(CActionList));
	total += sizeof(CActionList);
	printf("class CAdmission: %d Bytes\n", 
			sizeof(CAdmission));
	total += sizeof(CAdmission);
	printf("class CAdmissionList: %d Bytes\n", 
			sizeof(CAdmissionList));
	total += sizeof(CAdmissionList);

	printf("class CAdmiral: %d Bytes\n", 
			sizeof(CAdmiral));
	total += sizeof(CAdmiral);
	printf("class CAdmiralList: %d Bytes\n", 
			sizeof(CAdmiralList));
	total += sizeof(CAdmiralList);
	printf("class CAdmiralNameList: %d Bytes\n", 
			sizeof(CAdmiralNameList));
	total += sizeof(CAdmiralNameList);
	printf("class CAdmiralNameTable: %d Bytes\n", 
			sizeof(CAdmiralNameTable));
	total += sizeof(CAdmiralNameTable);
	printf("class CAdmiralNameScript: %d Bytes\n", 
			sizeof(CAdmiralNameScript));
	total += sizeof(CAdmiralNameScript);
	printf("class CBanner: %d Bytes\n", 
			sizeof(CBanner));
	total += sizeof(CBanner);
//	printf("class CBannerList: %d Bytes\n", 
//			sizeof(CBannerList));
	printf("class CBannerCenter: %d Bytes\n", 
			sizeof(CBannerCenter));


	printf("class CBattle: %d Bytes\n", 
			sizeof(CBattle));
	total += sizeof(CBattle);
	printf("class CBattleRecord: %d Bytes\n", 
			sizeof(CBattleRecord));
	total += sizeof(CBattleRecord);
	printf("class CBattleRecordTable: %d Bytes\n", 
			sizeof(CBattleRecordTable));
	total += sizeof(CBattleRecordTable);
	printf("class CBattleFleet: %d Bytes\n", 
			sizeof(CBattleFleet));
	total += sizeof(CBattleFleet);
	printf("class CBattleFleetList: %d Bytes\n", 
			sizeof(CBattleFleetList));
	total += sizeof(CBattleFleetList);




	printf("class CCluster: %d Bytes\n", 
			sizeof(CCluster));
	total += sizeof(CCluster);
	printf("class CClusterNameScript: %d Bytes\n", 
			sizeof(CClusterNameScript));
	total += sizeof(CClusterNameScript);

	printf("class CComponent: %d Bytes\n", 
			sizeof(CComponent));
	total += sizeof(CComponent);
	printf("class CComponentList: %d Bytes\n", 
			sizeof(CComponentList));
	total += sizeof(CComponentList);
	printf("class CComponentTable: %d Bytes\n", 
			sizeof(CComponentTable));
	total += sizeof(CComponentTable);
	printf("class CComponentScript: %d Bytes\n", 
			sizeof(CComponentScript));
	total += sizeof(CComponentScript);

	printf("class CComputer: %d Bytes\n", 
			sizeof(CComputer));
	total += sizeof(CComputer);
	printf("class CControlModel: %d Bytes\n", 
			sizeof(CControlModel));
	total += sizeof(CControlModel);



	printf("class CCouncil: %d Bytes\n", 
			sizeof(CCouncil));
	total += sizeof(CCouncil);
	printf("class CCouncilList: %d Bytes\n", 
			sizeof(CCouncilList));
	total += sizeof(CCouncilList);
	printf("class CCouncilTable: %d Bytes\n", 
			sizeof(CCouncilTable));
	total += sizeof(CCouncilTable);
	printf("class CCouncilAction: %d Bytes\n", 
			sizeof(CCouncilAction));
	total += sizeof(CCouncilAction);
	printf("class CCouncilActionList: %d Bytes\n", 
			sizeof(CCouncilActionList));
	total += sizeof(CCouncilActionList);
	printf("class CCouncilActionTable: %d Bytes\n", 
			sizeof(CCouncilActionTable));
	total += sizeof(CCouncilActionTable);

	printf("class CCouncilActionBreakAlly: %d Bytes\n", 
			sizeof(CCouncilActionBreakAlly));
	total += sizeof(CCouncilActionBreakAlly);
	printf("class CCouncilActionBreakPact: %d Bytes\n", 
			sizeof(CCouncilActionBreakPact));
	total += sizeof(CCouncilActionBreakPact);
	printf("class CCouncilActionDeclareTotalWar: %d Bytes\n", 
			sizeof(CCouncilActionDeclareTotalWar));
	total += sizeof(CCouncilActionDeclareTotalWar);
	printf("class CCouncilActionDeclareWar: %d Bytes\n", 
			sizeof(CCouncilActionDeclareWar));
	total += sizeof(CCouncilActionDeclareWar);
	printf("class CCouncilActionImproveRelation: %d Bytes\n", 
			sizeof(CCouncilActionImproveRelation));
	total += sizeof(CCouncilActionImproveRelation);
	printf("class CCouncilRelation: %d Bytes\n", 
			sizeof(CCouncilRelation));
	total += sizeof(CCouncilRelation);
	printf("class CCouncilRelationList: %d Bytes\n", 
			sizeof(CCouncilRelationList));
	total += sizeof(CCouncilRelationList);
	printf("class CCouncilRelationTable: %d Bytes\n", 
			sizeof(CCouncilRelationTable));
	total += sizeof(CCouncilRelationTable);
	

	printf("class CCouncilMessage: %d Bytes\n", 
			sizeof(CCouncilMessage));
	total += sizeof(CCouncilMessage);
	printf("class CCouncilMessageBox: %d Bytes\n", 
			sizeof(CCouncilMessageBox));
	total += sizeof(CCouncilMessageBox);

	printf("class CDefenseFleet: %d Bytes\n", 
			sizeof(CDefenseFleet));
	total += sizeof(CDefenseFleet);
	printf("class CDefenseFleetList: %d Bytes\n", 
			sizeof(CDefenseFleetList));
	total += sizeof(CDefenseFleetList);
	printf("class CDefensePlan: %d Bytes\n", 
			sizeof(CDefensePlan));
	total += sizeof(CDefensePlan);
	printf("class CDefensePlanList: %d Bytes\n", 
			sizeof(CDefensePlanList));
	total += sizeof(CDefensePlanList);

	printf("class CDevice: %d Bytes\n", 
			sizeof(CDevice));
	total += sizeof(CDevice);

	printf("class CDiplomaticMessage: %d Bytes\n", 
			sizeof(CDiplomaticMessage));
	total += sizeof(CDiplomaticMessage);
	printf("class CDiplomaticMessageBox: %d Bytes\n", 
			sizeof(CDiplomaticMessageBox));
	total += sizeof(CDiplomaticMessageBox);



	printf("class CEncyclopedia: %d Bytes\n", 
			sizeof(CEncyclopedia));
	total += sizeof(CEncyclopedia);

	printf("class CEncyclopediaTechIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaTechIndexGame));
	total += sizeof(CEncyclopediaTechIndexGame);
	printf("class CEncyclopediaTechPageGame: %d Bytes\n", 
			sizeof(CEncyclopediaTechPageGame));
	total += sizeof(CEncyclopediaTechPageGame);

	printf("class CEncyclopediaRaceIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaRaceIndexGame));
	total += sizeof(CEncyclopediaRaceIndexGame);
	printf("class CEncyclopediaRacePageGame: %d Bytes\n", 
			sizeof(CEncyclopediaRacePageGame));
	total += sizeof(CEncyclopediaRacePageGame);

	printf("class CEncyclopediaComponentIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaComponentIndexGame));
	total += sizeof(CEncyclopediaComponentIndexGame);
	printf("class CEncyclopediaComponentPageGame: %d Bytes\n", 
			sizeof(CEncyclopediaComponentPageGame));
	total += sizeof(CEncyclopediaComponentPageGame);

	printf("class CEncyclopediaProjectIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaProjectIndexGame));
	total += sizeof(CEncyclopediaProjectIndexGame);
	printf("class CEncyclopediaProjectPageGame: %d Bytes\n", 
			sizeof(CEncyclopediaProjectPageGame));
	total += sizeof(CEncyclopediaProjectPageGame);

	printf("class CEncyclopediaShipIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaShipIndexGame));
	total += sizeof(CEncyclopediaShipIndexGame);
	printf("class CEncyclopediaShipPageGame: %d Bytes\n", 
			sizeof(CEncyclopediaShipPageGame));
	total += sizeof(CEncyclopediaShipPageGame);

	printf("class CEncyclopediaSpyIndexGame: %d Bytes\n", 
			sizeof(CEncyclopediaSpyIndexGame));
	total += sizeof(CEncyclopediaSpyIndexGame);
	printf("class CEncyclopediaSpyPageGame: %d Bytes\n", 
			sizeof(CEncyclopediaSpyPageGame));
	total += sizeof(CEncyclopediaSpyPageGame);




	printf("class CEngine: %d Bytes\n", 
			sizeof(CEngine));
	total += sizeof(CEngine);

	printf("class CFleet: %d Bytes\n", 
			sizeof(CFleet));
	total += sizeof(CFleet);

	printf("class CFleetList: %d Bytes\n", 
			sizeof(CFleetList));
	total += sizeof(CFleetList);

	printf("class CGame: %d Bytes\n", 
			sizeof(CGame));
	total += sizeof(CGame);

	printf("class CKnownTech: %d Bytes\n", 
			sizeof(CKnownTech));
	total += sizeof(CKnownTech);

	printf("class CKnownTechList: %d Bytes\n", 
			sizeof(CKnownTechList));
	total += sizeof(CKnownTechList);






	printf("class CMission: %d Bytes\n", 
			sizeof(CMission));
	total += sizeof(CMission);

	printf("class CNews: %d Bytes\n", 
			sizeof(CNews));
	total += sizeof(CNews);

	printf("class CNewsCenter: %d Bytes\n", 
			sizeof(CNewsCenter));
	total += sizeof(CNewsCenter);

	printf("class CKnownTechNews: %d Bytes\n", 
			sizeof(CKnownTechNews));
	total += sizeof(CKnownTechNews);

	printf("class CPlanetNews: %d Bytes\n", 
			sizeof(CPlanetNews));
	total += sizeof(CPlanetNews);

	printf("class CPlanetNewsCenter: %d Bytes\n", 
			sizeof(CPlanetNewsCenter));
	total += sizeof(CPlanetNewsCenter);


	printf("class CPurchasedProjectNews: %d Bytes\n", 
			sizeof(CPurchasedProjectNews));
	total += sizeof(CPurchasedProjectNews);

	printf("class CAdmiralNews: %d Bytes\n", 
			sizeof(CAdmiralNews));
	total += sizeof(CAdmiralNews);


	printf("class CPlanet: %d Bytes\n", 
			sizeof(CPlanet));
	total += sizeof(CPlanet);

	printf("class CPlanetList: %d Bytes\n", 
			sizeof(CPlanetList));
	total += sizeof(CPlanetList);

	printf("class CPlanetTable: %d Bytes\n", 
			sizeof(CPlanetTable));
	total += sizeof(CPlanetTable);



	printf("class CPlayer: %d Bytes\n", 
			sizeof(CPlayer));
	total += sizeof(CPlayer);

	printf("class CPlayerList: %d Bytes\n", 
			sizeof(CPlayerList));
	total += sizeof(CPlayerList);

	printf("class CPlayerTable: %d Bytes\n", 
			sizeof(CPlayerTable));
	total += sizeof(CPlayerTable);


	printf("class CPlayerAction: %d Bytes\n", 
			sizeof(CPlayerAction));
	total += sizeof(CPlayerAction);

	printf("class CPlayerActionList: %d Bytes\n", 
			sizeof(CPlayerActionList));
	total += sizeof(CPlayerActionList);

	printf("class CPlayerActionTable: %d Bytes\n", 
			sizeof(CPlayerActionTable));
	total += sizeof(CPlayerActionTable);

	printf("class CPlayerActionCouncilDonation: %d Bytes\n", 
			sizeof(CPlayerActionCouncilDonation));
	total += sizeof(CPlayerActionCouncilDonation);

	printf("class CPlayerActionBreakAllay: %d Bytes\n", 
			sizeof(CPlayerActionBreakAlly));
	total += sizeof(CPlayerActionBreakAlly);

	printf("class CPlayerActionBreakPact: %d Bytes\n", 
			sizeof(CPlayerActionBreakPact));
	total += sizeof(CPlayerActionBreakPact);

	printf("class CPlayerRelation: %d Bytes\n", 
			sizeof(CPlayerRelation));
	total += sizeof(CPlayerRelation);

	printf("class CPlayerRelationList: %d Bytes\n", 
			sizeof(CPlayerRelationList));
	total += sizeof(CPlayerRelationList);

	printf("class CPlayerRelationTable: %d Bytes\n", 
			sizeof(CPlayerRelationTable));
	total += sizeof(CPlayerRelationTable);






	printf("class CPProject: %d Bytes\n", 
			sizeof(CProject));
	total += sizeof(CProject);
	printf("class CPProjectTable: %d Bytes\n", 
			sizeof(CProjectTable));
	total += sizeof(CProjectTable);
	printf("class CPProjectScript: %d Bytes\n", 
			sizeof(CProjectScript));
	total += sizeof(CProjectScript);




	printf("class CPurchasedProject: %d Bytes\n", 
			sizeof(CPurchasedProject));
	total += sizeof(CPurchasedProject);



	printf("class CRace: %d Bytes\n", 
			sizeof(CRace));
	total += sizeof(CRace);

	printf("class CRaceTable: %d Bytes\n", 
			sizeof(CRaceTable));
	total += sizeof(CRaceTable);
	printf("class CRaceScript: %d Bytes\n", 
			sizeof(CRaceScript));
	total += sizeof(CRaceScript);



	printf("class CRelation: %d Bytes\n", 
			sizeof(CRelation));
	total += sizeof(CRelation);
	printf("class CRepairBay: %d Bytes\n", 
			sizeof(CRepairBay));
	total += sizeof(CRepairBay);

	printf("class CResource: %d Bytes\n", 
			sizeof(CResource));
	total += sizeof(CResource);

	printf("class CShield: %d Bytes\n", 
			sizeof(CShield));
	total+= sizeof(CShield);

	printf("class CShipSize: %d Bytes\n", 
			sizeof(CShipSize));
	total += sizeof(CShipSize);

	printf("class CShipSizeList: %d Bytes\n", 
			sizeof(CShipSizeList));
	total += sizeof(CShipSizeList);

	printf("class CShipSizeTable: %d Bytes\n", 
			sizeof(CShipSizeTable));
	total += sizeof(CShipSizeTable);

	printf("class CShipScript: %d Bytes\n", 
			sizeof(CShipScript));
	total += sizeof(CShipScript);

	printf("class CShipDesign: %d Bytes\n", 
			sizeof(CShipDesign));
	total += sizeof(CShipDesign);

	printf("class CShipDesignList: %d Bytes\n", 
			sizeof(CShipDesignList));
	total += sizeof(CShipDesignList);

	printf("class CShipToBuild: %d Bytes\n", 
			sizeof(CShipToBuild));
	total += sizeof(CShipToBuild);

	printf("class CShipBuildQ: %d Bytes\n", 
			sizeof(CShipBuildQ));
	total += sizeof(CShipBuildQ);

	printf("class CDockedShip: %d Bytes\n", 
			sizeof(CDockedShip));
	total += sizeof(CDockedShip);

	printf("class CDock: %d Bytes\n", 
			sizeof(CDock));
	total += sizeof(CDock);

	printf("class CDamagedShip: %d Bytes\n", 
			sizeof(CDamagedShip));
	total += sizeof(CDamagedShip);

	printf("class CTech: %d Bytes\n", 
			sizeof(CTech));
	total += sizeof(CTech);

	printf("class CTechTable: %d Bytes\n", 
			sizeof(CTechTable));
	total += sizeof(CTechTable);

	printf("class CTechScript: %d Bytes\n", 
			sizeof(CTechScript));
	total += sizeof(CTechScript);

	printf("class CTurret: %d Bytes\n", 
			sizeof(CTurret));
	total += sizeof(CTurret);

	printf("class CUniverse: %d Bytes\n", 
			sizeof(CUniverse));
	total += sizeof(CUniverse);

	printf("class CVector: %d Bytes\n", 
			sizeof(CVector));
	total += sizeof(CVector);

	printf("class CWeapon: %d Bytes\n", 
			sizeof(CWeapon));
	total += sizeof(CWeapon);
	
	printf("class CPortalUser: %d Bytes\n",
			sizeof(CPortalUser));

//	total += sizeof(CPortalUserTable);
//	printf("class CPortalUserTable\n" sizeof(CPortalUserTable));
	printf("--------------------Library Class Size-----------------\n");
	long long int libtotal = 0;
	printf("class CPage: %d Bytes\n", 
			sizeof(CPage));
	libtotal += sizeof(CPage);
	printf("class CPageStation: %d Bytes\n", 
			sizeof(CPageStation));
	libtotal += sizeof(CPageStation);
	printf("class CConnection: %d Bytes\n", 
			sizeof(CConnection));
	libtotal += sizeof(CConnection);
	printf("class CQuery: %d Bytes\n", 
			sizeof(CQuery));
	libtotal += sizeof(CQuery);
	printf("class CQueryList: %d Bytes\n", 
			sizeof(CQueryList));
	libtotal += sizeof(CQueryList);
	printf("class CHTML: %d Bytes\n", 
			sizeof(CHTML));
	libtotal += sizeof(CHTML);
	printf("class CHTMLStation: %d Bytes\n", 
			sizeof(CHTMLStation));
	libtotal += sizeof(CHTMLStation);
	printf("class CCGIServer: %d Bytes\n", 
			sizeof(CCGIServer));
	libtotal += sizeof(CCGIServer);
	printf("class CFileHTML: %d Bytes\n", 
			sizeof(CFileHTML));
	libtotal += sizeof(CFileHTML);
	printf("class CBase: %d Bytes\n", 
			sizeof(CBase));
	libtotal += sizeof(CBase);
	printf("class CDatabase: %d Bytes\n", 
			sizeof(CDatabase));
	libtotal += sizeof(CDatabase);
	printf("class CApplication: %d Bytes\n", 
			sizeof(CApplication));
	libtotal += sizeof(CApplication);
	printf("class CKeyServer: %d Bytes\n", 
			sizeof(CKeyServer));
	libtotal += sizeof(CKeyServer);
	printf("class CCryptConnection: %d Bytes\n", 
			sizeof(CCryptConnection));
	libtotal += sizeof(CCryptConnection);
	printf("class CMessage: %d Bytes\n", 
			sizeof(CMessage));
	libtotal += sizeof(CMessage);
	printf("class CPacket: %d Bytes\n", 
			sizeof(CPacket));
	libtotal += sizeof(CPacket);
	printf("class CSocket: %d Bytes\n", 
			sizeof(CSocket));
	libtotal += sizeof(CSocket);
	printf("class CFIFO: %d Bytes\n", 
			sizeof(CFIFO));
	libtotal += sizeof(CFIFO);
	printf("class CClient: %d Bytes\n", 
			sizeof(CClient));
	libtotal += sizeof(CClient);
	printf("class CServer: %d Bytes\n", 
			sizeof(CServer));
	libtotal += sizeof(CServer);
	printf("class CLoopServer: %d Bytes\n", 
			sizeof(CLoopServer));
	libtotal += sizeof(CLoopServer);
	printf("class CIPList: %d Bytes\n", 
			sizeof(CIPList));
	libtotal += sizeof(CIPList);
	printf("class CTrigger: %d Bytes\n", 
			sizeof(CTrigger));
	libtotal += sizeof(CTrigger);
	printf("class CTriggerStation: %d Bytes\n", 
			sizeof(CTriggerStation));
	libtotal += sizeof(CTriggerStation);
	printf("class CStore: %d Bytes\n", 
			sizeof(CStore));
	libtotal += sizeof(CStore);
	printf("class CSQL: %d Bytes\n", 
			sizeof(CSQL));
	libtotal += sizeof(CSQL);
	printf("class CStoreCenter: %d Bytes\n", 
			sizeof(CStoreCenter));
	libtotal += sizeof(CStoreCenter);
	printf("class CList: %d Bytes\n", 
			sizeof(CList));
	libtotal += sizeof(CList);
	printf("class CSortedList: %d Bytes\n", 
			sizeof(CSortedList));
	libtotal += sizeof(CSortedList);
	printf("class CString: %d Bytes\n", 
			sizeof(CString));
	libtotal += sizeof(CString);
	printf("class CNode: %d Bytes\n", 
			sizeof(CNode));
	libtotal += sizeof(CNode);
	printf("class CCollection: %d Bytes\n", 
			sizeof(CCollection));
	libtotal += sizeof(CCollection);
	printf("class CStringList: %d Bytes\n", 
			sizeof(CStringList));
	libtotal += sizeof(CStringList);
	printf("class CIntegerList: %d Bytes\n", 
			sizeof(CIntegerList));
	libtotal += sizeof(CIntegerList);
	printf("class CIniFile: %d Bytes\n", 
			sizeof(CIntegerList));
	libtotal += sizeof(CIniFile);
	printf("class CCommandSet: %d Bytes\n", 
			sizeof(CCommandSet));
	libtotal += sizeof(CCommandSet);
	printf("class CMySQL: %d Bytes\n", 
			sizeof(CMySQL));
	libtotal += sizeof(CMySQL);
	printf("class CScript: %d Bytes\n", 
			sizeof(CScript));
	libtotal += sizeof(CScript);
	printf("class CSQL: %d Bytes\n",
			sizeof(CSQL));
	printf("General Total: %d Bytes\n", total);
	printf("Lib Total: %d Bytes\n", libtotal);
	printf("Grand Total: %d Bytes\n", libtotal + total);
	return 0;
}
