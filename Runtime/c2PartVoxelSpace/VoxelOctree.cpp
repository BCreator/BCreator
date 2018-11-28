#include<math.h>
#include<malloc.h>

#include<boost/assert.hpp>

#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"../c2Application.h"
#include"../c2PartController/c2PartCamera.h"
#include"../Render/Shader.h"
#include"../Render/Render.h"

#include<GLFW/glfw3.h>

#include<stb/stb_image.h>

#include"VoxelOctree.h"

//1/////////////////////////////////////////////////////////////////////////////
/*
- ����ֱ����ԼΪ10��10�η�mm��Լ��2��33-34�η����������32λ��ʾ�Ѿ��ܴ���.3��256��24�η���
- ��ȻLUT��ԭʼ���ݣ����Ծ�Ҫ���ǿռ�ʹ���Ч�����⡣
- TODO������õ����������Ժ��ǡ�
- ��EVE��������Ϸ���������������������ռ䣬�������ر����������ؿռ����Ϊ�ɾ��������
��֪�Ŀռ䡣��������������ϵ�����Ȳ�Ҫ����̫�ࡣ���ӵ��������������������磬���ӵ���Ϊ
������������������˼·�����������ܿ���Ϊһ������
- �����߸��
*/
/*XXX: Use bit filed. Use Repl.it to test*/
const Uint16 C2_LUTLEAVES_SIZE = 8;
//using NodeLUT = c2VNode[C2_LUTLEAVES_SIZE * C2_LUTLEAVES_SIZE * C2_LUTLEAVES_SIZE];
static int _getLUTPosition(int xMax, int yMax, int zMax, int x, int y, int z) {
	return y * zMax*xMax + z * xMax + x;//traverse from x->z->y(height)
};

/*LUT normal data order, x->y->z
TODO: center linear order like morton order? read and build from one center pointer in multi thread.*/
const c2VNode* c2BuildVoxelOctree(const c2VNode LeavesLUT[],
						const int xMax, const int yMax, const int zMax) {
	if (!LeavesLUT) {
		return nullptr;
	}
	/*4-----------------------------------------------------------------------*/
	/*define a lambda to collect one level into up-level of octree lut*/
	std::function<const c2VNode*(const c2VNode[], const int, const int, const int)> lbd_collect1level2uplut;
	lbd_collect1level2uplut = [&lbd_collect1level2uplut](const c2VNode _LutInput[],
								const int _xMax, const int _yMax, const int _zMax)->const c2VNode*{
		c2VNode* tpup_collectlut = new c2VNode[_xMax*_yMax*_zMax / 8];
		static int ilut[8], upi, i, count;
		for (int iy = 0; iy < _yMax; iy += 2) {
			for (int iz = 0; iz < _zMax; iz += 2) {
				for (int ix = 0; ix < _xMax; ix += 2) {//traverse from x->z->y(height)
					ilut[0] = _getLUTPosition(_xMax, _yMax, _zMax, ix,		iy,		iz);
					ilut[1] = _getLUTPosition(_xMax, _yMax, _zMax, ix + 1,	iy,		iz);
					ilut[2] = _getLUTPosition(_xMax, _yMax, _zMax, ix,		iy,		iz + 1);
					ilut[3] = _getLUTPosition(_xMax, _yMax, _zMax, ix + 1,	iy,		iz + 1);
					ilut[4] = _getLUTPosition(_xMax, _yMax, _zMax, ix,		iy + 1,	iz);
					ilut[5] = _getLUTPosition(_xMax, _yMax, _zMax, ix + 1,	iy + 1,	iz);
					ilut[6] = _getLUTPosition(_xMax, _yMax, _zMax, ix,		iy + 1,	iz + 1);
					ilut[7] = _getLUTPosition(_xMax, _yMax, _zMax, ix + 1,	iy + 1,	iz + 1);
					upi = _getLUTPosition(_xMax/2, _yMax/2, _zMax/2, ix/2, iy/2, iz/2);
					count = 0;
					static c2VNode tchildren[8];
					for (i = 0; i < 8; ++i) {
						if (_LutInput[ilut[i]]._nGType != C2_VOXGEOM_none) {//�ǿգ���ô�ϲ�LUT���ռ�
							tpup_collectlut[upi].cont._ChMask |= C2_VOXSLOT[i];
							tchildren[count] = _LutInput[ilut[i]];//copy from lut
							++count;
						}
					}
					if (!count) { 
						continue;
					}
					tpup_collectlut[upi]._nGType = C2_VOXGEOM_container;
					/*copy children from temp array*/
					tpup_collectlut[upi].cont._Children = new c2VNode[count];
					std::copy(tchildren, tchildren + count, tpup_collectlut[upi].cont._Children);
				}//x
			}//y
		}//z
		if (_xMax == 2) {
			BOOST_ASSERT(_xMax == _yMax && _xMax == _zMax);//my obsessive compulsive disorder
			return &(tpup_collectlut[0]);
		}
		else {//continue to recursive building
			return lbd_collect1level2uplut(tpup_collectlut, _xMax / 2, _yMax / 2, _zMax / 2);
		}
	};/*define lbd_collect1level2uplut lambda*/
	return lbd_collect1level2uplut(LeavesLUT, xMax, yMax, zMax);


	/*4-----------------------------------------------------------------------*/
	/*1)collapse all C2_VOXGEOM_none nodes.	2)and collapse long multi-layers
	container branch to be a path length value*/
//	return tp_nodelut;
}

