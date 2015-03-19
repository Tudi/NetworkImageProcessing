#pragma once

#define USE_NETWORK_BUFFERING

class ClientNetwork
{

public:

    // for error checking function calls in Winsock library
    int iResult;

    // socket for client to connect to server
    SOCKET ConnectSocket;

    // ctor/dtor
    ClientNetwork(char *IP, char *Port);
    ~ClientNetwork(void);
	void CloseConnection();

//	int receivePackets(char *);
	int ReceivePacketNonBlocking( char *, unsigned int );
	int ReplyToSender(char *, int);

	int MaxSocketBufferSize;
};

