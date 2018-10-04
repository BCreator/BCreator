#ifndef C2ENGINE__DEFINED_H_
#define C2ENGINE__DEFINED_H_
namespace C2engine {
//==============================================================================

#include<C2engine.h>

#define _C2Export

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
*        //包作者为用户定义包的导入宏
*        #define		C2ImportPackage-PartTerrain\
				C2ImportPackage(PartTerrain)\
*               _C2DependentPackage(RenderDevice)\
*               _C2DependentPackage(VFS)
*        ......
*        //用户使用此包的方法，在c/cpp中插入宏
*        C2ImportPackage-PartTerrain
* @endcode
* @see 函数::ReadFile::CloseFile (“::”是指定有连接功能,可以看文档里的CloseFile变成绿,点击它可以跳转到CloseFile.)
* @deprecated由于特殊的原因，这个函数可能会在将来的版本中取消
*/

#define _C2DependentPackage(package)	\
		#include<package##.h>

//==============================================================================
}//namespace C2engine
#endif// C2ENGINE__DEFINED_H_
