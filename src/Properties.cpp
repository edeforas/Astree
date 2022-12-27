// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "Properties.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

///////////////////////////////////////////////////////////////////////////////
Properties::Properties()
{ }
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, int iValue)
{
    stringstream ss;
    ss << iValue;

    _pairs[sKey]=ss.str();
}
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, unsigned int uiValue)
{
    stringstream ss;
    ss << uiValue;

    _pairs[sKey]=ss.str();
}
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, long lValue)
{
    stringstream ss;
    ss << lValue;

    _pairs[sKey]=ss.str();
}///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, bool bValue)
{
    _pairs[sKey]=bValue?"1":"0";
}
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, double dValue)
{
    stringstream ss;
    ss << setprecision(10) << dValue;

    _pairs[sKey]=ss.str();
}
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, const string& sValue)
{
    _pairs[sKey]=sValue;
}
///////////////////////////////////////////////////////////////////////////////
void Properties::set(const string& sKey, const vector<double>& vdValue)
{
    stringstream ss;
    for(unsigned int i=0;i<vdValue.size();i++)
    {
        ss << vdValue[i] ;
        if(i<vdValue.size()-1)
            ss << " ";
    }

    _pairs[sKey]=ss.str();
}
///////////////////////////////////////////////////////////////////////////////
bool Properties::exist(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    return it!=_pairs.end();
}
///////////////////////////////////////////////////////////////////////////////
string Properties::get(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
        return it->second;
    else
        return "";
}
///////////////////////////////////////////////////////////////////////////////
int Properties::get_int(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
    {
        stringstream ss(it->second);
        int iTmp;
        ss >> iTmp;
        return iTmp;
    } else
        return 0;
}
///////////////////////////////////////////////////////////////////////////////
unsigned int Properties::get_unsigned_int(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
    {
        stringstream ss(it->second);
        unsigned int uiTmp;
        ss >> uiTmp;
        return uiTmp;
    } else
        return 0;
}
///////////////////////////////////////////////////////////////////////////////
long Properties::get_long(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
    {
        stringstream ss(it->second);
        long lTmp;
        ss >> lTmp;
        return lTmp;
    } else
        return 0;
}///////////////////////////////////////////////////////////////////////////////
bool Properties::get_bool(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
        return it->second!="0";
    else
        return false;
}

///////////////////////////////////////////////////////////////////////////////
double Properties::get_double(const string& sKey) const
{
    auto it=_pairs.find(sKey);
    if(it!=_pairs.end())
    {
        stringstream ss(it->second);
        double dTmp;
        ss >> dTmp;
        return dTmp;
    } else
        return 0.;
}
///////////////////////////////////////////////////////////////////////////////
vector<double> Properties::get_vector_double(const string& sKey) const
{
    vector<double> vd;

    stringstream ss;
    string sValue=get(sKey);

    if(sValue.empty())
        return vd;

    ss.str(sValue);

    while(!ss.eof())
    {
        double d;
        if(ss >> d)
            vd.push_back(d);
    }

    return vd;
}
///////////////////////////////////////////////////////////////////////////////
bool Properties::save(const string& sFileName) const
{
    ofstream f(sFileName.c_str());
    if(!f)
        return false;

    for(auto it=_pairs.begin();it!= _pairs.end();++it)
    {
        string sValue=it->second;
        string sKey=it->first;

        if(!sValue.empty())
        {

            istringstream sText(sValue);
            string sLine;
            while (std::getline( sText, sLine))
            {
                f << sKey << "=" << sLine << "\n";
            }
        }
        else
            f << sKey << "=" << "\n";
    }

    f.close();
    return true;
}
//////////////////////////////////////////////////////////////////////////////
bool Properties::load(const string& sFileName)
{
    _pairs.clear();
    string sLine;

    ifstream f(sFileName.c_str());
    if(!f)
        return false;

    while(!f.eof())
    {
        std::getline(f,sLine);

        if(sLine.size()==0)
            continue;

        size_t iPosEgal=sLine.find_first_of("=");
        if(iPosEgal==string::npos)
            continue; //no key!

        //delete eventually the 13 character at the end
        if((int)(sLine[sLine.size()-1])==13)
            sLine.resize(sLine.size()-1);

        string sKey=sLine.substr(0,iPosEgal);
        string sValue=sLine.substr(iPosEgal+1);

        if(exist(sKey))
            set(sKey,get(sKey)+'\n'+sValue);
        else
            set(sKey,sValue);
    }

    f.close();

    return true;
}
//////////////////////////////////////////////////////////////////////////////
void Properties::remove(const string& sKey)
{
    _pairs.erase(sKey);
}
//////////////////////////////////////////////////////////////////////////////
const map<string,string>& Properties::all() const
{
    return _pairs;
}
//////////////////////////////////////////////////////////////////////////////

