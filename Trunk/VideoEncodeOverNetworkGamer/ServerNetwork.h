#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <map>
//#include "NetworkData.h"
using namespace std; 
#pragma comment (lib, "Ws2_32.lib")

//#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "6881" 
#pragma pack(push,1)
struct NetworkPacketHeader
{
	int Version;
	int Width,Stride,Height,PixelByteCount;
	int CompressionStrength;
};
#pragma pack(pop)

class ServerNetwork
{
public:
    ServerNetwork(void);
    ~ServerNetwork(void);

	int HasConnections() { return !sessions.empty(); }

	// send data to all clients
    void sendToAll(char * packets, int totalSize);

	// receive incoming data
//    int receiveData(unsigned int client_id, char * recvbuf);
	
	// accept new connections
    bool acceptNewClient(unsigned int & id);

    // Socket to listen for new connections
    SOCKET ListenSocket;

    // Socket to give to the clients
    SOCKET ClientSocket;

    // for error checking return values
    int iResult;

    // table to keep track of each client's socket
    std::map<unsigned int, SOCKET> sessions; 
};

