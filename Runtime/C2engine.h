/*!
 * \file C2engine.h
 * \date 2018/10/04 11:55
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \note
 * 根据依赖原则，本h只能被上层APP导入，Runtime内部不许直接使用，因已在内部集中导入过。
 *
*/

#ifndef C2ENGINE_H_
#define C2ENGINE_H_
namespace C2engine {
//==============================================================================

/**
* @brief 打开文件 \n
* 文件打开成功后，必须使用::CloseFile函数关闭
* @param[in] fileName    文件名
* @param[in] fileMode    文件模式，可以由以下几个模块组合而成：
*     -r 读取
*     -w 可写
*     -a 添加
*     -t 文本模式(不能与b联用)
*     -b 二进制模式(不能与t联用)
* @return 返回文件编号
*  --1表示打开文件失败(生成时:.-1)
* @note文件打开成功后，必须使用::CloseFile函数关闭
* @par 示例:
* @code
*        //用文本只读方式打开文件
*        int ret = OpenFile("test.txt", "a");
* @endcode
* @see 函数::ReadFile::CloseFile (“::”是指定有连接功能,可以看文档里的CloseFile变成绿,点击它可以跳转到CloseFile.)
* @deprecated由于特殊的原因，这个函数可能会在将来的版本中取消
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
