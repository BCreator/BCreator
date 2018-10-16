#ifndef C2_FACTORY_H_
#define C2_FACTORY_H_

#include"./Metas/PreDefined.h"
#include"./Metas/Part.h"
////////////////////////////////////////////////////////////////////////////////

C2Interface c2::Part::ARPart c2CreatePart(const char *sClass, const char *sName = nullptr);
C2Interface bool _c2RegistPartClass(const char *sClass, c2::Part::CreationFunc C);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_FACTORY_H_
