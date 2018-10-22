#ifndef C2_TSMEQUEUE_H_
#define C2_TSMEQUEUE_H_

namespace c2{

//TODO: 字节序，平台位等适配，以下仅仅是个概念形式于此备忘，不是实际设计。
//void c2BufUniformAdapter()

////////////////////////////////////////////////////////////////////////////////
template<class _T, class _TSize = size_t>
struct Allocator {//TODO: 撤换掉，用STL或者BOOST的。
	typedef _T			value_type;
	typedef value_type* pointer;
	typedef _TSize		size_type;
	// 申请内存
	inline pointer allocate(size_type sizeSize) {
		return (pointer)malloc(sizeSize * sizeof(value_type));
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
	pointer grow(pointer pOld, size_type sizeSize) {
		if (sizeSize > max_size())
			BOOST_LOG_TRIVIAL(fatal) <<
			"[Allocator::grow] can not allocate memory, size= " << sizeSize;
		pointer ptr = (pointer)realloc(pOld, sizeSize * sizeof(value_type));
#ifdef _MSC_VER
		if (NULL == ptr || _msize(ptr) < sizeSize * sizeof(value_type))
#else
		if (NULL == ptr)
#endif
			BOOST_LOG_TRIVIAL(fatal) <<
			"[Allocator::grow] can not allocate memory, size= " << sizeSize <<
			". errno: " << errno;
		return ptr;
	}
	// 最多能分配的数量
	inline size_type max_size() const {
		size_type nCount = (size_type)(-1) / sizeof(value_type);
		return (nCount ? nCount : 1);
	}
};
/*============================================================================*/
class tsMemQueue final : Allocator<char> {
	using allocator_type= Allocator<char>;
	size_t		_sizeFreeStart;	//空闲的空间开始地址的偏移量
	size_t		_sizeUsedStart;	//已占用的空间开始地址的偏移量
	size_t		_sizeFreeSize;	//可用空间	
	size_t		_sizeTotalSize;	//总空间
	void		*_pBuf;			//内存指针
	std::mutex	_Mutex;

	void grow(size_t sizeNewBufSize);
	void _push(const void *pInBuf, size_t sizeSize);		// 往队列中加入数据(物理)
	void _pop(void *pOutBuf, size_t sizeSize);				// 读出数据(物理)
	void _seek(size_t sizeSize);							// 跳过一定长度
public:
	tsMemQueue(size_t sizeBufSize);	// 初始大小
	~tsMemQueue();
	bool			isEmpty();		// 是否为空
	//只写入sizeSize大小的数据
	void			push(const void* pInBuf, size_t sizeSize);
	//只读出一段数据，并返回大小，0为失败。sizeOutBufSize是接受缓冲区大小。
	size_t			pop(void *pOutBuf, size_t sizeOutBufSize);
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_TSMEQUEUE_H_

//private:
//	class MsgBuffer {
//		void	*_pBuf;		//内存指针
//		size_t	_nTotal, _nUsed;
//		size_t	_nCurrPos;
//	public:
//		MsgBuffer() : _pBuf(nullptr), _nTotal(0), _nUsed(0), _nCurrPos(0) {}
//		~MsgBuffer() {}
//	};
//	struct esMsgHead {
//		union {
//			size_t	  _esSize;	//？负值表示被拆包，且此包不是包的结尾。FIXME
//			struct {
//				size_t		_esPackSize : 30;	//大小
//				//0: 表示是正常的封包; 1:表示是系统发出的NetEvent,不加密，且MsgType = 0；
//				size_t		_esNetEvent : 1;
//				//0: 表示是结尾包，数据接收完毕， 1表示被拆包，且此包不是结尾包，需要继续
//				//等待余下的封包
//				size_t		_esDataComplete : 1;
//			};
//		};
//		inline MsgHead() : _nSize(0) {}
//		inline MsgHead(size_t sizeSize) : _nSize(sizeSize) {}
//		}
//	};
//	/*------------------------------------------------------------------------*/
//private: