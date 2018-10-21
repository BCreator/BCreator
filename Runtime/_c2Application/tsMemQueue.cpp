#include<limits.h>
#include<mutex>
#include<boost/log/trivial.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace c2 {
template<class _T, class _TSize = size_t>
class Allocator {
public:
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
}//namespace c2
#include"tsMemQueue.h"
using namespace c2;

////////////////////////////////////////////////////////////////////////////////
tsMemoryQueue::tsMemoryQueue(size_t sizeBufSize)
	: _pBuf(nullptr)
	, _sizeFreeSize(0)
	, _sizeTotalSize(0)
	, _sizeFreeStart(0)
	, _sizeUsedStart(0) {
	grow(sizeBufSize);
}

tsMemoryQueue::~tsMemoryQueue() {
	std::lock_guard<std::mutex> autolck(_Mutex);
	deallocate((char*)_pBuf);
}

// 扩容
void tsMemoryQueue::grow(size_t nNewBufSize) {
	if (_sizeTotalSize)
		BOOST_LOG_TRIVIAL(fatal) << "[tsMemoryQueue::grow]<" << this <<
				"> grow memory size "<< _sizeTotalSize << "to " << nNewBufSize << ".";
	_pBuf = (void*)allocator_type::grow((char*)_pBuf, nNewBufSize);

	size_t size_offset = nNewBufSize - _sizeTotalSize;
	if (_sizeUsedStart > _sizeFreeStart) {
		memcpy((char*)_pBuf + _sizeUsedStart + size_offset, (char*)_pBuf + _sizeUsedStart,
					_sizeTotalSize - _sizeUsedStart);
		_sizeUsedStart += size_offset;
	}
	_sizeFreeSize += size_offset;
	_sizeTotalSize = nNewBufSize;
}

// 往队列中加入数据
void tsMemoryQueue::_write(const void *pInBuf, size_t sizeSize) {
	//空闲空间够否
	if (sizeSize >= _sizeFreeSize) {
		// 申请的空间太大了
		size_type nNewSize = 0;
		for (nNewSize = _sizeTotalSize; nNewSize + _sizeFreeSize < sizeSize;
				nNewSize += _sizeTotalSize) {
			// 如果分配空间大于DataLimit<int>::max_value()
			if (INT_MAX - nNewSize < _sizeTotalSize)
				BOOST_LOG_TRIVIAL(fatal) << "[tsMemoryQueue::_write]<" <<
							this << "> can not alloc memory, the size too larget.";
		}
		grow(_sizeTotalSize + nNewSize);
		BOOST_ASSERT(sizeSize <= _sizeFreeSize);
	};

	//缓冲区为循环使用,当靠近缓冲区尾部时,数据可能出现要分成两段处理的情况
	//nFrirstCopySize为一次可以写入的最大数据大小
	size_t firstcopy_size = _sizeTotalSize - _sizeFreeStart;
	if (firstcopy_size >= sizeSize) {
		memcpy((char*)_pBuf + _sizeFreeStart, (char*)pInBuf, sizeSize);
		_sizeFreeStart += sizeSize;
	}
	else {
		memcpy((char*)_pBuf + _sizeFreeStart, (char*)pInBuf, firstcopy_size);
		memcpy((char*)_pBuf, (char*)pInBuf + firstcopy_size, sizeSize - firstcopy_size);
		_sizeFreeStart = sizeSize - firstcopy_size;
	};

	_sizeFreeSize -= sizeSize;

	// TODO：检测是否有内存泄露
#ifdef C2_CHECK_MEM
	BOOST_ASSERT(_sizeFreeStart != _sizeUsedStart);
	if (_sizeFreeStart > _sizeUsedStart)
		BOOST_ASSERT(_sizeFreeStart - _sizeUsedStart + _sizeFreeSize == _sizeTotalSize);
	else
		BOOST_ASSERT(_sizeTotalSize - _sizeUsedStart + _sizeFreeStart + _sizeFreeSize ==
						_sizeTotalSize);
#endif // RV_UNCHECK_MEM
}

