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

#ifndef C2ENGINE_NODE_H_
#define C2ENGINE_NODE_H_
namespace C2engine {
//==============================================================================

class Node : public Part
{
	C2DefineClass(Node)
private:
	Node();
	~Node();
};

//==============================================================================
}//namespace C2engine
#endif// C2ENGINE_NODE_H_
