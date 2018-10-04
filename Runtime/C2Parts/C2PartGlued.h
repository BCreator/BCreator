/*!
 * \file Glued.h
 * \date 2018/10/04 12:29
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \brief 
 *
 * 类似Unity3d的GameObject，用来粘贴各种零件（Part，相当于Unity3D的Component）
 *
 * \note
 * 本类不允许被继承
*/

#ifndef C2ENGINE_GLUED_H_
#define C2ENGINE_GLUED_H_
namespace C2engine {
//==============================================================================

class Glued : public Part
{
	C2DefineClass(Glued)
private:
	Glued();
	~Glued();
};

//==============================================================================
}//namespace C2engine
#endif// C2ENGINE_GLUED_H_
