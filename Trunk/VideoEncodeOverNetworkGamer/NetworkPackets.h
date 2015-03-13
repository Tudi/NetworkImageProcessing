#pragma once

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