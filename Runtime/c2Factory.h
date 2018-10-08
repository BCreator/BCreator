#ifndef C2_FACTORY_H_
#define C2_FACTORY_H_

#include"./Metas/_Defined.h"
#include"./Metas/Part.h"
////////////////////////////////////////////////////////////////////////////////

C2Interface c2::ARPart c2CreatePart(const char *sClass, const char *sName = 0);
C2Interface bool _c2RegistPartClass(const char *sClass, 
						c2::Part::CreationFunc C, c2::Part::DestructionFunc D);
#define C2RegistPartClass(classname)	\
	::_c2RegistPartClass(#classname, classname::_create, classname::_destroy);

////////////////////////////////////////////////////////////////////////////////
#endif//C2_FACTORY_H_
