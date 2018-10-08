/*!
 * \file MemQueue.h
 * \date 2018/10/07 15:26
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \brief 
 *
 * ��Ϊ�ײ�����ϲ�ʹ����������ų����ж��̣߳�������Ҫ�����̰߳�ȫ��
 *
 * \note
*/

#ifndef _C2ENGINE_MEMQUEUE_H_
#define _C2ENGINE_MEMQUEUE_H_
namespace c2 {
////////////////////////////////////////////////////////////////////////////////

template<class _T, class _TSize = size_t>
class Allocator
{
public:
	typedef _T			value_type;
	typedef value_type* pointer;

	typedef _TSize		size_type;

	// �����ڴ�
	inline pointer allocate(size_type nSize) {
		return (pointer)malloc(nSize * sizeof(value_type));
	}

	// �ͷ��ڴ�
	inline void deallocate(pointer ptr) {
		std::free(ptr);
	}

	// ����
	inline void construct(pointer ptr, const value_type& val) {
		std::_Construct(ptr, val);
	}

	// ����
	inline void destroy(pointer ptr) {
		std::_Destroy(ptr);
	}

	// ���·����ڴ��С
	pointer grow(pointer pOld, size_type nSize) {
		if (nSize > max_size())
			ray::kernel::LOGGER::Fatal("[Allocator::grow] can not allocate memory, size = %d.\r\n", nSize);
		pointer ptr = (pointer)realloc(pOld, nSize * sizeof(value_type));
#ifdef _MSC_VER
		if (0 == ptr || _msize(ptr) < nSize * sizeof(value_type))
#else
		if (0 == ptr)
#endif
			ray::kernel::LOGGER::Fatal("[Allocator::grow] can not allocate memory, size = %d.errno:%d\r\n", nSize, errno);
		return ptr;
	}

	// ����ܷ��������
	inline size_type max_size() const {
		size_type nCount = (size_type)(-1) / sizeof(value_type);
		return (nCount ? nCount : 1);
	}
};


////////////////////////////////////////////////////////////////////////////////
class MsgBuffer;
class MemoryQueue : public Allocator<Uint8>, public ray::kernel::LockCS
{
	friend class MsgBuffer;
	// �ڴ������
	typedef ray::kernel::Allocator<Uint8> allocator_type;

	// ������ͷ
	struct DataHeader
	{
		NetInfo::MsgHead	_SizeComplete;			//���ݴ�С,�������Ƿ�����������Ϣ
		HandleNet			_hNet;	
	};

	Uint32 _nFreeStart;	//���еĿռ俪ʼ��ַ��ƫ����
	Uint32 _nUsedStart;	//��ռ�õĿռ俪ʼ��ַ��ƫ����
	Uint32 _nFreeSize;	//���ÿռ�	
	Uint32 _nTotalSize;	//�ܿռ�
	Uint8* _pBuf;		//�ڴ�ָ��
	//RV_LOCKOBJ _LockObj;//

	void grow(Uint32 nNewBufSize); 
	void _write(const Uint8* pInBuf, int nSize);	// �������м�������(����)
	void _read(Uint8* pOutBuf, int nSize);			// ��������(����)
	void _seek(int nSize);							// ����һ������
public:
	MemoryQueue(Uint32 nBufSize);	// ��ʼ��С
	~MemoryQueue();
	
	int					isEmpty();																						// �Ƿ�Ϊ��

	//�����read/write������ֻ��������������������
	int					write(const Uint8* pInBuf, const NetInfo::MsgHead &sizeComplete, const HandleNet& hNet, bool bNetEvent);		// hNet��QUQUE��дnSize����С������pInBuf
	int					readAll(MsgBuffer &msgBuffer);
	//NetInfo::MsgHead	read(Uint8* pOutBuf, int nOutBufSize, HandleNet& hNet);											// ��QUQUE���һ�����ݵ�pOutBuf�pOutBuf�Ĵ�СΪnSize�� ��������hNet

	//�����write/read��������ʹ�ã����ܺ�����Ļ���, ���ڷ��������Ļ���������
	int					write(const void* pInBuf, int nSize);		//ֻд��nSize��С�����ݣ�������Netio��Ϣ
	int					read(void* pOutBuf, int nOutBufSize);		//ֻ����һ�����ݣ������ش�С��������Netio��Ϣ
};

class NETIO_API MsgBuffer: public ray::kernel::Allocator<Uint8>
{
	friend class MemoryQueue;

	Uint8* _pBuf;		//�ڴ�ָ��
	Uint32	_nTotal, _nUsed;
	Uint32	_nCurrPos;
public:
	MsgBuffer();
	~MsgBuffer();

	//unsigned char* fetch(NetInfo::MsgHead &head, HandleNet& hNet);				//����non-const buffer,��������ֱ�������buff����
	NetInfo* fetch(NetTCP *pNetTCP, NetInfo::MsgHead &headMsg, void **pMsg);		//����msg_data,����ֱ�������buff����
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//_C2ENGINE_MEMQUEUE_H_