/*!
 * \file C2engine.h
 * \date 2018/10/04 11:55
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \note
 * ��������ԭ�򣬱�hֻ�ܱ��ϲ�APP���룬Runtime�ڲ�����ֱ��ʹ�ã��������ڲ����е������
 *
*/

#ifndef C2ENGINE_H_
#define C2ENGINE_H_
namespace C2engine {
//==============================================================================

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
*        //���ı�ֻ����ʽ���ļ�
*        int ret = OpenFile("test.txt", "a");
* @endcode
* @see ����::ReadFile::CloseFile (��::����ָ�������ӹ���,���Կ��ĵ����CloseFile�����,�����������ת��CloseFile.)
* @deprecated���������ԭ������������ܻ��ڽ����İ汾��ȡ��
*/
#define C2ImportPackage(package)	\
	#include<package##.h>
//	#define C2##package
//
//#define C2Package(package)			\
//	#if !defined(C2##package)			\
//	#error		Invalid Package

//==============================================================================
//Events
//

//==============================================================================
}//namespace C2engine
#endif// C2ENGINE_H_
