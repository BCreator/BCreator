#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<assert.h>

#include"Part.h"
using namespace c2;
////////////////////////////////////////////////////////////////////////////////

Part::LifeFuncsDict		Part::_FuncsDict;

/******************************************************************************/
bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C,
						Part::DestructionFunc D) {
	if (!sClass || !C || !D)
		return false;
	Part::LifeFuncs fs(C, D);
	return Part::_FuncsDict.insert(			//如果已存在同样类名注册，则返回false。
						Part::LifeFuncsDict::value_type(sClass, fs)).second;
}

ARPart c2CreatePart(const char *sClass, const char *sName = 0) {
	if (!sClass)
		return NULL;
	Part::LifeFuncsDict::iterator ci = Part::_FuncsDict.find(sClass);
	if (ci == Part::_FuncsDict.end())
		return NULL;
	Part::CreationFunc	c = ci->second._C;
	assert(c);
	return c();
}
