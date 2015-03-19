#pragma once
//#pragma comment (lib, "Ws2_32.lib")


class ServerNetwork
{
public:
    ServerNetwork( char *IP, char *Port );
    ~ServerNetwork(void);

	int HasConnections() { return !sessions.empty(); }

	// send data to all clients
    void sendToAll(char * packets, int totalSize);

	// receive incoming data
//    int receiveData(unsigned int client_id, char * recvbuf);
	
	// accept new connections
    bool acceptNewClient();

	void CloseConnections();

    // Socket to listen for new connections
    SOCKET ListenSocket;

    // Socket to give to the clients
//    SOCKET ClientSocket;

    // for error checking return values
    int iResult;

    // table to keep track of each client's socket
    std::map<unsigned int, SOCKET> sessions; 

	int MaxSocketBufferSize;

	ShortTermDataUsageSampler NetworkUsageStat;
};

void StartWaitAcceptNewConnectionsThread( ServerNetwork *Listener );