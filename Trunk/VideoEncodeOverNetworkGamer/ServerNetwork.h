#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <map>
#pragma comment (lib, "Ws2_32.lib")

#pragma pack(push,1)
struct NetworkPacketHeader
{
	int Version;
	int Width,Stride,Height,PixelByteCount;
	int CompressionStrength;
	int CompressedSize;
	int	PacketSize;
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
    bool acceptNewClient();

    // Socket to listen for new connections
    SOCKET ListenSocket;

    // Socket to give to the clients
//    SOCKET ClientSocket;

    // for error checking return values
    int iResult;

    // table to keep track of each client's socket
    std::map<unsigned int, SOCKET> sessions; 

	int MaxSocketBufferSize;
};

