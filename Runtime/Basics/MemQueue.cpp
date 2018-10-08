#include"../Metas/PreDefined.h"
#include"MemQueue.h"
//#include"../c2Debugger.h"

using namespace c2;
////////////////////////////////////////////////////////////////////////////////

MemoryQueue::MemoryQueue(Uint32 nBufSize)
	: _pBuf(NULL)
	, _nFreeSize(0)
	, _nTotalSize(0)
	, _nFreeStart(0)
	, _nUsedStart(0)
{
	grow(nBufSize);	
}

MemoryQueue::~MemoryQueue()
{
	AUTOLOCK(this);
	deallocate(_pBuf);
}

// ����
void MemoryQueue::grow(Uint32 nNewBufSize)
{
	//if(_nTotalSize)
	//	LOGGER::Warn("[MemoryQueue::grow]<%p> grow memory size %d to %d.\r\n", this, _nTotalSize, nNewBufSize);
	_pBuf = allocator_type::grow(_pBuf, nNewBufSize);
	
	Uint32 nSizeOffset = nNewBufSize - _nTotalSize;
	if( _nUsedStart > _nFreeStart )
	{
		memcpy( _pBuf+_nUsedStart+nSizeOffset, _pBuf+_nUsedStart, _nTotalSize-_nUsedStart);
		_nUsedStart += nSizeOffset;
	}
	_nFreeSize += nSizeOffset;
	_nTotalSize = nNewBufSize;		
}

// �������м�������
void MemoryQueue::_write(const Uint8* pInBuf, int nSize)
{
	//���пռ乻��
	if (nSize >= _nFreeSize)
	{
		// ����Ŀռ�̫����
		size_type nNewSize = 0;
		for( nNewSize = _nTotalSize; nNewSize+_nFreeSize < nSize; nNewSize += _nTotalSize)
		{
			//if( DataLimit<Uint32>::max_value() - nNewSize < _nTotalSize )	// �������ռ����DataLimit<Uint32>::max_value()
			//	LOGGER::Fatal("[MemoryQueue::_write]<%p> can not alloc memory, the size too larget.\r\n", this);
		}
		grow(_nTotalSize+nNewSize);
		assert(nSize <= _nFreeSize);
	};

	//������Ϊѭ��ʹ��,������������β��ʱ,���ݿ��ܳ���Ҫ�ֳ����δ�������
	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
	Uint32 nFirstCopySize = _nTotalSize - _nFreeStart;
	if (nFirstCopySize >= nSize)
	{
		memcpy(_pBuf+_nFreeStart, pInBuf, nSize);
		_nFreeStart += nSize;
	}
	else
	{
		memcpy(_pBuf+_nFreeStart, pInBuf, nFirstCopySize);
		memcpy(_pBuf, pInBuf+nFirstCopySize, nSize-nFirstCopySize);
		_nFreeStart = nSize-nFirstCopySize;
	};

	_nFreeSize -= nSize;

	// ����Ƿ����ڴ�й¶
#ifndef RV_UNCHECK_MEM
	assert( _nFreeStart != _nUsedStart );
	if( _nFreeStart > _nUsedStart )
		assert( _nFreeStart - _nUsedStart + _nFreeSize == _nTotalSize );
	else
		assert( _nTotalSize - _nUsedStart + _nFreeStart + _nFreeSize == _nTotalSize );
#endif // RV_UNCHECK_MEM
}

//=============================================================================================
int MemoryQueue::write(const Uint8* pInBuf, const NetInfo::MsgHead &sizeComplete, const HandleNet& hNet, bool bNetEvent)
{
	AUTOLOCK(this);

	DataHeader header;
	header._SizeComplete	= sizeComplete;
	header._SizeComplete._bNetEvent = bNetEvent;	//�Ƿ���NetEvent
	header._hNet			= hNet;
	_write((const Uint8*)(&header), sizeof(header) );
	_write(pInBuf, sizeComplete._nPackSize);
	
	return sizeComplete._nPackSize;
}


