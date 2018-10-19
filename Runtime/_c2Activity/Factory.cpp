#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<assert.h>

#include"../Metas/Part.h"
using namespace c2;
////////////////////////////////////////////////////////////////////////////////

Part::CreationDict		Part::_CreationDict;

/******************************************************************************/
bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C) {
	if (!sClass || !C )
		return false;
	return Part::_CreationDict.insert(			//如果已存在同样类名注册，则返回false。
						Part::CreationDict::value_type(sClass, C)).second;
}

Part::ARPart c2CreatePart(const char *sClass, const char *sName = nullptr) {
	if (!sClass)
		return NULL;
	Part::CreationDict::iterator ci = Part::_CreationDict.find(sClass);
	if (ci == Part::_CreationDict.end())
		return NULL;
	Part::CreationFunc	create = ci->second;
	assert(create);
	return create();
}
