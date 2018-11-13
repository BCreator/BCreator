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

#ifndef C2_SPACETREE_C2NODE_H_
#define C2_SPACETREE_C2NODE_H_

#include"../../../c2Foundation/c2Part.h"

////////////////////////////////////////////////////////////////////////////////
class C2API c2Node final : public c2Part {
	C2DefineClass(c2Node)
private:
	explicit c2Node();
	virtual ~c2Node() override;
};

class c2Camera : public c2Part {
	c2Node*	_pNode;
};

////////////////////////////////////////////////////////////////////////////////
#endif// C2_SPACETREE_C2NODE_H_
