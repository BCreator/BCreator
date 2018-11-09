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

////////////////////////////////////////////////////////////////////////////////

#define C2DefineClass(classname)	\
		public:\
			static c2Part* _create() {\
				return new classname;\
			}

/******************************************************************************/
//���˼̳��⣬c2Part���������û�̬ʹ�á�
//TODO�� ֻ��һ����ʱ��ԭ�ͼ�ʵ�֣��������½�����Ŀ���������ԭ�ͽ�ģ��1���̰߳�ȫ����
//�걸��ԭ�Ӳ�����lockfree�����걸��2������Լ��Ҳ�����걸�����绹��Ҫ�ø��Ͻ���������
//�Ѹ�ֵ��Ҳ��˽�л�������Ӧ��ֻ��������������ʵ�ַ�������
class c2Part {
public:
	using ARPart = boost::intrusive_ptr<c2Part>;
	C2DefineClass(c2Part)
private:
	typedef	int			GUID;		//FIXME
	GUID _GUID;						//����Ȼ���������йأ�ͨ�����������ַ������ɣ�
	/*------------------------------------------------------------------------*/
private:
	mutable boost::atomic<int>	_Ref;
	friend void			intrusive_ptr_add_ref(const c2Part *pP) {
		if (!pP)			return;
		pP->_Ref.fetch_add(1, boost::memory_order_relaxed);
	}
	friend void			intrusive_ptr_release(const c2Part *pP)
	{
		if (!pP)			return;
		if (pP->_Ref.fetch_sub(1, boost::memory_order_release) == 1) {
			boost::atomic_thread_fence(boost::memory_order_acquire);
			delete pP;
		}
	}
protected:
	explicit c2Part() : _Ref(0), _GUID(0) {}
	virtual ~c2Part() { _Ref = 0; _GUID = 0; }
	c2Part(c2Part &&other) = delete;
	//c2Part& operator=(c2Part &&rhs);	//TODO
	/*------------------------------------------------------------------------*/
public:
	using CreationFunc		= c2Part* (*)();
	typedef stdext::hash_map<std::string, CreationFunc>	CreationDict;
	static CreationDict		_CreationDict;
	//friend bool _c2RegistPartClass(const char *sClass, c2Part::CreationFunc C);
	//friend ARPart c2CreatePart(const char *sClass, const char *sName = nullptr);
/*============================================================================*/
private:
	std::vector<ARPart>		_Parts;
};


////////////////////////////////////////////////////////////////////////////////
#endif//C2_PART_H_
