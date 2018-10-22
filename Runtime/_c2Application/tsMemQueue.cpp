#include<limits.h>
#include<mutex>
#include<boost/log/trivial.hpp>
#include"../c2PreDefined.h"
#include"tsMemQueue.h"

using namespace c2;

////////////////////////////////////////////////////////////////////////////////
tsMemQueue::tsMemQueue(size_t sizeBufSize)
	: _pBuf(nullptr)
	, _sizeFreeSize(0)
	, _sizeTotalSize(0)
	, _sizeFreeStart(0)
	, _sizeUsedStart(0) {
	grow(sizeBufSize);
}

tsMemQueue::~tsMemQueue() {
	std::lock_guard<std::mutex> autolck(_Mutex);
	deallocate((char*)_pBuf);
}

// 扩容
void tsMemQueue::grow(size_t sizeNewBufSize) {
	if (_sizeTotalSize)
		BOOST_LOG_TRIVIAL(warning) << "[tsMemQueue::grow]<" << this <<
				"> grow memory size "<< _sizeTotalSize << "to " << sizeNewBufSize << ".";
	_pBuf = (void*)allocator_type::grow((char*)_pBuf, sizeNewBufSize);
	if (!_pBuf)
		BOOST_LOG_TRIVIAL(fatal) << "[tsMemQueue::grow]<" <<
		this << "> allocate memory failed, maybe the size too larget.";

	size_t size_offset = sizeNewBufSize - _sizeTotalSize;
	if (_sizeUsedStart > _sizeFreeStart) {
		memcpy((char*)_pBuf + _sizeUsedStart + size_offset, (char*)_pBuf + _sizeUsedStart,
					_sizeTotalSize - _sizeUsedStart);
		_sizeUsedStart += size_offset;
	}
	_sizeFreeSize += size_offset;
	_sizeTotalSize = sizeNewBufSize;
}

// 往队列中加入数据
void tsMemQueue::_push(const void *pInBuf, size_t sizeSize) {
	//空闲空间够否
	if (sizeSize >= _sizeFreeSize) {
		// 申请的空间太大了
		size_type nNewSize = 0;
		for (nNewSize = _sizeTotalSize; nNewSize + _sizeFreeSize < sizeSize;
				nNewSize += _sizeTotalSize) {
			// 如果分配空间大于DataLimit<int>::max_value()
			if (INT_MAX - nNewSize < _sizeTotalSize)
				BOOST_LOG_TRIVIAL(fatal) << "[tsMemQueue::_push]<" <<
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
void tsMemQueue::_pop(void *pOutBuf, size_t sizeSize) {
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

void tsMemQueue::_seek(size_t sizeSize) {
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

void tsMemQueue::push(const void *pInBuf, size_t sizeSize) {
	std::lock_guard<std::mutex> autolck(_Mutex);

	c2BufLen es_sizewrite = sizeSize; //转化为明确长度的es变量。
	_push(&es_sizewrite, sizeof(es_sizewrite));
	_push(pInBuf, es_sizewrite);
}

size_t tsMemQueue::pop(void *pOutBuf, size_t sizeOutBufSize) {
	std::lock_guard<std::mutex> autolck(_Mutex);
	if (_sizeFreeStart == _sizeUsedStart)
		return 0;

	// 取出长度信息。使用明确长度的es变量。
	c2BufLen es_size = 0;
	_pop(&es_size, sizeof(es_size));

	//接受缓冲区是否够空间
	if (sizeOutBufSize < es_size) {
		BOOST_LOG_TRIVIAL(error) << "[tsMemQueue::read]sizeOutBufSize("
								<< sizeOutBufSize << "< sizeSize("<< es_size << ")";
		BOOST_LOG_TRIVIAL(error) << "[tsMemQueue::read]This msg will be ignored.";
		_seek(es_size);
		return 0;
	}

	// 读取数据
	_pop(pOutBuf, es_size);
	return static_cast<size_t>(es_size);
}

// 队列是否为空
bool tsMemQueue::isEmpty()
{
	std::lock_guard<std::mutex> autolck(_Mutex);
	return _sizeFreeStart == _sizeUsedStart;
}
