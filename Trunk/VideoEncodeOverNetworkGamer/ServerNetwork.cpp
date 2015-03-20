#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>

ServerNetwork::ServerNetwork(char *IP, char *Port)
{
	// create WSADATA object
    WSADATA wsaData;

	MaxSocketBufferSize = 0;

    // our sockets for the server
    ListenSocket = INVALID_SOCKET;
//    ClientSocket = INVALID_SOCKET;

	// address info for the server to listen to
    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) 
	{
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }

    // set address information
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
    hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, Port, &hints, &result);

    if ( iResult != 0 ) 
	{
        printf("getaddrinfo failed with error: %d\n", iResult);
		CloseConnections();
		return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET) 
	{
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
		CloseConnections();
		return;
    }

	MaxSocketBufferSize = 1 * 65535;
	if( setsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&MaxSocketBufferSize, sizeof(MaxSocketBufferSize) ) == -1 ) 
		printf( "Error setting socket opts: %s\n", WSAGetLastError() );

	int optlen = sizeof(MaxSocketBufferSize);
	iResult = getsockopt(ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&MaxSocketBufferSize, &optlen );

//	timeval tv;
//	tv.tv_sec = 1; 
//	setsockopt(ListenSocket, SOL_SOCKET, SO_RCVTIMEO,(const char *)&tv,sizeof(tv));
//	tv.tv_sec = 10; 
//	setsockopt(ListenSocket, SOL_SOCKET, SO_SNDTIMEO,(const char *)&tv,sizeof(tv));

    // Set the mode of the socket to be nonblocking
/*    u_long iMode = 1;
    iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);
    if (iResult == SOCKET_ERROR) 
	{
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        exit(1);
    }/**/

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) 
	{
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
		CloseConnections();
		return;
    }

    // no longer need address information
    freeaddrinfo(result);

    // start listening for new clients attempting to connect
    iResult = listen(ListenSocket, SOMAXCONN);

    if (iResult == SOCKET_ERROR) 
	{
        printf("listen failed with error: %d\n", WSAGetLastError());
		CloseConnections();
    }
}

ServerNetwork::~ServerNetwork(void)
{
	CloseConnections();
}

void ServerNetwork::CloseConnections()
{
	std::map<unsigned int, SOCKET>::iterator iter;
    for (iter = sessions.begin(); iter != sessions.end(); iter++)
        closesocket( iter->second );
	sessions.clear();
	closesocket( ListenSocket );
	ListenSocket = INVALID_SOCKET;
    if (iResult == SOCKET_ERROR) 
        printf("closesocket function failed with error %d\n", WSAGetLastError());
	WSACleanup();
}

// accept new connections
static int ClientNr = 0;
bool ServerNetwork::acceptNewClient()
{
    // if client waiting, accept the connection and save the socket
    SOCKET ClientSocket = accept( ListenSocket, NULL, NULL );

    if (ClientSocket != INVALID_SOCKET) 
    {
        //disable nagle on the client's socket
//        char value = 1;
//        setsockopt( ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );

        // insert new client into session id table
		sessions.insert( std::pair<unsigned int, SOCKET>( ClientNr, ClientSocket) );
		ClientNr++;

        return true;
    }

	CloseConnections();

    return false;
}

// send data to all clients
void ServerNetwork::sendToAll(char *packets, int totalSize)
{
    SOCKET currentSocket;
    std::map<unsigned int, SOCKET>::iterator iter;
    int iSendResult;

    for (iter = sessions.begin(); iter != sessions.end(); iter++)
    {
        currentSocket = iter->second;
        iSendResult = send( currentSocket, packets, totalSize, 0 );

		NetworkUsageStat.AddSample( totalSize );

//		for( int i = 0; i < 60; i++ )
//			printf( "0x%02X ", (unsigned char)( packets[i] ) );

        if (iSendResult == SOCKET_ERROR) 
        {
			printf("Client %d. Send failed with error: %d\n", iter->first, WSAGetLastError() );
            closesocket( currentSocket );
			iter->second = 0;
			// !!! presuming there is only 1 client !!
			sessions.erase( iter ); 
			return;
        }
/*
		int ReplyFromClient[4];
        iSendResult = recv( currentSocket, (char*)ReplyFromClient, sizeof( ReplyFromClient ), 0 );
		if( GlobalData.ShowStatistics == 2 )
			printf( "Statistics : Encoder avg render speed %d\n", ReplyFromClient[0] );
			*/
    }
}

// receive incoming data
/*
int ServerNetwork::receiveData(unsigned int client_id, char * recvbuf)
{
    if( sessions.find(client_id) != sessions.end() )
    {
        SOCKET currentSocket = sessions[client_id];
        iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);

        if (iResult == 0)
        {
            printf("Connection closed\n");
            closesocket(currentSocket);
        }

        return iResult;
    }

    return 0;
}
*/

void WaitAcceptNewConnectionsThread( void *arg )
{
	GlobalData.ThreadsAliveCount++;
	ServerNetwork *Listener = (ServerNetwork *)arg;
	while( GlobalData.ThreadIsRunning == 1 && Listener->ListenSocket != INVALID_SOCKET )
		Listener->acceptNewClient();
	GlobalData.ThreadsAliveCount--;
}

void StartWaitAcceptNewConnectionsThread( ServerNetwork *Listener )
{
	_beginthread( WaitAcceptNewConnectionsThread, 0, (void*)Listener );
}