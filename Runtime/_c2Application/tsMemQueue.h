#ifndef C2_TXMEMORYQUEUE_H_
#define C2_TXMEMORYQUEUE_H_

//TODO: 明确长度的数字类型应该仅仅用在本处，所以最好后面把名字修改一下。其他请见README.md
#ifdef _MSC_VER
using c2BufLen= unsigned __int64;//32位和64下size_t并不明确，故定义个明确大小的缓冲消息长度值
typedef __int8 Sint8;
typedef unsigned __int8 Uint8;
typedef __int16 Sint16;
typedef unsigned __int16 Uint16;
typedef __int32 Sint32;
typedef unsigned __int32 Uint32;
typedef __int64 Sint64;
typedef unsigned __int64 Uint64;
#else
using c2BufLen64 = unsigned long long;//32位和64下size_t并不明确，故定义个明确大小的消息长度值
typedef signed char Sint8;
typedef unsigned char Uint8;
typedef signed short Sint16;
typedef unsigned short Uint16;
typedef signed int Sint32;
typedef unsigned int Uint32;
typedef signed long long Sint64;
typedef unsigned long long Uint64;
#endif
//public:
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

namespace c2{

//TODO: 字节序，平台位等适配，以下仅仅是个概念形式于此备忘，不是实际设计。
//void c2BufUniformAdapter()

////////////////////////////////////////////////////////////////////////////////
class tsMemoryQueue : Allocator<char> {
	using allocator_type= Allocator<char>;
	size_t		_sizeFreeStart;	//空闲的空间开始地址的偏移量
	size_t		_sizeUsedStart;	//已占用的空间开始地址的偏移量
	size_t		_sizeFreeSize;		//可用空间	
	size_t		_sizeTotalSize;	//总空间
	void		*_pBuf;			//内存指针
	std::mutex	_Mutex;

	void grow(size_t nNewBufSize);
	void _write(const void *pInBuf, size_t sizeSize);		// 往队列中加入数据(物理)
	void _read(void *pOutBuf, size_t sizeSize);			// 读出数据(物理)
	void _seek(size_t sizeSize);							// 跳过一定长度
public:
	tsMemoryQueue(size_t sizeBufSize);	// 初始大小
	~tsMemoryQueue();
	bool		isEmpty();		// 是否为空
	//只写入sizeSize大小的数据，不含有Netio信息
	void		write(const void* pInBuf, size_t sizeSize);
	//只读出一段数据，并返回大小，不含有Netio信息
	bool		read(void *pOutBuf, size_t sizeOutBufSize);
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_TXMEMORYQUEUE_H_
