/*!
 * \file Node.h
 * \date 2018/10/04 12:29
 *
 * \author houstond
 * Contact tim@c2engine.com
 *
 * \brief
 *
 * ����Unity3d��GameObject������ճ�������������Part���൱��Unity3D��Component��
 *
 * \note
 * ���಻�����̳�
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