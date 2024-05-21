	mNick.clear();
	if( get_court_rank() > CR_NONE ){
		CCluster
			*Cluster = UNIVERSE->get_by_id(mHomeClusterID);
		if( Cluster == NULL )
			mNick.format( "%s the %s (#%d)", (char *)mName, get_court_rank_name(), mGameID );
		else
			mNick.format( "%s the %s of %s (#%d)", (char *)mName, get_court_rank_name(), Cluster->get_name(), mGameID );
	} else {
		mNick.format("%s (#%d)", (char *)mName, mGameID);
	}

