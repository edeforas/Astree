// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef ImageQuality_
#define ImageQuality_

#include <vector>
using namespace std;

class ImageQuality
{
public:	
    ImageQuality();

    void init(int iNbangles);
    int nb_angles() const;

    vector<double> vdAngles;
    vector<double> vdDist;
    vector<double> vdVignetting;
    vector<double> vdSpotSize;
    vector<double> vdSpotvsAiry;

    double dFNumber;
    double dAirySize;
    bool isImageInfinite;

private:
    int _iNbAngles;
};

#endif
