# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <sstream>
#include <fstream>
#include <string>
#include <cassert>

using namespace std

#include "DeviceIoZmx.h"
#include "OpticalDevice.h"

################################################/
# utf16/utf8 utilies ( partially from stackoverflow)

#############################################/
class TextFileReader #handle utf8 or utf16 encoding
public:
	TextFileReader()
		_bIsUtf16=False


	bool open(string sFile)
		_bIsUtf16 = is_utf16(sFile)

		if _bIsUtf16:
			wistr.open(sFile,ios.binary)

			wchar_t bom
			wistr >> bom

			return not (not wistr)

		else:
			istr.open(sFile)
			return not (not istr)


	
	bool eof()
		if _bIsUtf16:
			return wistr.eof()
		else:
			return istr.eof()


	bool getline(string& s)
		bool bOk
		if _bIsUtf16:
			wstring ws
			std.getline(wistr, ws)
			s = to_string(ws)

			bOk= not (not wistr)

		else:
			std.getline(istr, s)
			bOk= not (not istr)


		iPosStart = s.find_first_not_of(" \n\r\t")
		iPosEnd = s.find_last_not_of(" \n\r\t")
		if iPosStart != string.npos:
			s = s.substr(iPosStart, iPosEnd - iPosStart + 1)

		return bOk


	bool is_utf16(string sFile)
		ifstream ifs(sFile, ios.binary)
		while (not ifs.eof())
			unsigned char c
			ifs >> c
			if c == 0:
				return True

		return False


	std.string to_string( std.wstring& wstr)
		#raw way to convert from wstring...
		string str
		for (i = 0; i < wstr.size(); i++)
			w = wstr[i]
			if  (w<127) and (w>0) :
				str.push_back((char)w)


		return str


private:
	bool _bIsUtf16
	ifstream istr
	wifstream wistr


