#include "net.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

CClient::CClient(): CSocket()
{
}

CClient::~CClient()
{
}

int
CClient::open(const char *aServerName, int aPort)
{
	struct sockaddr_in 
		Server;
	struct in_addr 
		Address;
	struct hostent 
		*HostEntry;
	
	memset(&Server, 0, sizeof(struct sockaddr_in));

	Server.sin_family = AF_INET;
	Server.sin_port = htons(aPort);

	if ((Address.s_addr = inet_addr(aServerName)) == -1)
	{
		HostEntry = gethostbyname(aServerName);
		if (HostEntry == NULL)
		{
			system_log("Could not get server ip(%s)", aServerName);
			return -1;
		}
		memcpy((char*)&Address.s_addr, 
				HostEntry->h_addr, HostEntry->h_length);
	}

	Server.sin_addr.s_addr = Address.s_addr;

	int 
		Socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (Socket < 0)
	{
		system_log("Could not create TCP socket");
		return -1;
	}

	if (pth_connect(Socket, (struct sockaddr*)&Server, sizeof(Server)) < 0)
	{
		system_log("Could not connect to server %s %d", aServerName, 
				aPort);
		return -1;
	}

	set(Socket);
	return Socket;
}
