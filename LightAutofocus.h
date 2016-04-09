// this file is covered by the General Public License version 2 or later
// please see GPL.html for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef LightAutofocus_
#define LightAutofocus_

class Light;

class LightAutofocus
{
public:
	double autofocus(Light& l);

private:
    double compute_spot_size(Light& l,double z);
};

#endif
