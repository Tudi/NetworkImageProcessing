#include "StdAfx.h"
#include "ClientNetwork.h"


ClientNetwork::ClientNetwork(void)
{
    // create WSADATA object
    WSADATA wsaData;

    // socket
    ConnectSocket = INVALID_SOCKET;

    // holds address info for socket to connect to
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) 
	{
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }

	// set address info
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

	
    //resolve server address and port 
	iResult = getaddrinfo( GlobalData.CapturePCIP, GlobalData.CapturePCPort, &hints, &result);

    if( iResult != 0 ) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        exit(1);
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
	{
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET)
		{
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            printf ("The server is down... did not connect");
        }
    }

	// no longer need address info for server
    freeaddrinfo(result);

    // if connection failed
    if (ConnectSocket == INVALID_SOCKET) 
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        exit(1);
    }

	// Set the mode of the socket to be nonblocking
    u_long iMode = 1;
    iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        exit(1);        
    } 
	/**/

	//disable nagle
//    char value = 1;
//    setsockopt( ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );
}


ClientNetwork::~ClientNetwork(void)
{
	closesocket(ConnectSocket);
	WSACleanup();
}

int ClientNetwork::receivePackets(char *recvbuf) 
{
	iResult = recv( ConnectSocket, recvbuf, GlobalData.MaxPacketSize, 0 );
//	char test[10];
//	iResult = recv( ConnectSocket, test, 10, 0 );

	if ( iResult == 0 )
    {
        printf( "Connection closed\n" );
        closesocket( ConnectSocket );
        WSACleanup();
//		exit(1);
        return iResult;
    }
	else if( iResult < 0 )
	{
		int iResult2 = WSAGetLastError();
		if( iResult2 == WSAEWOULDBLOCK )
		{
			return 0;
		}
		printf( "recv failed: %d - %d\n", iResult, iResult2 );
        closesocket( ConnectSocket );
        WSACleanup();
//		exit(1);
        return iResult;
	}

    return iResult;
}
