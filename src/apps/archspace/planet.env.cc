static char *mGasName[] = 
{
	"Hydrogen",
	"Chlorine",
	"Carbon Dioxide",
	"Oxygen",
	"Nitrogen",
	"Methane",
	"Water"
};

static char *mResourceDescription[] = 
{
	"Ultra Poor",
	"Poor",
	"Normal",
	"Rich",
	"Ultra Rich"
};

static char *mSizeDescription[] = {
	"Tiny",
	"Small",
	"Medium",
	"Large",
	"Huge"
};

CPlanetEnvironment::CPlanetEnvironment()
{
	memset(mAtmosphere, 0, sizeof(mAtmosphere));
	mTemperature = 300;
	mSize = 2;
	mResource = 2;
	mGravity = 1.0;
}

CPlanetEnvironment::~CPlanetEnvironment()
{
}

const char*
CPlanetEnvironment::get_size_description()
{
	return mSizeDescription[mSize];
}

const char*
CPlanetEnvironment::get_resource_description()
{
	return mResourceDescription[mResource];
}

bool
CPlanetEnvironment::set_atmosphere(EGasType aGasType, int aGas)
{
	if(aGasType<GAS_BEGIN || aGasType>=GAS_MAX || 
			aGas<0 || aGas>5)
		return false;

	mChanged += C_ATMOSPHERE;

	mAtmosphere[aGasType] = aGas;
	return true;
}

bool 
CPlanetEnvironment::set_temperature(int aTemperature)
{
	if(aTemperature<100 || aTemperature>500) 
		return false;

	mChanged += C_TEMPERATURE;

	mTemperature = aTemperature;
	return true;
}

bool 
CPlanetEnvironment::set_gravity(double aGravity)
{
	if(aGravity<0.2 || aGravity>2.5) 
		return false;

	mChanged += C_GRAVITY;

	mGravity = aGravity;
	return true;
}

bool 
CPlanetEnvironment::set_size(int aSize)
{
	if(aSize<0 || aSize>4) 
		return false;

	mChanged += C_SIZE;

	mSize = aSize;
	return true;
}

bool
CPlanetEnvironment::set_resource(int aResource)
{
	if(aResource<0 || aResource>4) 
		return false;

	mChanged += C_RESOURCE;

	mResource = aResource;
	return true;
}

bool 
CPlanetEnvironment::change_atmosphere(EGasType aDecreasing, 
		EGasType aIncreasing )
{
	int Changed = 0;

	if(aDecreasing >= GAS_BEGIN && aDecreasing < GAS_MAX){
		if( mAtmosphere[aDecreasing] > 0 )
			mAtmosphere[aDecreasing]--;
		Changed = 1;
	}
	if(aIncreasing >= GAS_BEGIN && aIncreasing < GAS_MAX){
		if( mAtmosphere[aIncreasing] > 0 )
			mAtmosphere[aIncreasing]++;
		Changed = 1;
	}

	if( Changed ) mChanged += C_ATMOSPHERE;

	return true;
}

int 
CPlanetEnvironment::change_temperature(int aTemperature)
{
	mTemperature += aTemperature;

	if(mTemperature<100) mTemperature = 100;
	if(mTemperature>500) mTemperature = 500;

	mChanged += C_TEMPERATURE;

	return mTemperature;
}

double 
CPlanetEnvironment::change_gravity(double aGravity)
{
	mGravity += aGravity;

	if(mGravity<0.2) mGravity = 0.2;
	if(mGravity>2.5) mGravity = 2.5;

	mChanged += C_GRAVITY;

	return mGravity;
}

int 
CPlanetEnvironment::change_size(int aSize)
{
	mSize += aSize;

	if(mSize<0) mSize = 0;
	if(mSize>4) mSize = 4;

	mChanged += C_SIZE;

	return mSize;
}

int 
CPlanetEnvironment::change_resource(int aResource)
{
	mResource += aResource;

	if(mResource<0) 
		mResource = 0;
	if(mResource>4) 
		mResource = 4;

	mChanged += C_RESOURCE;

	return mResource;
}

const char *
CPlanetEnvironment::get_atmosphere_string()
{
	static char AtmosphereString[GAS_MAX+1];

	for( int i = GAS_BEGIN; i < GAS_MAX; i++ ){
		AtmosphereString[i] = mAtmosphere[i] + '0';
	}

	return AtmosphereString;
}

bool 
CPlanetEnvironment::set_atmosphere_string( const char *aString )
{
	bool ReturnValue = true;
	int Sum = 0;

	for( int i = GAS_BEGIN; i < GAS_MAX; i++ ){
		mAtmosphere[i] = aString[i] - '0';
		if( mAtmosphere[i] < 0 || mAtmosphere[i] > 5 ){
			mAtmosphere[i] = 0;
			ReturnValue = false;
		}
		Sum += mAtmosphere[i];
	}

	if( Sum != 5 ) ReturnValue = false;

	return ReturnValue;
}


const CString &
CPlanetEnvironment::get_query( char *prefix )
{
	static CString Query;

	Query.clear();

	if (mChanged.is_empty()) return Query;

	if (mChanged.has(C_ATMOSPHERE))
		Query.format(", %satmosphere = '%s'", prefix, 
									get_atmosphere_string());

	if (mChanged.has(C_TEMPERATURE))
		Query.format(", %stemperature = %d", prefix, get_temperature());

	if (mChanged.has(C_SIZE))
		Query.format(", %ssize = %d", prefix, get_size());

	if (mChanged.has(C_RESOURCE))
		Query.format(", %sresource = %d", prefix, get_resource());

	if (mChanged.has(C_GRAVITY))
		Query.format(", %sgravity = %f", prefix, get_gravity());

	return Query;
}

int
CPlanetEnvironment::atmosphere_name_to_index( char *aName )
{
	for( int i = GAS_BEGIN; i < GAS_MAX; i++ )
		if( strcasecmp( aName, mGasName[i] ) == 0 ) return i;

	return -1;
}

int
CPlanetEnvironment::calc_environment_control( CPlayer *aOwner )
{
	const CPlanetEnvironment
		&HomeStandard = aOwner->get_home_environment();

	int 
		Control = 0;

	if( aOwner->has_ability( ABILITY_NO_BREATH ) == false ){
		int 
			GasDifference = 0;
		for( int i = GAS_BEGIN; i < GAS_MAX; i++ ){
			int
				Tmp = get_atmosphere((EGasType)i) 
							- HomeStandard.get_atmosphere((EGasType)i);
			Tmp = -abs(Tmp);
			GasDifference += Tmp;
		}
		GasDifference /= 2;
		Control += GasDifference;
	}
	
	int 
		TemperatureDifference = get_temperature() 
									- HomeStandard.get_temperature();
	TemperatureDifference = -abs(TemperatureDifference);
	Control += TemperatureDifference/50;

	double
		GravityDifference = get_gravity() - HomeStandard.get_gravity();
	GravityDifference = -fabs(GravityDifference);
	Control += (int)(GravityDifference/0.2);

	return Control;
}
