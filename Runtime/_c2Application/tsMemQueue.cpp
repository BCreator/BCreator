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

// ����
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

// �������м�������
void tsMemQueue::_push(const void *pInBuf, size_t sizeSize) {
	//���пռ乻��
	if (sizeSize >= _sizeFreeSize) {
		// ����Ŀռ�̫����
		size_type nNewSize = 0;
		for (nNewSize = _sizeTotalSize; nNewSize + _sizeFreeSize < sizeSize;
				nNewSize += _sizeTotalSize) {
			// �������ռ����DataLimit<int>::max_value()
			if (INT_MAX - nNewSize < _sizeTotalSize)
				BOOST_LOG_TRIVIAL(fatal) << "[tsMemQueue::_push]<" <<
							this << "> can not alloc memory, the size too larget.";
		}
		grow(_sizeTotalSize + nNewSize);
		BOOST_ASSERT(sizeSize <= _sizeFreeSize);
	};

	//������Ϊѭ��ʹ��,������������β��ʱ,���ݿ��ܳ���Ҫ�ֳ����δ�������
	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
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

	// TODO������Ƿ����ڴ�й¶
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
// pOutBuf��һ���㹻��Ŀռ䣬��sizeSize����С���ڴ�
void tsMemQueue::_pop(void *pOutBuf, size_t sizeSize) {
	BOOST_ASSERT(_sizeFreeStart != _sizeUsedStart);

	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
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

	// ����Ƿ����ڴ�й¶(�߼��ϵ�)
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
	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
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

	// ����Ƿ����ڴ�й¶(�߼��ϵ�)
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

	c2BufLen es_sizewrite = sizeSize; //ת��Ϊ��ȷ���ȵ�es������
	_push(&es_sizewrite, sizeof(es_sizewrite));
	_push(pInBuf, es_sizewrite);
}

size_t tsMemQueue::pop(void *pOutBuf, size_t sizeOutBufSize) {
	std::lock_guard<std::mutex> autolck(_Mutex);
	if (_sizeFreeStart == _sizeUsedStart)
		return 0;

	// ȡ��������Ϣ��ʹ����ȷ���ȵ�es������
	c2BufLen es_size = 0;
	_pop(&es_size, sizeof(es_size));

	//���ܻ������Ƿ񹻿ռ�
	if (sizeOutBufSize < es_size) {
		BOOST_LOG_TRIVIAL(error) << "[tsMemQueue::read]sizeOutBufSize("
								<< sizeOutBufSize << "< sizeSize("<< es_size << ")";
		BOOST_LOG_TRIVIAL(error) << "[tsMemQueue::read]This msg will be ignored.";
		_seek(es_size);
		return 0;
	}

	// ��ȡ����
	_pop(pOutBuf, es_size);
	return static_cast<size_t>(es_size);
}

// �����Ƿ�Ϊ��
bool tsMemQueue::isEmpty()
{
	std::lock_guard<std::mutex> autolck(_Mutex);
	return _sizeFreeStart == _sizeUsedStart;
}
