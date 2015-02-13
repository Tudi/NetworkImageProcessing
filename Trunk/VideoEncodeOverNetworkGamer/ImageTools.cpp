#include "StdAfx.h"

void NeglectSmallChanges( MEImageDescRGB32 &In, MEImageDescRGB32 &Out, int FlatLimit, int PCTLimitFromAVG )
{
	//get sum of change strengths
	if( PCTLimitFromAVG > 0 )
	{
		int SumOfSAD = 0;
		int NrOfSAD = 0;
		for( unsigned int y = In.StartY; y < In.EndY; y +=1 )
			for( unsigned int x = In.StartX; x < In.EndX; x += 1 )
			{
				int PixelValue = In.Data[ y * In.Stride + x ];
				if( PixelValue != 0 )
				{
					NrOfSAD++;
					SumOfSAD += PixelValue;
				}
			}
		if( NrOfSAD > 0 )
		{
			int AVGSAD = SumOfSAD / NrOfSAD;
			int PCTSADLimit = AVGSAD * PCTLimitFromAVG / 100;
			for( unsigned int y = In.StartY; y < In.EndY; y +=1 )
				for( unsigned int x = In.StartX; x < In.EndX; x += 1 )
				{
					int PixelValue = In.Data[ y * In.Stride + x ];
					if( PixelValue < PCTSADLimit || PixelValue < FlatLimit )
						In.Data[ y * In.Stride + x ] = 0;
				}
		}
	}
	else
	{
		for( unsigned int y = In.StartY; y < In.EndY; y +=1 )
			for( unsigned int x = In.StartX; x < In.EndX; x += 1 )
			{
				int PixelValue = In.Data[ y * In.Stride + x ];
				if( PixelValue < FlatLimit )
					In.Data[ y * In.Stride + x ] = 0;
			}
	}
}

//forget small moving objects
void ErrodeSumSADMap( MEImageDescRGB32 &In, MEImageDescRGB32 &Out, int ErodeRadius )
{
	if( In.ErodeLimit == 0 )
		In.ErodeLimit = ( 2 * ErodeRadius + 1 ) * ( 2 * ErodeRadius + 1 ) / 2;

	memset( Out.Data, 0, Out.Stride * Out.EndY );

	for( unsigned int y = In.StartY; y < In.EndY; y +=1 )
		for( unsigned int x = In.StartX; x < In.EndX; x += 1 )
		{
			//check the 4x4 map to see if majority has values
			int ValueCount = 0;
			int ValueSum = 0;
			for( int y1=-ErodeRadius;y1<=ErodeRadius;y1++)
				for( int x1=-ErodeRadius;x1<=ErodeRadius;x1++)
				{
					int PixelValue = In.Data[ ( y + y1 ) * In.Stride + x + x1 ];
					if( PixelValue != 0 )
					{
						ValueCount++;
						ValueSum += PixelValue;
					}
				}
			//majority of values changed in this block
			if( ValueCount >= In.ErodeLimit )
			{
//				Out.Data[ y * In.Stride + x ] = ValueSum / ValueCount;
				Out.Data[ y * In.Stride + x ] = 127;
			}
		}
}