// pOutBuf��һ���㹻��Ŀռ䣬��nSize����С���ڴ�
void MemoryQueue::_read(Uint8* pOutBuf, int nSize)
{
	assert(_nFreeStart != _nUsedStart);

	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
	Uint32 nFirstCopySize = _nTotalSize - _nUsedStart;
	if(nFirstCopySize >= nSize) 
	{
		memcpy(pOutBuf, _pBuf + _nUsedStart, nSize);
		_nUsedStart += nSize;
	}
	else
	{
		memcpy(pOutBuf, _pBuf+_nUsedStart, nFirstCopySize);
		memcpy(pOutBuf+nFirstCopySize, _pBuf, nSize-nFirstCopySize);
		_nUsedStart = nSize - nFirstCopySize;
	};

	_nFreeSize += nSize;

	// ����Ƿ����ڴ�й¶(�߼��ϵ�)
#ifndef RV_UNCHECK_MEM
	if( _nFreeStart == _nUsedStart )
		return;
	if( _nFreeStart > _nUsedStart )
		assert( _nFreeStart - _nUsedStart + _nFreeSize == _nTotalSize );
	else
		assert( _nTotalSize - _nUsedStart + _nFreeStart + _nFreeSize == _nTotalSize );
#endif // _DEBUG
}

void MemoryQueue::_seek(int nSize)
{
	assert(_nFreeStart != _nUsedStart);
	//nFrirstCopySizeΪһ�ο���д���������ݴ�С
	Uint32 nFirstCopySize = _nTotalSize - _nUsedStart;
	if(nFirstCopySize >= nSize) 
	{
		_nUsedStart += nSize;
	}
	else
	{
		_nUsedStart = nSize - nFirstCopySize;
	};

	_nFreeSize += nSize;

	// ����Ƿ����ڴ�й¶(�߼��ϵ�)
#ifndef RV_UNCHECK_MEM
	if( _nFreeStart == _nUsedStart )
		return;
	if( _nFreeStart > _nUsedStart )
		assert( _nFreeStart - _nUsedStart + _nFreeSize == _nTotalSize );
	else
		assert( _nTotalSize - _nUsedStart + _nFreeStart + _nFreeSize == _nTotalSize );
#endif // _DEBUG
}

#if 0
// pOutBuf�Ĵ�СΪnSize
NetInfo::MsgHead MemoryQueue::read(Uint8* pOutBuf, int nOutBufSize, HandleNet& hNet)
{
	AUTOLOCK(this);
	if(_nFreeStart == _nUsedStart)
		return 0;

	// ȡ������ͷ
	DataHeader header;
	_read((Uint8*)(&header), sizeof(header) );	

	//���ܻ������Ƿ񹻿ռ�
	if (nOutBufSize < header._SizeComplete._nPackSize)
	{
		LOGGER::Error("[MemoryQueue::read]nOutBufSize(%d) < header._SizeComplete._nPackSize(%d)��\r\n", nOutBufSize, header._SizeComplete._nPackSize);
		LOGGER::Error("[MemoryQueue::read]This msg will be ignored.\r\n");
		_seek(header._SizeComplete._nPackSize);
		return 0;
	}

	// ��ȡ����
	_read(pOutBuf, header._SizeComplete._nPackSize);
	
	hNet = header._hNet;			//NetEvent, NEVENTTYPE_CONNECTFAILED��ϢhNet = 0;
	return header._SizeComplete;
}

#endif

int MemoryQueue::write(const void* pInBuf, int nSize)		//ֻд��nSize��С�����ݣ�������Netio��Ϣ
{
	AUTOLOCK(this);

	Sint32 nSizeWrite = nSize;
	_write((const Uint8*)(&nSizeWrite), sizeof(Sint32) );
	_write((const Uint8*)pInBuf, nSize);

	return nSize;
}

int MemoryQueue::read(void* pOutBuf, int nOutBufSize)			//ֻ����һ�����ݣ������ش�С��������Netio��Ϣ
{
	AUTOLOCK(this);
	if(_nFreeStart == _nUsedStart)
		return 0;

	// ȡ��������Ϣ
	Sint32 nSize = 0;
	_read((Uint8*)(&nSize), sizeof(Sint32));	

	//���ܻ������Ƿ񹻿ռ�
	if (nOutBufSize < nSize)
	{
		LOGGER::Error("[MemoryQueue::read]nOutBufSize(%d) < nSize(%d).\r\n", nOutBufSize, nSize);
		LOGGER::Error("[MemoryQueue::read]This msg will be ignored.\r\n");
		_seek(nSize);
		return 0;
	}

	// ��ȡ����
	_read((Uint8*)pOutBuf, nSize);
	return nSize;
}


