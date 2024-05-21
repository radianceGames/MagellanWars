#include "planet.h"
#include <libintl.h>

TZone gResourceZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CResource),
	0,
	0,
	NULL,
	"Zone CResource"
};

CResource::CResource()
{
	clear();
}

CResource::CResource(CResource& aResource)
{
	memcpy(mResource, aResource.mResource, sizeof(mResource));
}

CResource::CResource( int aProduction, int aMilitary, int aResearch )
{
	clear();
	mResource[BUILDING_FACTORY] = aProduction;
	mResource[BUILDING_MILITARY_BASE] = aMilitary;
	mResource[BUILDING_RESEARCH_LAB] = aResearch;
}

CResource::~CResource()
{
}

void
CResource::set_all(int aProduction, int aMilitary, int aResearch)
{
	set(RESOURCE_PRODUCTION, aProduction);
	set(RESOURCE_MILITARY, aMilitary);
	set(RESOURCE_RESEARCH, aResearch);
}

bool
CResource::set(EResourceType aResourceType, int aResourceValue)
{
	if (aResourceType < RESOURCE_BEGIN || aResourceType >= RESOURCE_MAX) return false;

	if (aResourceValue > MAX_RESOURCE)
	{
		mResource[aResourceType] = MAX_RESOURCE;
	}
	else if (aResourceValue < 0)
	{
		mResource[aResourceType] = 0;
	}
	else
	{
		mResource[aResourceType] = aResourceValue;
	}

	return true;
}

bool
CResource::change(EResourceType aResourceType, int aResourceValue)
{
	long long int CurrentResource = 0,
		PassedResource = 0;
	if( aResourceType < RESOURCE_BEGIN ||
			aResourceType >= RESOURCE_MAX) return false;

	if (aResourceValue == 0) return true;

	// never allow the resource to be negative
	if (mResource[aResourceType] < 0)
	    mResource[aResourceType] = 0;

	CurrentResource = mResource[aResourceType];
	PassedResource = aResourceValue;


	if (PassedResource > 0)
	{
	    if (aResourceValue + CurrentResource > MAX_RESOURCE)
	    {
	        CurrentResource = MAX_RESOURCE;
	    }
	    else if (PassedResource + CurrentResource < CurrentResource) // if overflow
	    {
	        CurrentResource = MAX_RESOURCE;
	    }
	    else
		{
			CurrentResource += PassedResource;
		}
	}
	else
	{
		if (CurrentResource + PassedResource < 0)
		{
			CurrentResource= 0;
		}
		else
		{
			CurrentResource += PassedResource;
		}
	}
	mResource[aResourceType] = CurrentResource;

	return true;
}

int
CResource::get(EResourceType aResourceType) const
{
	if (aResourceType < RESOURCE_BEGIN || aResourceType >= RESOURCE_MAX) return 0;

	return mResource[aResourceType];
}

CResource&
CResource::operator=(const CResource &aResource)
{
	memcpy(mResource, aResource.mResource, sizeof(mResource));
	return *this;
}

CResource&
CResource::operator+=(const CResource &aResource)
{
	for (int i=RESOURCE_BEGIN ; i<RESOURCE_MAX ; i++)
	{
		change((EResourceType)i, aResource.get((EResourceType)i));
	}

	return *this;
}

int
CResource::total() const
{
	int
		Total = 0;

	for (int i=RESOURCE_BEGIN ; i<RESOURCE_MAX ; i++)
	{
		if (MAX_RESOURCE - Total < mResource[i])
		{
			return MAX_RESOURCE;
		}
		Total += mResource[i];
	}

	return Total;
}

const char*
CResource::debug_info()
{
	static CString
		DebugString;

	DebugString = format("Pro/Fac:%d, Mil/Mil:%d, Res/Lab:%d",
						mResource[RESOURCE_PRODUCTION],
						mResource[RESOURCE_MILITARY],
						mResource[RESOURCE_RESEARCH]);
	return (char*)DebugString;
}

const CString &
CResource::get_query( char *prefix )
{
	static CString Query;

	Query.clear();

	Query.format( ", %sfactory = %d", prefix, mResource[RESOURCE_PRODUCTION] );
	Query.format( ", %smilitary_base = %d", prefix, mResource[RESOURCE_MILITARY] );
	Query.format( ", %sresearch_lab = %d", prefix, mResource[RESOURCE_RESEARCH] );

	return Query;
}

const char *
CResource::get_building_name( int aBuilding )
{
	switch( aBuilding ){
		case BUILDING_FACTORY :
			return GETTEXT("Factory");
		case BUILDING_MILITARY_BASE :
			return GETTEXT("Military Base");
		case BUILDING_RESEARCH_LAB :
			return GETTEXT("Research Lab.");
		default :
			return NULL;
	}
}