void c2freeVoxelOctree(const c2VNode* pRoot) {
	if (pRoot)	delete pRoot;
}
void c2freeVoxelLUT(const c2VNode* pLUT) {
	if (pLUT)	delete[] pLUT;
}

const c2VNode* c2MakeVoxelLUTFromImage(int &xMax, int &yMax, int &zMax,
											const char* sFilePath) {
	if (!sFilePath)
		return nullptr;
	/*4-----------------------------------------------------------------------*/
	//Construct from the leaf level, the bottom 1st level
	int channel, tymax;
	Uint8* data = stbi_load(sFilePath, &xMax, &zMax, &channel, 1);//Just read one channel
	yMax = 256;
	xMax = xMax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : xMax;
	yMax = yMax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : yMax;
	zMax = zMax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : zMax;
	c2VNode* lut = new c2VNode[xMax*yMax*xMax];
	int i = 0, ilut = 0;
	for (int iy = 0; iy < yMax; iy += 1) {//XXX: need improve. it is KISS(keep it simple and stupid) temporarily.
		for (int iz = 0; iz < zMax; iz += 1) {
			for (int ix = 0; ix < xMax; ix += 1) {//traverse from x->z->y(height)
				i = iz * xMax + ix;
				tymax = data[i];//some position of lut will be empty. the c2VNode of this pos has been initialized with default value C2_VOXGEOM_none
				if (iy <= tymax) {
					ilut = _getLUTPosition(xMax, yMax, zMax, ix, iy, iz);
					lut[ilut]._nGType = C2_VOXGEOM_solid;
				}
			}
		}
	}
	stbi_image_free(data);
	return lut;
}
// 
// //1/////////////////////////////////////////////////////////////////////////////
// /*again: the order of children and slot mask is very strict!*/
// c2VNode* c2VNode::getChild(const int nSlot) const {
// 	if (nSlot < 0 || nSlot >= 8)
// 		return nullptr;
// 	if (!(cont._ChMask & C2_VOXSLOT[nSlot])) {
// 		return nullptr;
// 	}
// 	VNList::iterator &chi = cont._pChildren->begin();
// 	for (int i = 0; i < nSlot; ++i) {
// 		if (cont._ChMask & C2_VOXSLOT[i]) {
// 			++chi;
// 		}
// 	}
// 	return &(*chi);
// }
// 
// /*����汾���Լ��������ӡ����������Ǵ�Ҷ�����Ϲ��������ڸ�ǰ��ʹ����ˣ����Բ����ʡ�
// ��Ҷ�����Ϲ����ķ�ʽ���ʺ�ÿһ�㶼��һ��LUT��������������ֻ������ӵ�LUT ID��*/
// /*again: the order of children and slot mask is very strict!*/
// void c2VNode::encodeChildren(
// 	const Uint8 GTypeDown1,	const Uint8 GTypeDown2,
// 	const Uint8 GTypeDown3,	const Uint8 GTypeDown4,
// 	const Uint8 GTypeUp1,	const Uint8 GTypeUp2,
// 	const Uint8 GTypeUp3,	const Uint8 GTypeUp4,
// 	/*set material id only when leaf node*/
// 	const Uint8 MaterialIDDown1, const Uint8 MaterialIDDown2,
// 	const Uint8 MaterialIDDown3, const Uint8 MaterialIDDown4,
// 	const Uint8 MaterialIDUp1, const Uint8 MaterialIDUp2,
// 	const Uint8 MaterialIDUp3, const Uint8 MaterialIDUp4
// ) {
// 	/*std::function is too detailed. XXX: reduce the parameters' of lambda through [&]*/
// 	auto lambda_addchild= [this](const Uint8 bitSlot,
// 							const Uint8 nChGeomType, const Uint8 MaterialID)->c2VNode* {
// 		BOOST_ASSERT(C2_VOXGEOM_container == _nGType);
// 		if (nChGeomType < C2_VOXGEOM_container ||
// 			nChGeomType >= C2_VOXGEOM_typeammount) {//empty slot. should collapse.
// 			cont._ChMask &= (~bitSlot);
// 			return nullptr;
// 		}
// 		c2VNode* pnewchild = nullptr;
// 		/*get the child slot. and reset it & ensure it's Geometry Type.*/
// 		if (cont._ChMask & bitSlot) {//if exist already, just refill the data
// 			BOOST_ASSERT(cont._pChildren);//assert that pChildren exist already.
// 			pnewchild = getChild(bitSlot);
// 			BOOST_ASSERT(pnewchild);//assert the slot is exist already.
// 			pnewchild->reset();
// 		}
// 		else {
// 			if (!cont._pChildren) {//the first child
// 				cont._pChildren = new VNList;
// 				BOOST_ASSERT(!cont._ChMask);//assert that children mask is empty
// 			}
// 			cont._ChMask |= bitSlot;
// 			cont._pChildren->push_back(nChGeomType);
// 			pnewchild = &(cont._pChildren->back());
// 			BOOST_ASSERT(pnewchild);
// 		}
// 		/*fill the child material into the slot if leaf. next sibling does NOT be set */
// 		pnewchild->_nGType = nChGeomType;
// 		if (pnewchild->_nGType != C2_VOXGEOM_container) {
// 			pnewchild->leaf._MaterialID = MaterialID;
// 		}
// 		BOOST_ASSERT(nChGeomType==pnewchild->_nGType);//not need. just my obsessive compulsive disorder
// 		return pnewchild;
// 	};//define lambda_addchild
// 	reset();
// 	_nGType = C2_VOXGEOM_container;
// 	/*again: the order is very strict!*/
// 	lambda_addchild(C2_VOXSLOT_BitDown1, GTypeDown1, MaterialIDDown1);
// 	lambda_addchild(C2_VOXSLOT_BitDown2, GTypeDown2, MaterialIDDown2);
// 	lambda_addchild(C2_VOXSLOT_BitDown3, GTypeDown3, MaterialIDDown3);
// 	lambda_addchild(C2_VOXSLOT_BitDown4, GTypeDown4, MaterialIDDown4);
// 	lambda_addchild(C2_VOXSLOT_BitUp1, GTypeUp1, MaterialIDUp1);
// 	lambda_addchild(C2_VOXSLOT_BitUp2, GTypeUp2, MaterialIDUp2);
// 	lambda_addchild(C2_VOXSLOT_BitUp3, GTypeUp3, MaterialIDUp3);
// 	lambda_addchild(C2_VOXSLOT_BitUp4, GTypeUp4, MaterialIDUp4);
// }

