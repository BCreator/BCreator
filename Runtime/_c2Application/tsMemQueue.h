#ifndef C2_TXMEMORYQUEUE_H_
#define C2_TXMEMORYQUEUE_H_

//TODO: ��ȷ���ȵ���������Ӧ�ý������ڱ�����������ú���������޸�һ�¡��������README.md
#ifdef _MSC_VER
using c2BufLen= unsigned __int64;//32λ��64��size_t������ȷ���ʶ������ȷ��С�Ļ�����Ϣ����ֵ
typedef __int8 Sint8;
typedef unsigned __int8 Uint8;
typedef __int16 Sint16;
typedef unsigned __int16 Uint16;
typedef __int32 Sint32;
typedef unsigned __int32 Uint32;
typedef __int64 Sint64;
typedef unsigned __int64 Uint64;
#else
using c2BufLen64 = unsigned long long;//32λ��64��size_t������ȷ���ʶ������ȷ��С����Ϣ����ֵ
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
//		void	*_pBuf;		//�ڴ�ָ��
//		size_t	_nTotal, _nUsed;
//		size_t	_nCurrPos;
//	public:
//		MsgBuffer() : _pBuf(nullptr), _nTotal(0), _nUsed(0), _nCurrPos(0) {}
//		~MsgBuffer() {}
//	};
//	struct esMsgHead {
//		union {
//			size_t	  _esSize;	//����ֵ��ʾ��������Ҵ˰����ǰ��Ľ�β��FIXME
//			struct {
//				size_t		_esPackSize : 30;	//��С
//				//0: ��ʾ�������ķ��; 1:��ʾ��ϵͳ������NetEvent,�����ܣ���MsgType = 0��
//				size_t		_esNetEvent : 1;
//				//0: ��ʾ�ǽ�β�������ݽ�����ϣ� 1��ʾ��������Ҵ˰����ǽ�β������Ҫ����
//				//�ȴ����µķ��
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

//TODO: �ֽ���ƽ̨λ�����䣬���½����Ǹ�������ʽ�ڴ˱���������ʵ����ơ�
//void c2BufUniformAdapter()

////////////////////////////////////////////////////////////////////////////////
class tsMemoryQueue : Allocator<char> {
	using allocator_type= Allocator<char>;
	size_t		_sizeFreeStart;	//���еĿռ俪ʼ��ַ��ƫ����
	size_t		_sizeUsedStart;	//��ռ�õĿռ俪ʼ��ַ��ƫ����
	size_t		_sizeFreeSize;		//���ÿռ�	
	size_t		_sizeTotalSize;	//�ܿռ�
	void		*_pBuf;			//�ڴ�ָ��
	std::mutex	_Mutex;

	void grow(size_t nNewBufSize);
	void _write(const void *pInBuf, size_t sizeSize);		// �������м�������(����)
	void _read(void *pOutBuf, size_t sizeSize);			// ��������(����)
	void _seek(size_t sizeSize);							// ����һ������
public:
	tsMemoryQueue(size_t sizeBufSize);	// ��ʼ��С
	~tsMemoryQueue();
	bool		isEmpty();		// �Ƿ�Ϊ��
	//ֻд��sizeSize��С�����ݣ�������Netio��Ϣ
	void		write(const void* pInBuf, size_t sizeSize);
	//ֻ����һ�����ݣ������ش�С��������Netio��Ϣ
	bool		read(void *pOutBuf, size_t sizeOutBufSize);
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_TXMEMORYQUEUE_H_
