// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "ImageQuality.h"

////////////////////////////////////////////////////////////////////////////////
ImageQuality::ImageQuality()
{
    init(0);
}
////////////////////////////////////////////////////////////////////////////////
void ImageQuality::init(int iNbangles)
{
    _iNbAngles=iNbangles;
    vdAngles.assign(iNbangles,-1.);
    vdDist.assign(iNbangles,-1.);
    vdVignetting.assign(iNbangles,-1.);
    vdSpotSize.assign(iNbangles,-1.);
    vdSpotvsAiry.assign(iNbangles,-1.);

    dFNumber=-1.;
    dAirySize=-1.;
    isImageInfinite=false;
}
////////////////////////////////////////////////////////////////////////////////
int ImageQuality::nb_angles() const
{
    return _iNbAngles;
}
////////////////////////////////////////////////////////////////////////////////