########################################
def import(self, sFile):
	TextFileReader tfr
	if not tfr.open(sFile):
		return nullptr

    OpticalDevice* pOD=new OpticalDevice()
    pOD.set_relative_convention(True)

	bMustUseColoredLight = False
    double dDimensionFactor=1.
    int iSurface=0
    bool bSurfPending=False
    string sType
    double dCurvature=0.
    double dTick=0.
    double dDiameter=0.
    double dConic=0.
    double dInnerDiameter=0.
    double dApertureDiameter=-1.
	double dHalfFOV=0
    string sComment
    string sNote="Imported from ZMX file: "+ sFile+"\nWarning, may be inaccuratenot \n\n"
    string sGlassCatalog

    while(not tfr.eof())
		string sLine
		tfr.getline(sLine)

        #trim first space if any
        size_t iPos=sLine.find_first_not_of(" ")
        if iPos!=string.npos:
            sLine=sLine.substr(iPos)

        if sLine.size()<4:
            continue; # nothing to parse

        string sKey=sLine.substr(0,4)

        string sVal=sLine.substr(4); # can be empty

        #trim first space if any
        size_t iPosStart=sVal.find_first_not_of(" \n\r")
		if iPosStart !=string.npos:
            sVal=sVal.substr(iPosStart)
			
        if (sKey=="SURF") or (sKey=="BLNK"):
            if bSurfPending:
                pOD.insert_surface(iSurface)
                pOD.set_comment(iSurface,sComment)
                pOD.set_type(iSurface,sType)
                pOD.set(iSurface,THICK,dDimensionFactor*dTick)
                pOD.set(iSurface,CURVATURE,dCurvature/dDimensionFactor)
                pOD.set(iSurface,DIAMETER,dDiameter*dDimensionFactor)
                pOD.set(iSurface,CONIC,dConic)
                pOD.set(iSurface,INNER_DIAMETER,dInnerDiameter*dDimensionFactor)
                bSurfPending=False


            if(sKey=="SURF") #new surf
                iSurface=-1
                stringstream ss(sVal); ss >> iSurface

                if iSurface==-1:
                { delete pOD; return 0; } # not a valid field

                bSurfPending=True

                dCurvature=0.
                dDiameter=0.
                sType="Air"
                dConic=0.
                sComment=""
                dInnerDiameter=0.



		if sKey == "YFLD") #input light field of view (in degree for now:
			stringstream ss(sVal)
			while (not ss.eof())
				d = 0.
				ss >> d
				if d > dHalfFOV:
					dHalfFOV = d



        if sKey=="UNIT":
            string sFirstVal=sVal.substr(0,sVal.find_first_of(" "))

            if sFirstVal=="MM":
                dDimensionFactor=1.

            if sFirstVal=="METER":
                dDimensionFactor=1.e3

            if sFirstVal=="CM":
                dDimensionFactor=10.

            if sFirstVal=="IN":
                dDimensionFactor=25.4


        if sKey=="TYPE":
            if  (sVal!="STANDARD") and (sVal!="EVENASPH") :
            { delete pOD; return 0; } # dont' know how to parse


        if(sKey=="CURV") #rcurv
            stringstream ss(sVal); ss >> dCurvature


        if(sKey=="COMM") #comment
            sComment=sVal


		if (sKey == "STOP") #for example, stop
			sType="stop"

		
		
		if(sKey=="NOTE") #note
            string sNoteVal
            auto iPosSpace=sVal.find_first_of(" ")
            if iPosSpace!=string.npos:
                sNoteVal=sVal.substr(iPosSpace,string.npos)

            sNote+=sNoteVal+"\n"


        if(sKey=="DISZ") #thicks
			if sVal == "INFINITY":
				dTick = 0.
			else:
				stringstream ss(sVal)
				ss >> dTick



        if sKey=="GCAT":
            stringstream ss(sVal); ss >> sGlassCatalog; #TODO use


        if(sKey=="OBSC") #inner diameter
            double dTemp
            stringstream ss(sVal); ss >> dTemp >> dInnerDiameter


        if(sKey=="CONI") #conic
            stringstream ss(sVal); ss >> dConic


        if(sKey=="DIAM") #diameter
            stringstream ss(sVal); ss >> dDiameter
            dDiameter*=2.


        if(sKey=="ENPD") #aperture diameter
            stringstream ss(sVal); ss >> dApertureDiameter


        if(sKey=="GLAS") #type or glass
            string sFirstWord
            auto iPosSpace=sVal.find_first_of(" ")
            if iPosSpace!=string.npos:
                sFirstWord=sVal.substr(0,iPosSpace)

            if sFirstWord=="MIRROR":
                sType="reflect"
			else:
				sType = sFirstWord; #TODO more robust test
				bMustUseColoredLight=True




	if bSurfPending:
		pOD.insert_surface(iSurface)
		pOD.set_comment(iSurface, sComment)
		pOD.set_type(iSurface, sType)
		pOD.set(iSurface, THICK, dDimensionFactor*dTick)
		pOD.set(iSurface, CURVATURE, dCurvature / dDimensionFactor)
		pOD.set(iSurface, DIAMETER, dDiameter*dDimensionFactor)
		pOD.set(iSurface, CONIC, dConic)
		pOD.set(iSurface, INNER_DIAMETER, dInnerDiameter*dDimensionFactor)
		bSurfPending = False

	assert(not bSurfPending)

    if not sNote.empty():
        pOD.set_note(sNote)

    # set the last surface type as image, better check
	iLastSurface = pOD.nb_surface() - 1
	if iLastSurface >=0:
    {    
		if pOD.type(iLastSurface) == "Air":
			pOD.set_type(iLastSurface, "image")
			pOD.set(iLastSurface, AUTO_DIAMETER, 1.)
			pOD.set_image_autocurvature(True)

        if dApertureDiameter>=0.:
            pOD.set(0,DIAMETER,dApertureDiameter*dDimensionFactor)


	# add colored light in case of glass, visible light for now
	if bMustUseColoredLight:
		pOD.set_light_colors("Red.Yellow.Green.Blue.")

	#field of view
	pOD.set_half_field_of_view(dHalfFOV)

    return pOD

########################################
