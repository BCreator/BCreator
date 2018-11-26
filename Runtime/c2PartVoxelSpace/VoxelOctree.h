#pragma once

//#define TEST_VOXELBUILD

//1////////////////////////////////////////////////////////////////////////////
enum {//Voxel geometry type related to geometry optimization
	C2_VOXGEOM_none,
	C2_VOXGEOM_node,//Just a logical concept, not a concret voxel
	C2_VOXGEOM_solid,
	C2_VOXGEOM_transp,//specific density(alpha) depend on the material properties
	C2_VOXGEOM_typeammount
};

/*Right-handed System*/
const Uint8 C2_VOXPOS_UP1	= 0x80;
const Uint8 C2_VOXPOS_UP2	= 0x40;
const Uint8 C2_VOXPOS_UP3	= 0x20;
const Uint8 C2_VOXPOS_UP4	= 0x10;
const Uint8 C2_VOXPOS_DOWN1	= 0x08;
const Uint8 C2_VOXPOS_DOWN2	= 0x04;
const Uint8 C2_VOXPOS_DOWN3	= 0x02;
const Uint8 C2_VOXPOS_DOWN4	= 0x01;

//1////////////////////////////////////////////////////////////////////////////
struct Voxel {
	Uint8	_nGeomType;
	Uint8	_MaterialID;//The pallet id of color or material
};

//XXX: 需要用二叉树加速么？
struct c2VNode {
	Uint8	_ChMask		= 0x00;
	void*	_Children	= nullptr;

	/*4-----------------------------------------------------------------------*/
#ifdef TEST_VOXELBUILD
	int getChildrenAmount();
#endif
	void encodeChildren(int UP1, int UP2, int UP3, int UP4,
								int DOWN1, int DOWN2, int DOWN3, int DOWN4);
	void decodeChildren();

	/*4-----------------------------------------------------------------------*/
	void draw(const Render &Rr);//FIXME：以后改成cull of collector
};
