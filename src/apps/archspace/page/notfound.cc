#include "../pages.h"
#include "../archspace.h"
#include "../game.h"
const char*
CPageNotFound::get_name()
{
	return "not_found.as";
}

bool
CPageNotFound::handle(CConnection &aConnection)
{
	CQueryList
		mCookies;
    // TODO: make it load from a HTML/XML file
    CString OutputPage;
    OutputPage.clear();
    OutputPage+="<html>\n";
    OutputPage+="<head>\n";
    OutputPage+="<title>*********ARCHSPACE************</title>\n";
    OutputPage+="<meta http-equiv=\"Content-Type\" content=\"text/html; $CHAR_SET\">\n";
    OutputPage+="<meta http-equiv=\"Cache-Control\" content=\"no-cache\"/>\n";
    OutputPage+="<meta http-equiv=\"Expires\" content=\"0\"/>\n";
    OutputPage+="<meta http-equiv=\"Pragma\" content=\"no-cache\"/>\n";
    OutputPage+="<SCRIPT TYPE=\"text/javascript\" LANGUAGE=javascript SRC=/jsLib.js></SCRIPT>\n";
    OutputPage+="<LINK REL=\"stylesheet\" HREF=\"/archspace.css\">\n";
    OutputPage+="</head>\n";

    OutputPage+="<body bgcolor=\"#000000\" style=\"margin:0;\" marginwidth=\"0\" marginheight=\"0\" link=\"#999999\" vlink=\"#999999\" alink=\"#999999\" onLoad=\"\">\n";
    OutputPage+="<TABLE WIDTH=\"610\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";
    OutputPage+="  <TR> \n";
    OutputPage+="    <TD> \n";
    OutputPage+="      <TABLE  WIDTH=590 BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" HEIGHT=\"100\" BGCOLOR=\"#000000\" ALIGN=\"center\">\n";
    OutputPage+="        <TR ALIGN=\"center\" VALIGN=\"middle\"> \n";
    OutputPage+="          <TD ALIGN\"center\" HEIGHT=\"18\">\n";
    OutputPage.format("<IMG SRC=\"%s/image/as_game/message/message.gif\" WIDTH=\"175\" HEIGHT=\"131\">",(char *)CGame::mImageServerURL);
    OutputPage+="          </TD>\n";
    OutputPage+="        </TR>\n";
    OutputPage+="      </TABLE>\n";
    OutputPage+="    </TD>\n";
    OutputPage+="  </TR>\n";
    OutputPage+="  <TR> \n";
    OutputPage+="    <TD ALIGN=\"center\">&nbsp;</TD>\n";
    OutputPage+="  </TR>\n";
    OutputPage+="  <TR> \n";
    OutputPage.format("    <TD ALIGN=\"center\" CLASS=\"maintext\">%s does not exist</TD>\n",aConnection.get_uri());
    OutputPage+="  </TR>\n";
    OutputPage+="  <TR> \n";
    OutputPage+="    <TD ALIGN=\"center\">&nbsp;</TD>\n";
    OutputPage+="  </TR>                               \n";
    OutputPage+="  <TR> \n";
    OutputPage+="    <TD>&nbsp;</TD>\n";
    OutputPage+="  </TR>\n";
    OutputPage+="</TABLE>\n";
    OutputPage+="<P>&nbsp;</P>\n";
    OutputPage+="</body>\n";
    OutputPage+="</html>\n";
    mOutput.append((char *)OutputPage);
    
    return true;
}
