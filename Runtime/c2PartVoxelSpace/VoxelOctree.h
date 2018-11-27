#pragma once

//#define TEST_VOXELBUILD

/*
TODO: 
- plotting scale.
- look up table LUT.
- draw: face slice.
- draw: invisible
*/

//1////////////////////////////////////////////////////////////////////////////
/*Right-handed System*/
const Uint8 C2_VOXPOS_None = 0x0;
const Uint8 C2_VOXPOS_Down1 = 0x01;//000
const Uint8 C2_VOXPOS_Down2 = 0x02;//100
const Uint8 C2_VOXPOS_Down3 = 0x04;//001
const Uint8 C2_VOXPOS_Down4 = 0x08;//101
const Uint8 C2_VOXPOS_Up1 = 0x10;//010
const Uint8 C2_VOXPOS_Up2 = 0x20;//110
const Uint8 C2_VOXPOS_Up3 = 0x40;//011
const Uint8 C2_VOXPOS_Up4 = 0x80;//111

enum {//Voxel geometry type related to geometry optimization
	C2_VOXGEOM_none,
	C2_VOXGEOM_node,//Just a logical concept, not a concrete voxel
	C2_VOXGEOM_solid,
	C2_VOXGEOM_transp,//specific density(alpha) depend on the material properties
	C2_VOXGEOM_typeammount
};

//1////////////////////////////////////////////////////////////////////////////
//XXX: 需要用二叉树加速么？
//TODO: 坍塌（合并）加速。
struct c2VNode {
	Uint8	_nGeomType : 4;
	Uint8	_NextSiblingPos : 4;
	Uint8	_ChMask;
	void*	_Children;
	size_t	_ChSize;

	/*4-----------------------------------------------------------------------*/
#ifdef TEST_VOXELBUILD
	int getChildrenAmount();
#endif
	/*if nChGeomType is not C2_VOXGEOM_node, MaterialID must not be 0*/
	void* addChild(const Uint8 nChGeomType, const Uint8 PosBit, const Uint8 MaterialID= 0);
	void encodeChildren(Uint8 GTypeUP1,		Uint8 GTypeUP2,
						Uint8 GTypeUP3,		Uint8 GTypeUP4,
						Uint8 GTypeDOWN1,	Uint8 GTypeDOWN2,
						Uint8 GTypeDOWN3,	Uint8 GTypeDOWN4);
	void reset() {
		_nGeomType = C2_VOXGEOM_none;
		_NextSiblingPos = C2_VOXPOS_None;
		_ChMask = 0x00;
		if (_Children) {
			free(_Children);
			_Children = nullptr;
		}
		_ChSize = 0;
	}

	/*4-----------------------------------------------------------------------*/
	void draw(const Render &Rr);//FIXME：以后改成cull of collector
};
