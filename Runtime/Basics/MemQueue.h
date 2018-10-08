/*!
 * \file MemQueue.h
 * \date 2018/10/07 15:26
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \brief 
 *
 * 作为底层包，上层使用情况不能排除会有多线程，所以需要考虑线程安全。
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

	// 申请内存
	inline pointer allocate(size_type nSize) {
		return (pointer)malloc(nSize * sizeof(value_type));
	}

	// 释放内存
	inline void deallocate(pointer ptr) {
		std::free(ptr);
	}

	// 构造
	inline void construct(pointer ptr, const value_type& val) {
		std::_Construct(ptr, val);
	}

	// 析构
	inline void destroy(pointer ptr) {
		std::_Destroy(ptr);
	}

	// 重新分配内存大小
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

	// 最多能分配的数量
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
	// 内存分配器
	typedef ray::kernel::Allocator<Uint8> allocator_type;

	// 物理块的头
	struct DataHeader
	{
		NetInfo::MsgHead	_SizeComplete;			//数据大小,并含有是否是完整包信息
		HandleNet			_hNet;	
	};

	Uint32 _nFreeStart;	//空闲的空间开始地址的偏移量
	Uint32 _nUsedStart;	//已占用的空间开始地址的偏移量
	Uint32 _nFreeSize;	//可用空间	
	Uint32 _nTotalSize;	//总空间
	Uint8* _pBuf;		//内存指针
	//RV_LOCKOBJ _LockObj;//

	void grow(Uint32 nNewBufSize); 
	void _write(const Uint8* pInBuf, int nSize);	// 往队列中加入数据(物理)
	void _read(Uint8* pOutBuf, int nSize);			// 读出数据(物理)
	void _seek(int nSize);							// 跳过一定长度
public:
	MemoryQueue(Uint32 nBufSize);	// 初始大小
	~MemoryQueue();
	
	int					isEmpty();																						// 是否为空

	//下面的read/write函数，只用于网络封包缓冲区管理。
	int					write(const Uint8* pInBuf, const NetInfo::MsgHead &sizeComplete, const HandleNet& hNet, bool bNetEvent);		// hNet往QUQUE里写nSize个大小的数据pInBuf
	int					readAll(MsgBuffer &msgBuffer);
	//NetInfo::MsgHead	read(Uint8* pOutBuf, int nOutBufSize, HandleNet& hNet);											// 从QUQUE里读一条数据到pOutBuf里，pOutBuf的大小为nSize， 数据来自hNet

	//下面的write/read必须配套使用，不能和上面的混用, 用于非网络封包的缓冲区管理。
	int					write(const void* pInBuf, int nSize);		//只写入nSize大小的数据，不含有Netio信息
	int					read(void* pOutBuf, int nOutBufSize);		//只读出一段数据，并返回大小，不含有Netio信息
};

class NETIO_API MsgBuffer: public ray::kernel::Allocator<Uint8>
{
	friend class MemoryQueue;

	Uint8* _pBuf;		//内存指针
	Uint32	_nTotal, _nUsed;
	Uint32	_nCurrPos;
public:
	MsgBuffer();
	~MsgBuffer();

	//unsigned char* fetch(NetInfo::MsgHead &head, HandleNet& hNet);				//返回non-const buffer,这样可以直接用这个buff解密
	NetInfo* fetch(NetTCP *pNetTCP, NetInfo::MsgHead &headMsg, void **pMsg);		//返回msg_data,可以直接用这个buff解密
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//_C2ENGINE_MEMQUEUE_H_