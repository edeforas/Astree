// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef LightAutofocus_
#define LightAutofocus_

class Light;

class LightAutofocus
{
public:
    LightAutofocus();
    double autofocus(const Light& l);

    void get_center(double& xCenter,double& yCenter) const;

private:
    double compute_spot_size(const Light& l,double z);

    double _xCenter,_yCenter;
};

#endif