int MemoryQueue::readAll(MsgBuffer &msgBuffer)
{
	msgBuffer._nUsed = 0;
	msgBuffer._nCurrPos = 0;

	AUTOLOCK(this);
	if(_nFreeStart == _nUsedStart)
		return 0;

	//ͳ����Ч��Ϣ���ݡ�
	NetInfo::MsgHead msg_head;
	Uint32 pos = _nUsedStart;
	while(1)
	{
		msg_head = *(NetInfo::MsgHead*)(_pBuf+pos);
		if(pos + sizeof(msg_head) > _nTotalSize)
		{
			Uint32 b0 = _nTotalSize - pos;
			unsigned char* p_msg_head_char =(unsigned char*)&msg_head;
			unsigned i  = 0;
			for(; i < b0; ++i)
				*(p_msg_head_char+i) = _pBuf[i+pos];
			for(; i < sizeof(msg_head)-b0; ++i)
				*(p_msg_head_char+i) = _pBuf[i];
		}

		pos += sizeof(DataHeader);
		if(pos > _nTotalSize)
			pos -= _nTotalSize;

		//test ʣ���������û��msg_head._nPackSize��ô��
		if(_nFreeStart > pos)
		{
			if(_nFreeStart - pos < msg_head._nPackSize)
				break;
		}
		else
		{
			if(_nTotalSize - pos + _nFreeStart < msg_head._nPackSize)
				break;
		}

		pos += msg_head._nPackSize;
		if(pos > _nTotalSize)
			pos -= _nTotalSize;
		if(pos >= _nFreeStart)
			break;
	}

	unsigned len = (pos>_nUsedStart ? pos-_nUsedStart : _nTotalSize-_nUsedStart+pos);
	if(len == 0)
		return 0;
	
	if(msgBuffer._nTotal < len)
	{
		msgBuffer._nTotal += (len/0x1000) * 0x1000 + 0x1000;
		msgBuffer._pBuf = (unsigned char*)msgBuffer.grow(msgBuffer._pBuf, len + msgBuffer._nTotal);
	}
	
	unsigned b0 = _nTotalSize - _nUsedStart;
	if(b0 < len)
	{
		memcpy(msgBuffer._pBuf, _pBuf+_nUsedStart, b0);
		memcpy(msgBuffer._pBuf + b0, _pBuf, len-b0);
	}
	else
	{
		memcpy(msgBuffer._pBuf, _pBuf+_nUsedStart, len);
	}

	_nFreeSize += len;
	_nUsedStart += len;
	if(_nUsedStart > _nTotalSize)
		_nUsedStart -= _nTotalSize;	
	if(_nUsedStart == _nFreeStart)		
		_nUsedStart = _nFreeStart = 0;	//��ȫ�ǿյ��ˣ�_nFreeSizeӦ�õ���_nTotalSize.

	msgBuffer._nUsed = len;
	return len;
}

// �����Ƿ�Ϊ��
int MemoryQueue::isEmpty()
{
	AUTOLOCK(this);
	int nEmpty = (_nFreeStart == _nUsedStart);
	return nEmpty;
}


MsgBuffer::MsgBuffer()
	: _pBuf(NULL), _nTotal(0), _nUsed(0), _nCurrPos(0)
{

}

MsgBuffer::~MsgBuffer()
{

}

NetInfo* MsgBuffer::fetch(NetTCP *pNetTCP, NetInfo::MsgHead &headMsg, void **pMsg)
{
	while (_nCurrPos < _nUsed)
	{
#if defined(ANDROID) || defined(__IOS__) 
		MemoryQueue::DataHeader head;
		memcpy(&head, _pBuf + _nCurrPos, sizeof(head));
		MemoryQueue::DataHeader *h = &head;
#else
		MemoryQueue::DataHeader *h = (MemoryQueue::DataHeader*)(_pBuf + _nCurrPos);
#endif
		headMsg = h->_SizeComplete;

		Uint8 *ret = _pBuf + _nCurrPos + sizeof(*h);
		_nCurrPos += sizeof(*h) + headMsg._nPackSize;

        HandleNet hNet;
        memcpy(&hNet, &h->_hNet, sizeof(hNet));
		NetInfo *pni = pNetTCP->getNetInfo(hNet);

		if (headMsg._bNetEvent && headMsg._nPackSize == 3)
		{
			//-- fixme: ֱ�Ӷ�ȡNetEventMsg��Android��IOS���ܻ�����ֽڶ������⣬�����ֱ���������������������������ƶ�������
			//-- ����ĿǰҪ�������������Դй¶�����⣬ֻ������˽����
			if (NEVENTTYPE_DESTROY == *(ret + 2))
			{
				pNetTCP->postDestroyNetInfo(hNet);
				continue;
			}
		}

		if (NULL == pni || pni->isProcessedAsBigPackg(&ret, headMsg))
			continue;

		*pMsg = ret;
		return pni;	
	}

	return NULL;
}