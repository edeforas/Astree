// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#ifndef Properties_
#define Properties_

#include <map>
#include <string>
#include <vector>

using namespace std;

/**
Simple association (dictionnary key-values ) class

Handle creation, loading and saving from files, adding and testing keys.

@author Etienne de Foras
@version 1.01
*/
class Properties
{

public:
    Properties();

    bool save(const string& sFileName) const;
    bool load(const string& sFileName);

    void set(const string& sKey,const string& sValue);
    void set(const string& sKey, int iValue);
    void set(const string& sKey, unsigned int uiValue);
    void set(const string& sKey, long lValue);
    void set(const string& sKey, bool bValue);
    void set(const string& sKey, double dValue);
    void set(const string& sKey, const vector<double>& vdValue);

    bool exist(const string& sKey) const;
    void remove(const string& sKey);

    string get(const string& sKey) const;
    int get_int(const string& sKey) const;
    unsigned int get_unsigned_int(const string& sKey) const;
    long get_long(const string& sKey) const;
    bool get_bool(const string& sKey) const;
    double get_double(const string& sKey) const;
    vector<double> get_vector_double(const string& sKey) const;

    const map<string,string>& all() const;
private:
    map<string,string> _pairs;
};

#endif
