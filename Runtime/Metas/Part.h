#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<assert.h>

//==============================================================================
#define C2DefineClass(classname)	\
		public:\
			static Part* _create() {\
				return new classname;\
			}\
			static void _destroy(Part *pPart) {\
				if (!pPart)\
					return;\
				classname* p = dynamic_cast<classname*>(pPart);\
				if (!p)\
					return;\
				delete p;\
			}

#define	LOCK		//暂时是假的
#define	UNLOCK		//暂时是假的
#define	AUTOLOCK	//暂时是假的
#define	ATOMINCRE	//暂时是假的
#define	ATOMDECRE	//暂时是假的
#define	ATOMRESET	//暂时是假的
//PARTAUTOREF也还没有，对外界完全禁止使用指针。确保重用性？

class Part {
	C2DefineClass(Part)
private:
	typedef	int GUID;
	GUID _GUID;
	typedef	int ATOMCOUNT;	//暂时是假的
	ATOMCOUNT	_Ref;
	inline void resetRef() {
		ATOMRESET(_Ref);
	}
	inline void addRef() {
		ATOMINCRE(_Ref);
	}
	inline void release()
	{
		ATOMDECRE(_Ref);
		AUTOLOCK(_Ref);
		if (0 >= _Ref)
			_destroy(this);
	}
protected:
	Part() { _GUID = NULL; }
	virtual ~Part() { _GUID = NULL; }

protected:
	typedef Part* (*CreationFunc)();
	typedef void(*DestructionFunc)(Part *pPart);
	struct LifeFuncs {
		CreationFunc	_C;
		DestructionFunc	_D;
		inline LifeFuncs(CreationFunc C, DestructionFunc D) : _C(C), _D(D) {}
		~LifeFuncs() { _C = NULL, _D = NULL; }
		friend bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C, Part::DestructionFunc D);
	};
	typedef stdext::hash_map<std::string, Part::LifeFuncs>	LifeFuncsDict;
	static LifeFuncsDict		_FuncsDict;
	friend bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C, Part::DestructionFunc D);
	friend Part* c2CreatePart(const char *sClass, const char *sName);
};
Part::LifeFuncsDict		Part::_FuncsDict;

bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C, Part::DestructionFunc D) {
	if (!sClass || !C || !D)
		return false;
	Part::LifeFuncs fs(C, D);
	return Part::_FuncsDict.insert(			//如果已存在同样类名注册，则返回false。
				Part::LifeFuncsDict::value_type(sClass, fs)).second;
}

Part* c2CreatePart(const char *sClass, const char *sName = NULL) {
	if (!sClass)
		return NULL;
	Part::LifeFuncsDict::iterator ci = Part::_FuncsDict.find(sClass);
	if (ci == Part::_FuncsDict.end())
		return NULL;
	Part::CreationFunc	c = ci->second._C;
	assert(c);
	return c();
}

class PartClassTest0 : private Part {
	C2DefineClass(PartClassTest0)
};

#define C2RegistPartClass(classname)	\
		_c2RegistPartClass(#classname, classname::_create, classname::_destroy);