#ifndef C2_TSMEQUEUE_H_
#define C2_TSMEQUEUE_H_

namespace c2{

//TODO: �ֽ���ƽ̨λ�����䣬���½����Ǹ�������ʽ�ڴ˱���������ʵ����ơ�
//void c2BufUniformAdapter()

////////////////////////////////////////////////////////////////////////////////
template<class _T, class _TSize = size_t>
struct Allocator {//TODO: ����������STL����BOOST�ġ�
	typedef _T			value_type;
	typedef value_type* pointer;
	typedef _TSize		size_type;
	// �����ڴ�
	inline pointer allocate(size_type sizeSize) {
		return (pointer)malloc(sizeSize * sizeof(value_type));
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
	// ����ܷ��������
	inline size_type max_size() const {
		size_type nCount = (size_type)(-1) / sizeof(value_type);
		return (nCount ? nCount : 1);
	}
};
/*============================================================================*/
class tsMemQueue final : Allocator<char> {
	using allocator_type= Allocator<char>;
	size_t		_sizeFreeStart;	//���еĿռ俪ʼ��ַ��ƫ����
	size_t		_sizeUsedStart;	//��ռ�õĿռ俪ʼ��ַ��ƫ����
	size_t		_sizeFreeSize;	//���ÿռ�	
	size_t		_sizeTotalSize;	//�ܿռ�
	void		*_pBuf;			//�ڴ�ָ��
	std::mutex	_Mutex;

	void grow(size_t sizeNewBufSize);
	void _push(const void *pInBuf, size_t sizeSize);		// �������м�������(����)
	void _pop(void *pOutBuf, size_t sizeSize);				// ��������(����)
	void _seek(size_t sizeSize);							// ����һ������
public:
	tsMemQueue(size_t sizeBufSize);	// ��ʼ��С
	~tsMemQueue();
	bool			isEmpty();		// �Ƿ�Ϊ��
	//ֻд��sizeSize��С������
	void			push(const void* pInBuf, size_t sizeSize);
	//ֻ����һ�����ݣ������ش�С��0Ϊʧ�ܡ�sizeOutBufSize�ǽ��ܻ�������С��
	size_t			pop(void *pOutBuf, size_t sizeOutBufSize);
};

////////////////////////////////////////////////////////////////////////////////
}//namespace c2
#endif//C2_TSMEQUEUE_H_

//private:
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