//1/////////////////////////////////////////////////////////////////////////////
static bool g_bDirtyFirst = true;
static GLuint VBO = 0, vao_voxel = 0;
static Shader lightingShader;
static glm::vec3 lightPos = glm::vec3(12.0f, 10.0f, 20.0f);//FIXME
static void _BuildVAOVoxel() {
#ifdef C2_USE_OPENGLES
	lightingShader.create("es3voxel.vs", "es3voxel.fs");
#else
	lightingShader.create("330voxel.vs", "330voxel.fs");
#endif//C2_USE_OPENGLES
	VBO = c2GetBoxVBOFloat();
	/*4----------------------------------------------------------------------*/
	//voxel
	glGenVertexArrays(1, &vao_voxel);
	glBindVertexArray(vao_voxel);
#ifdef USE_INTEGER
	glVertexAttribIPointer(0, 3, GL_INT, 6 * sizeof(GLint), (void*)0);
#else
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
#endif
	glEnableVertexAttribArray(0);
	// normal attribute
#ifdef USE_INTEGER
	glVertexAttribIPointer(1, 3, GL_INT, 6 * sizeof(GLint), (void*)(3 * sizeof(GLint)));
#else
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
#endif
	glEnableVertexAttribArray(1);
}
/*2****************************************************************************/
void c2VNode::draw(const Render &Rr) const {
	if (0 == vao_voxel)
		_BuildVAOVoxel();
	BOOST_ASSERT(_nGType < C2_VOXGEOM_typeammount);
	/*4----------------------------------------------------------------------*/
	lightingShader.use();
	lightingShader.setVec3("lightPos", lightPos);

	lightingShader.setVec3("viewPos", Rr._PosView);
	lightingShader.setMat4("projection", Rr._MatProjection);
	lightingShader.setMat4("view", Rr._MatView);

	lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	/*4----------------------------------------------------------------------*/
	glm::mat4 mat(1.0f);
	std::function<void(const c2VNode&, const glm::mat4 &)> lambda_draw;
	lambda_draw = [&lambda_draw](const c2VNode& Node, const glm::mat4& MatModel) {
		if (Node._nGType == C2_VOXGEOM_container) {
			/*FIXME: after we do the process of collapsing, we can assert it!*/
			//BOOST_ASSERT(Node.cont._pChildren);
			if (!Node.cont._Children) {
				return;
			}
			int count = 0;
			for (int i = 0; i < 8; ++i) {
				//			MatModel = translate(MatModel, );
				if (Node.cont._ChMask&C2_VOXSLOT[i]) {
					lambda_draw(Node.cont._Children[count], MatModel);
					++count;
				}
			}
		}
		else {
			lightingShader.setMat4("model", MatModel);
			glBindVertexArray(vao_voxel);
			//	glDrawArrays(GL_TRIANGLES, 0, 36);
			//	glEnable(GL_PROGRAM_POINT_SIZE);
			 //	glPointSize(50);
			//	glDrawArrays(GL_POINTS, 0, 36);
			glDrawArrays(GL_LINES, 0, 36);
		}
	};
	lambda_draw(*this, mat);

#if 0//tmp test
	int _xMax, _zMax, channel, ymax;
	static Uint8* g_pImageData = nullptr;
	if (!g_pImageData) {
		g_pImageData = stbi_load("d:/qjf2017.png", &_xMax, &_zMax, &channel, 1);//Just read one channel
		_xMax = _xMax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : _xMax;
		_zMax = _zMax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : _zMax;
	}
	glBindVertexArray(vao_voxel);
	for (int ix = 0; ix < _xMax; ix += 1)	for (int iz = 0; iz < _zMax; iz += 1) {
		ymax = ((Uint8*)g_pImageData)[ix * _xMax + iz];
		ymax = ymax > C2_LUTLEAVES_SIZE ? C2_LUTLEAVES_SIZE : ymax;
		for (int iy = 0; iy < ymax; iy += 1) {
			lightingShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(ix*1.0f, iy*1.0f, iz*1.0f)));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
	return;
#endif
}

static unsigned int default_palette[256] = {
	0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
	0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
	0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
	0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
	0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
	0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
	0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
	0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
	0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
	0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
	0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
	0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
	0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
	0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
	0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
	0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
};

////////////////////////////////////////////////////////////////////////////////
/*
 *           v4 +----------e4---------+ v5
 *             /.                    /|
 *            / .                   / |
 *          e7  .                 e5  |                    +-----------+
 *          /   .                 /   |                   /           /|
 *         /    .                /    |                  /   f1      / |  <f2
 *     v7 +----------e6---------+ v6  |                 +-----------+  |
 *        |     .               |     e9            f5> |           |f4|
 *        |     e8              |     |                 |           |  |
 *        |     .               |     |                 |    f3     |  +
 *        |     .               |     |                 |           | /
 *        |  v0 . . . .e0 . . . | . . + v1              |           |/
 *       e11   .                |    /                  +-----------+
 *        |   .                e10  /                         ^
 *        |  e3                 |  e1                         f0
 *        | .                   | /
 *        |.                    |/
 *     v3 +---------e2----------+ v2
 *
 */