//in case we detected something and want to inspect it later
static int ImageFileAutoIncrement = 0;
void SaveImageToFile( MEImageDescRGB32 *In, MEImageDescRGB32 *Mask, char *FileName, bool AutoIncrement, ImageSaveSourceTypes InType )
{
	//gen file name
	char MyFileName[500];
	if( AutoIncrement == true )
	{
		BOOL FileExists;
		do {
			sprintf_s( MyFileName, 500, "%s_%04d.bmp", FileName, ImageFileAutoIncrement );
			FileExists = PathFileExists( MyFileName );
			ImageFileAutoIncrement++;
		}while( FileExists == TRUE );
	}
	else
		sprintf_s( MyFileName, 500, "%s.bmp", FileName );

	//create image
	CImage Img;
	int Height = In->EndY - In->StartY;
	float MaskToImageScalerX;
	float MaskToImageScalerY;
	if( Mask )
	{
//		MaskToImageScalerX = (float)(Mask->EndX - Mask->StartX) / (float)(In->EndX - In->StartX);
//		MaskToImageScalerY = (float)(Mask->EndY - Mask->StartY) / (float)(In->EndY - In->StartY);
		MaskToImageScalerX = (float)(Mask->EndX) / (float)(In->EndX);
		MaskToImageScalerY = (float)(Mask->EndY) / (float)(In->EndY);
	}
	Img.Create( In->EndX - In->StartX, Height, 32 );
	for( unsigned int y = In->StartY; y < In->EndY; y +=1 )
		for( unsigned int x = In->StartX; x < In->EndX; x += 1 )
		{
			if( Mask )
			{
				if( MaskToImageScalerX != 1.0f || MaskToImageScalerY != 1.0f )
				{
//					int mx = (int)( Mask->StartX + ( x - In->StartX ) * MaskToImageScalerX );
//					int my = (int)( Mask->StartY + ( y - In->StartY ) * MaskToImageScalerY );
					int mx = (int)( ( x - In->StartX ) * MaskToImageScalerX );
					int my = (int)( ( y - In->StartY ) * MaskToImageScalerY );
					if( Mask->Data[ my * Mask->Stride + mx ] == 0 )
						continue;
				}
				else if( Mask->Data[ y * Mask->Stride + x ] == 0 )
					continue;
			}

			if( InType == RGB_CHAR_DATA_TYPE )
				Img.SetPixel( x - In->StartX, Height - 1 - ( y - In->StartY ), RGB( In->Data[ y * In->Stride + x * In->PixelByteCount + 2 ], In->Data[ y * In->Stride + x * In->PixelByteCount + 1 ], In->Data[ y * In->Stride + x * In->PixelByteCount + 0 ] ) );
			else
				Img.SetPixel( x - In->StartX, y - In->StartY, RGB( In->Data[ y * In->Stride + x ], In->Data[ y * In->Stride + x ], In->Data[ y * In->Stride + x ] ) );
		}
	Img.Save( MyFileName );
}

int GET_RGBSUM_FROM_ADDR( unsigned char *adr ) 
{
	return ( adr[0] + adr[1] + adr[2] );
}

//maybe this helps us eliminate textures
//sobel would be : abs((p[1]+2*p[2]+p[3])-(p[7]+2*p[8]+p[9]))+abs((p[3]+2*p[6]+p[9])-(p[1]+2*p[4]+p[7]))
void GenerateEdgeMapRobertCross( MEImageDescRGB32 &In, MEImageDescRGB32 &Out )
{
	if( In.PixelByteCount == 1 )
	{
		for( unsigned int y = In.StartY + 1; y < In.EndY; y +=1 )
			for( unsigned int x = In.StartX + 1; x < In.EndX; x += 1 )
			{
				int p[4];
				p[0] = In.Data[ ( y - 1 ) * In.Stride + x - 1 ];
				p[1] = In.Data[ ( y - 1 ) * In.Stride + x - 0 ];
				p[2] = In.Data[ ( y - 0 ) * In.Stride + x - 1 ];
				p[3] = In.Data[ ( y - 0 ) * In.Stride + x - 0 ];
				int Edge = abs( p[0] - p[3] ) + abs( p[1] - p[2] );
				Out.Data[ y * Out.Stride + x ] = Edge / 2;
			}
	}
	else if( In.PixelByteCount == 4 )
	{
		for( unsigned int y = In.StartY + 1; y < In.EndY; y +=1 )
			for( unsigned int x = In.StartX + 1; x < In.EndX; x += 1 )
			{
				int p[4];
				p[0] = GET_RGBSUM_FROM_ADDR( &In.Data[ ( y - 1 ) * In.Stride + ( x - 1 ) * 4 ] );
				p[1] = GET_RGBSUM_FROM_ADDR( &In.Data[ ( y - 1 ) * In.Stride + ( x - 0 ) * 4 ] );
				p[2] = GET_RGBSUM_FROM_ADDR( &In.Data[ ( y - 0 ) * In.Stride + ( x - 1 ) * 4 ] );
				p[3] = GET_RGBSUM_FROM_ADDR( &In.Data[ ( y - 0 ) * In.Stride + ( x - 0 ) * 4 ] );
				int Edge = abs( p[0] - p[3] ) + abs( p[1] - p[2] );
				Out.Data[ y * Out.Stride + x ] = Edge / 2;
			}
	}
}