/*============================================================================*/
// pOutBuf是一个足够大的空间，读sizeSize个大小的内存
void tsMemoryQueue::_read(void *pOutBuf, size_t sizeSize) {
	BOOST_ASSERT(_sizeFreeStart != _sizeUsedStart);

	//nFrirstCopySize为一次可以写入的最大数据大小
	size_t firstcopy_size = _sizeTotalSize - _sizeUsedStart;
	if (firstcopy_size >= sizeSize)
	{
		memcpy((char*)pOutBuf, (char*)_pBuf + _sizeUsedStart, sizeSize);
		_sizeUsedStart += sizeSize;
	}
	else
	{
		memcpy((char*)pOutBuf, (char*)_pBuf + _sizeUsedStart, firstcopy_size);
		memcpy((char*)pOutBuf + firstcopy_size, (char*)_pBuf, sizeSize - firstcopy_size);
		_sizeUsedStart = sizeSize - firstcopy_size;
	};

	_sizeFreeSize += sizeSize;

	// 检测是否有内存泄露(逻辑上的)
#ifdef C2_CHECK_MEM
	if (_sizeFreeStart == _sizeUsedStart)
		return;
	if (_sizeFreeStart > _sizeUsedStart)
		BOOST_ASSERT(_sizeFreeStart - _sizeUsedStart + _sizeFreeSize == _sizeTotalSize);
	else
		BOOST_ASSERT(_sizeTotalSize - _sizeUsedStart + _sizeFreeStart + _sizeFreeSize ==
					_sizeTotalSize);
#endif // _DEBUG
}

void tsMemoryQueue::_seek(size_t sizeSize) {
	BOOST_ASSERT(_sizeFreeStart != _sizeUsedStart);
	//nFrirstCopySize为一次可以写入的最大数据大小
	size_t firstcopy_size = _sizeTotalSize - _sizeUsedStart;
	if (firstcopy_size >= sizeSize)
	{
		_sizeUsedStart += sizeSize;
	}
	else
	{
		_sizeUsedStart = sizeSize - firstcopy_size;
	};

	_sizeFreeSize += sizeSize;

	// 检测是否有内存泄露(逻辑上的)
#ifdef C2_CHECK_MEM
	if (_sizeFreeStart == _sizeUsedStart)
		return;
	if (_sizeFreeStart > _sizeUsedStart)
		BOOST_ASSERT(_sizeFreeStart - _sizeUsedStart + _sizeFreeSize == _sizeTotalSize);
	else
		BOOST_ASSERT(_sizeTotalSize - _sizeUsedStart + _sizeFreeStart + _sizeFreeSize ==
					_sizeTotalSize);
#endif // _DEBUG
}

//只写入sizeSize大小的数据，不含有Netio信息
void tsMemoryQueue::write(const void *pInBuf, size_t sizeSize) {
	std::lock_guard<std::mutex> autolck(_Mutex);

	c2BufLen esSizeWrite = sizeSize; //转化为明确长度的es变量。
	_write(&esSizeWrite, sizeof(esSizeWrite));
	_write(pInBuf, esSizeWrite);
}

//只读出一段数据，并返回大小，不含有Netio信息
bool tsMemoryQueue::read(void *pOutBuf, size_t sizeOutBufSize) {
	std::lock_guard<std::mutex> autolck(_Mutex);
	if (_sizeFreeStart == _sizeUsedStart)
		return false;

	// 取出长度信息。使用明确长度的es变量。
	c2BufLen esSize = 0;
	_read(&esSize, sizeof(esSize));

	//接受缓冲区是否够空间
	if (sizeOutBufSize < esSize) {
		BOOST_LOG_TRIVIAL(error) << "[tsMemoryQueue::read]sizeOutBufSize("
								<< sizeOutBufSize << "< sizeSize("<< esSize << ")";
		BOOST_LOG_TRIVIAL(error) << "[tsMemoryQueue::read]This msg will be ignored.";
		_seek(esSize);
		return false;
	}

	// 读取数据
	_read(pOutBuf, esSize);
	return true;
}

// 队列是否为空
bool tsMemoryQueue::isEmpty()
{
	std::lock_guard<std::mutex> autolck(_Mutex);
	return _sizeFreeStart == _sizeUsedStart;
}
