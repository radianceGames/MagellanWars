bool
CPageCouncil::handler()
{
//	system_log("start page handler %s", get_name());

	CPlayer
		*Player = get_player();

	if (!Player) return false; 

	CQueryList 
		Conversion;
	get_conversion(Conversion);

	CString
		Buffer;
	Buffer.format("%s (#%d)", Player->get_name(), Player->get_game_id());
	Conversion.set_value("PLAYER_NICK", (char*)Buffer);

	Buffer.clear();
	CCouncil
		*Council = Player->get_council();
	Buffer.format( "%s (#%d)", Council->get_name(), Council->get_id() );
	Conversion.set_value("COUNCIL_NICK", (char*)Buffer);

//	system_log("end page handler %s", get_name());

	return output("council.html", Conversion);
}
