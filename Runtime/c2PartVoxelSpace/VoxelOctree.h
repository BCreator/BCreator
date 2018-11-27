#pragma once

//#define TEST_VOXELBUILD

/*
- TODO: plotting scale.
- TODO: look up table LUT.
- TODO: draw: face slice.
- TODO: draw: invisible
- XXX: 需要用二叉树加速么？
- TODO: 坍塌（合并）加速。
- FIXME: ?big-endian & little endian
*/

//1////////////////////////////////////////////////////////////////////////////
/*Right-handed System*/
//const Uint8 C2_VOXSLOT_BitNone = 0x0;
const Uint8 C2_VOXSLOT_BitDown1 = 0x01;	// 0 0 0
const Uint8 C2_VOXSLOT_BitDown2 = 0x02;	//+1 0 0
const Uint8 C2_VOXSLOT_BitDown3 = 0x04;	// 0 0+1
const Uint8 C2_VOXSLOT_BitDown4 = 0x08;	//+1 0+1
const Uint8 C2_VOXSLOT_BitUp1 = 0x10;	// 0+1 0
const Uint8 C2_VOXSLOT_BitUp2 = 0x20;	//+1+1 0
const Uint8 C2_VOXSLOT_BitUp3 = 0x40;	// 0+1+1
const Uint8 C2_VOXSLOT_BitUp4 = 0x80;	//+1+1+1

/*again: the order of children and slot mask is very strict!*/
const int C2_VOXSLOT[8] = {
	C2_VOXSLOT_BitDown1,
	C2_VOXSLOT_BitDown2,
	C2_VOXSLOT_BitDown3,
	C2_VOXSLOT_BitDown4,
	C2_VOXSLOT_BitUp1,
	C2_VOXSLOT_BitUp2,
	C2_VOXSLOT_BitUp3,
	C2_VOXSLOT_BitUp4
};

enum {//Voxel geometry type related to geometry optimization
	C2_VOXGEOM_none= 0,
	C2_VOXGEOM_container,//Just a logical container concept, not a concrete voxel
	C2_VOXGEOM_solid,
	C2_VOXGEOM_transp,//specific density(alpha) depend on the material properties
	C2_VOXGEOM_typeammount
};

//1////////////////////////////////////////////////////////////////////////////
struct c2VNode;
const c2VNode* c2BuildVoxelOctree(const c2VNode LeavesLUT[],
						const int xMax, const int yMax, const int zMax);
void c2freeVoxelOctree(const c2VNode* pRoot);
const c2VNode* c2MakeVoxelLUTFromImage(int &xMax, int &yMax, int &zMax,
					const char* sFilePath);
void c2freeVoxelLUT(const c2VNode* pLUT);


//1////////////////////////////////////////////////////////////////////////////
struct c2VNode {
	void draw(const Render &Rr) const;//FIXME：以后改成cull of collector
	~c2VNode() {
		reset();
	}
	using VNList = std::list<c2VNode>;//?why can't put in cpp with just a "class VNList" in h.
//	c2VNode* getChild(const int nSlot) const;
	/*GType is Geometry type of the slot(could be none when empty slot).
	if not a leaf, material id will be ignored.*/
// 	void encodeChildren(
// 		const Uint8 GTypeDown1, const Uint8 GTypeDown2,
// 		const Uint8 GTypeDown3, const Uint8 GTypeDown4,
// 		const Uint8 GTypeUp1, const Uint8 GTypeUp2,
// 		const Uint8 GTypeUp3, const Uint8 GTypeUp4,
// 		/*set material id only when leaf node. No.0 is default to debug*/
// 		const Uint8 MaterialIDDown1 = 0, const Uint8 MaterialIDDown2 = 0,
// 		const Uint8 MaterialIDDown3 = 0, const Uint8 MaterialIDDown4 = 0,
// 		const Uint8 MaterialIDUp1 = 0, const Uint8 MaterialIDUp2 = 0,
// 		const Uint8 MaterialIDUp3 = 0, const Uint8 MaterialIDUp4 = 0
// 	);

	/*4-----------------------------------------------------------------------*/
	c2VNode(const Uint8 nGeomType= C2_VOXGEOM_none) {
		reset();
		_nGType = nGeomType;
	}
	void reset() {
 		if (_nGType == C2_VOXGEOM_container && cont._Children) {
 			delete[] cont._Children;
 		}
 		memset(this, 0, sizeof(c2VNode));
 	}
	/*2************************************************************************/
/*data member*/
// 	Uint8	_nGType : 4; //Geometry type
// 	Uint8	_nLength2Root:4;
	Uint8	_nGType; //XXX: use bit filed. Geometry type
	union {
		struct {
			Uint8		_ChMask;//Children slots mask
			c2VNode*	_Children;//impact children list indicated by mask
		}cont;//container
		struct {
			Uint8	_MaterialID;//The material of No.0 is default to debug
		}leaf;
	};
	friend const c2VNode* c2BuildVoxelOctree(const c2VNode LeavesLUT[],
								const int xMax, const int yMax, const int zMax);
	friend void c2freeVoxelOctree(const c2VNode* pRoot);
	friend const c2VNode* c2MakeVoxelLUTFromImage(int &xMax, int &yMax, int &zMax,
								const char* sFilePath);
	friend void c2freeVoxelLUT(const c2VNode* pLUT);
};
