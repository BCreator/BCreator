#ifndef C2_PART_H_
#define C2_PART_H_

#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<assert.h>

namespace c2 {
////////////////////////////////////////////////////////////////////////////////

#define	LOCK		//��ʱ�Ǽٵ�
#define	UNLOCK		//��ʱ�Ǽٵ�
#define	AUTOLOCK	//��ʱ�Ǽٵ�
#define	ATOMINCRE	//��ʱ�Ǽٵ�
#define	ATOMDECRE	//��ʱ�Ǽٵ�
#define	ATOMRESET	//��ʱ�Ǽٵ�

/******************************************************************************/
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

/******************************************************************************/
class Part {
	friend struct ARPart;
	C2DefineClass(Part)
private:
	typedef	int GUID;		//FIXME
	GUID _GUID;				//����Ȼ���������йأ�ͨ�����������ַ������ɣ�
	/*------------------------------------------------------------------------*/
	typedef	int ATOMCOUNT;	//��ʱ�Ǽٵ�
	ATOMCOUNT	_Ref;
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
	Part() { _GUID = 0; }
	virtual ~Part() { _GUID = 0; }

	/*------------------------------------------------------------------------*/
public:
	typedef Part* (*CreationFunc)();
	typedef void(*DestructionFunc)(Part *pPart);
	struct LifeFuncs {
		CreationFunc	_C;
		DestructionFunc	_D;
		inline LifeFuncs(CreationFunc C, DestructionFunc D) : _C(C), _D(D) {}
		~LifeFuncs() { _C = 0, _D = 0; }
	};
	typedef stdext::hash_map<std::string, Part::LifeFuncs>	LifeFuncsDict;
	static LifeFuncsDict		_FuncsDict;
	//friend bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C,
	//							Part::DestructionFunc D);
	//friend ARPart c2CreatePart(const char *sClass, const char *sName = 0);
	};

//���˼̳��⣬Part���������û�̬ʹ�á�
struct ARPart {
	inline ARPart() : _p(0) {}
	inline ARPart(Part *pP) {
		if (!pP)
			return;
		AUTOLOCK(_p);
		_p = pP;
		_p->addRef();
	}
	inline ARPart(const ARPart &AR) {
		if (!AR._p)
			return;
		AUTOLOCK(_p);
		_p = AR._p;
		_p->addRef();
	}
	/*------------------------------------------------------------------------*/
	inline ~ARPart() {
		AUTOLOCK(_p);
		if (!_p)
			return;
		_p->release();
	}
	/*------------------------------------------------------------------------*/
	inline ARPart& operator =(ARPart &AR) {
		AUTOLOCK(_p);
		if (_p)
			_p->release();
		_p = AR._p;
		_p->addRef();
		return *this;
	}
	inline ARPart& operator =(Part *pP) {
		AUTOLOCK(_p);
		if (_p)
			_p->release();
		_p = pP;
		_p->addRef();
		return *this;
	}
	inline Part* operator ->() const {
		return _p;
	}
	inline bool operator !() const {
		return 0 == _p;
	}
	inline Part& operator *() const {
		return *_p;
	}
	//inline operator Part *() const {	//��ȷ������ʹ����ָͨ�롣
	//	return _p;
	//}
private:
	Part *_p;
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_PART_H_
