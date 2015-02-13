#ifndef _IMAGE_TOOLS_H_
#define _IMAGE_TOOLS_H_

//forget small moving objects
void NeglectSmallChanges( MEImageDescRGB32 &In, MEImageDescRGB32 &Out, int FlatLimit = 0, int PCTLimitFromAVG = 0 );
void ErrodeSumSADMap( MEImageDescRGB32 &In, MEImageDescRGB32 &Out, int ErodeRadius = 3 );
//if most parts of the map are moving than there is a chance user is navigating ont the map and not enemy is rigth in front of us
void IsMapMoving( MEImageDescRGB32 &In );
//in case we detected something and want to inspect it later
enum ImageSaveSourceTypes
{
	CHAR_CHAR_DATA_TYPE = 1,
	RGB_CHAR_DATA_TYPE,
	RGB_CHARQUARTER_DATA_TYPE,
};
void SaveImageToFile( MEImageDescRGB32 *In, MEImageDescRGB32 *Mask, char *FileName, bool AutoIncrement = true, ImageSaveSourceTypes InType = CHAR_CHAR_DATA_TYPE );
//Edge detection is not accurate, but might help us differentiate texture movement from object movement
void GenerateEdgeMapRobertCross( MEImageDescRGB32 &In, MEImageDescRGB32 &Out );

#endif