#ifndef C2_PART_H_
#define C2_PART_H_

#include<assert.h>
#include<string>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include<hash_map>
#include<mutex>
#include<boost/smart_ptr/intrusive_ptr.hpp>
#include<boost/atomic.hpp>

#define USE_intrusive_ptr

namespace c2 {
////////////////////////////////////////////////////////////////////////////////


/******************************************************************************/
#define C2DefineClass(classname)	\
		public:\
			static Part* _create() {\
				return new classname;\
			}

/******************************************************************************/
//���˼̳��⣬Part���������û�̬ʹ�á�
//TODO�� ֻ��һ����ʱ��ԭ�ͼ�ʵ�֣��������½�����Ŀ���������ԭ�ͽ�ģ��1���̰߳�ȫ����
//�걸��ԭ�Ӳ�����lockfree�����걸��2������Լ��Ҳ�����걸�����绹��Ҫ�ø��Ͻ���������
//�Ѹ�ֵ��Ҳ��˽�л�������Ӧ��ֻ��������������ʵ�ַ�������
class Part {
public:
	using ARPart = boost::intrusive_ptr<Part>;
	C2DefineClass(Part)
private:
	typedef	int			GUID;		//FIXME
	GUID _GUID;						//����Ȼ���������йأ�ͨ�����������ַ������ɣ�
	/*------------------------------------------------------------------------*/
private:
	mutable boost::atomic<int>	_Ref;
	friend void			intrusive_ptr_add_ref(const Part *pP) {
		if (!pP)			return;
		pP->_Ref.fetch_add(1, boost::memory_order_relaxed);
	}
	friend void			intrusive_ptr_release(const Part *pP)
	{
		if (!pP)			return;
		if (pP->_Ref.fetch_sub(1, boost::memory_order_release) == 1) {
			boost::atomic_thread_fence(boost::memory_order_acquire);
			delete pP;
		}
	}
protected:
	explicit Part() : _Ref(0), _GUID(0) {}
	virtual ~Part() { _Ref = 0; _GUID = 0; }
	Part(Part &&other) = delete;
	//Part& operator=(Part &&rhs);	//TODO

	/*------------------------------------------------------------------------*/
public:
	using CreationFunc		= Part* (*)();
	typedef stdext::hash_map<std::string, CreationFunc>	CreationDict;
	static CreationDict		_CreationDict;
	//friend bool _c2RegistPartClass(const char *sClass, Part::CreationFunc C);
	//friend ARPart c2CreatePart(const char *sClass, const char *sName = nullptr);
};


////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_PART_H_
