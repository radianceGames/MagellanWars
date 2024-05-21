#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "net.h"

int
main()
{
	CIPList
		IPList;

	if (!IPList.load("/space/space/relay/etc/ip_allow"))
	{
		printf("cannot open");
	}
	return 0;
}
