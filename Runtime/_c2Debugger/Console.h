/*!
 * \file Node.h
 * \date 2018/10/04 12:29
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \brief
 *
 * 类似Unity3d的GameObject，用来粘贴各种零件（而Part，相当于Unity3D的Component）
 *
 * \note
 * 本类不允许被继承
*/

#ifndef C2_SPACETREE_NODE_H_
#define C2_SPACETREE_NODE_H_
namespace c2 {
//==============================================================================

class Node final : public Part {
	C2DefineClass(Node)
private:
	Node();
	~Node();

	//plugPart();

};

//==============================================================================
}//namespace c2
#endif// C2_SPACETREE_NODE_H_