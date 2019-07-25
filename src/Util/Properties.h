#ifndef _Properties_
#define _Properties_

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

    bool save(string sFileName) const;
    bool load(string sFileName);

    void set(string sKey, string sValue);
    void set(string sKey, int iValue);
    void set(string sKey, unsigned int uiValue);
    void set(string sKey, long lValue);
    void set(string sKey, bool bValue);
    void set(string sKey, double dValue);
    void set(string sKey, const vector<double>& vdValue);

    bool exist(string sKey) const;
    void remove(string sKey);

    string get(string sKey) const;
    int get_int(string sKey) const;
    unsigned int get_unsigned_int(string sKey) const;
    long get_long(string sKey) const;
    bool get_bool(string sKey) const;
    double get_double(string sKey) const;
    vector<double> get_vector_double(string sKey) const;

    const map<string,string>& all() const;
private:
    map<string,string> _pairs;
};

#endif
