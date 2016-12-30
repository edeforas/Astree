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

private:
    int _iNbAngles;
};

#endif
