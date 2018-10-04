#ifndef C2ENGINE__DEFINED_H_
#define C2ENGINE__DEFINED_H_
namespace C2engine {
//==============================================================================

#include<C2engine.h>

#define _C2Export

/**
* @brief ���ļ� \n
* �ļ��򿪳ɹ��󣬱���ʹ��::CloseFile�����ر�
* @param[in] fileName    �ļ���
* @param[in] fileMode    �ļ�ģʽ�����������¼���ģ����϶��ɣ�
*     -r ��ȡ
*     -w ��д
*     -a ���
*     -t �ı�ģʽ(������b����)
*     -b ������ģʽ(������t����)
* @return �����ļ����
*  --1��ʾ���ļ�ʧ��(����ʱ:.-1)
* @note�ļ��򿪳ɹ��󣬱���ʹ��::CloseFile�����ر�
* @par ʾ��:
* @code
*        //������Ϊ�û�������ĵ����
*        #define		C2ImportPackage-PartTerrain\
				C2ImportPackage(PartTerrain)\
*               _C2DependentPackage(RenderDevice)\
*               _C2DependentPackage(VFS)
*        ......
*        //�û�ʹ�ô˰��ķ�������c/cpp�в����
*        C2ImportPackage-PartTerrain
* @endcode
* @see ����::ReadFile::CloseFile (��::����ָ�������ӹ���,���Կ��ĵ����CloseFile�����,�����������ת��CloseFile.)
* @deprecated���������ԭ������������ܻ��ڽ����İ汾��ȡ��
*/

#define _C2DependentPackage(package)	\
		#include<package##.h>

//==============================================================================
}//namespace C2engine
#endif// C2ENGINE__DEFINED_H_
