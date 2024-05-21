#include "common.h"
#include "util.h"
#include "project.h"
#include "define.h"
#include "archspace.h"
#include "game.h"

TZone gPurchasedProjectZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPurchasedProject),
	0,
	0,
	NULL,
	"Zone CPurchasedProject"
};

CPurchasedProject::CPurchasedProject()
{
	mProject = NULL;
	mOwner = 0;
}

CPurchasedProject::CPurchasedProject(int aOwner, int aProjectID)
{
	CProject
		*Project = PROJECT_TABLE->get_by_id(aProjectID);
	
	mOwner = aOwner;
	mProject = Project;
}

CPurchasedProject::CPurchasedProject( CProject *aProject )
{
	mProject = aProject;
	mOwner = 0;
}

CPurchasedProject::~CPurchasedProject()
{
	mProject = NULL;
}

bool
CPurchasedProject::initialize(int aOwner, int aProjectID, int aType)
{
	CProject
		*Project = PROJECT_TABLE->get_by_id(aProjectID);
/*
	// start telecard 2001/03/14
	if( Project == NULL )
	{
		Project = SECRET_PROJECT_TABLE->get_by_id( aProjectID );
	}
	// end telecard 2001/03/14
	//start telecard 2001/04/02
	if( Project == NULL )
	{
		Project = ENDING_PROJECT_TABLE->get_by_id( aProjectID );
	}
	//end telecard 2001/04/02
*/
	mOwner = aOwner;
	mProject = Project;
	if(Project == NULL) return false;
	Project->set_type(aType);

	return true;
}

CString &
CPurchasedProject::query()
{
	static CString 
		Query;

	Query.clear();

	switch( type() ){
		case QUERY_INSERT :
			Query.format("INSERT INTO project (owner, project_id, type)"
								" VALUES (%d, %d, %d)",
						mOwner, mProject->get_id(), mProject->get_type());
			break;
		case QUERY_UPDATE :
			break;
		case QUERY_DELETE :
			Query.format("DELETE FROM project WHERE owner = %d and project_id = %d",
						mOwner, mProject->get_id());
			break;
	}

	mStoreFlag.clear();

	return Query;
}

const char*
CPurchasedProject::html()
{
	static CString
		Buf;

	Buf.clear();

	Buf = "<TABLE BORDER = 0 CELLSPACING = 0>";

	Buf.format( "<TR><TD>ID = %d</TD>\n", mProject->get_id() );
	Buf.format( "<TD COLSPAN = 2>Name = %s</TD></TR>\n", 
			mProject->get_name());
	Buf.format( "<TR><TD COLSPAN = 3 WIDTH = 600>%s</TD></TR>\n", 
				mProject->get_description());

	Buf += "</TABLE>";

	return (char*)Buf;
}

CStoreCenter&
operator<<(CStoreCenter& aStoreCenter, CPurchasedProject &aPurchasedProject)
{
	aStoreCenter.store( aPurchasedProject );
	return aStoreCenter;
}

