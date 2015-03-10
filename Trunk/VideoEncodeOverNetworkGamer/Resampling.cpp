#include "StdAfx.h"

#ifndef MAX
	#define  MAX(a,b)		(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
	#define  MIN(a,b)		(((a) < (b)) ? (a) : (b))
#endif

//note that this is mostly tested when in/out size is dividable by 16. Using Odd numbers might give issues at rounding
void ResampleRGBLiniar4ByteDownsample( unsigned char *psrc, unsigned char *pdst, unsigned int SrcW, unsigned int SrcH, unsigned int DestW, unsigned int DestH, unsigned int SrcStride, unsigned int DestStride )
{
	unsigned int int_conv_y = (unsigned int)( SrcH * FLOAT_PRECISSION / DestH );
	unsigned int int_conv_x = (unsigned int)( SrcW * FLOAT_PRECISSION / DestW );
	unsigned int *src = (unsigned int *)psrc;
	unsigned int *dst = (unsigned int *)pdst;

#ifdef _DEBUG
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) ) <= SrcH );
	assert( ( ( ( int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) ) <= SrcW );
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) * ( (int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) * 3 / 2 ) <= SrcW * SrcH * 3 / 2 );
	assert( DestH <= SrcH );
#endif
	if( SrcStride == 0 )
		SrcStride = SrcW;
	if( DestStride == 0 )
		DestStride = DestW;

	unsigned int stacking_precission_y = 0;
	for( unsigned int y=0;y<DestH;y++)
	{
		unsigned int	stacking_precission_x = 0;

		unsigned int	converted_row_index = stacking_precission_y >> FLOAT_PRECISSION_BITS;
		unsigned int    *tsrc = src + converted_row_index * SrcW;
		stacking_precission_y += int_conv_y;

		unsigned int start = y * DestW;
		unsigned int end = start + DestW;
#ifdef _DEBUG
		assert( converted_row_index <= SrcH );
		assert( dst + end <= dst + DestW * DestH );
#endif

		for( unsigned int x=start;x<end;x++)
		{
			unsigned int converted_col_index = stacking_precission_x >> FLOAT_PRECISSION_BITS;
#ifdef _DEBUG
			assert( converted_col_index <= SrcW );
#endif
			dst[ x ] = tsrc[ converted_col_index ];
			stacking_precission_x += int_conv_x;
		}
	}
}

void ResampleRGBLiniar4to3ByteDownsample( unsigned char *psrc, unsigned char *pdst, unsigned int SrcW, unsigned int SrcH, unsigned int DestW, unsigned int DestH, unsigned int SrcStride, unsigned int DestStride )
{
	unsigned int int_conv_y = (unsigned int)( SrcH * FLOAT_PRECISSION / DestH );
	unsigned int int_conv_x = (unsigned int)( SrcW * FLOAT_PRECISSION / DestW );
	unsigned int *src = (unsigned int *)psrc;
	unsigned char *dst = (unsigned char *)pdst;

#ifdef _DEBUG
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) ) <= SrcH );
	assert( ( ( ( int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) ) <= SrcW );
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) * ( (int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) * 3 / 2 ) <= SrcW * SrcH * 3 / 2 );
	assert( DestH <= SrcH );
#endif
	if( SrcStride == 0 )
		SrcStride = SrcW;
	if( DestStride == 0 )
		DestStride = DestW;

	unsigned int stacking_precission_y = 0;
	for( unsigned int y=0;y<DestH;y++)
	{
		unsigned int	stacking_precission_x = 0;

		unsigned int	converted_row_index = stacking_precission_y >> FLOAT_PRECISSION_BITS;
		unsigned int    *tsrc = src + converted_row_index * SrcW;
		stacking_precission_y += int_conv_y;

		unsigned int start = y * DestW;
		unsigned int end = start + DestW;
#ifdef _DEBUG
		assert( converted_row_index <= SrcH );
		assert( dst + end <= dst + DestW * DestH );
#endif

		for( unsigned int x=start;x<end;x++)
		{
			unsigned int converted_col_index = stacking_precission_x >> FLOAT_PRECISSION_BITS;
#ifdef _DEBUG
			assert( converted_col_index <= SrcW );
#endif
			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ];
			stacking_precission_x += int_conv_x;
		}
	}
}

void ResampleRGBLiniar4to5BitsDownsample( unsigned char *psrc, unsigned char *pdst, unsigned int SrcW, unsigned int SrcH, unsigned int DestW, unsigned int DestH, unsigned int SrcStride, unsigned int DestStride )
{
	unsigned int int_conv_y = (unsigned int)( SrcH * FLOAT_PRECISSION / DestH );
	unsigned int int_conv_x = (unsigned int)( SrcW * FLOAT_PRECISSION / DestW );
	unsigned int *src = (unsigned int *)psrc;
	unsigned char *dst = (unsigned char *)pdst;

#ifdef _DEBUG
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) ) <= SrcH );
	assert( ( ( ( int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) ) <= SrcW );
	assert( ( ( ( int_conv_y * DestH ) >> FLOAT_PRECISSION_BITS ) * ( (int_conv_x * DestW ) >> FLOAT_PRECISSION_BITS ) * 3 / 2 ) <= SrcW * SrcH * 3 / 2 );
#endif
	if( SrcStride == 0 )
		SrcStride = SrcW;
	if( DestStride == 0 )
		DestStride = DestW;

	unsigned int stacking_precission_y = 0;
	for( unsigned int y=0;y<DestH;y++)
	{
		unsigned int	stacking_precission_x = 0;

		unsigned int	converted_row_index = stacking_precission_y >> FLOAT_PRECISSION_BITS;
		unsigned int    *tsrc = src + converted_row_index * SrcW;
		stacking_precission_y += int_conv_y;

		unsigned int start = y * DestW;
		unsigned int end = start + DestW;
#ifdef _DEBUG
		assert( converted_row_index <= SrcH );
		assert( dst + end <= dst + DestW * DestH );
#endif

		for( unsigned int x=start;x<end;x++)
		{
			unsigned int converted_col_index = stacking_precission_x >> FLOAT_PRECISSION_BITS;
#ifdef _DEBUG
			assert( converted_col_index <= SrcW );
#endif
//			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ];
//			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ] & 0x00FEFEFE;
//			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ] & 0x00FCFCFC;
//			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ] & 0x00F8F8F8;
//			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ] & 0x00F0F0F0;
			*(unsigned int*)&dst[ x * 3 ] = tsrc[ converted_col_index ] & GlobalData.ColorFilterMask;
			stacking_precission_x += int_conv_x;
		}
	}
}
