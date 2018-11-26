//#include<memory.h>
#include<malloc.h>

#include<boost/assert.hpp>

#include<glm/mat4x4.hpp>
#include<glm/gtc/matrix_transform.hpp>

#include"../c2Application.h"
#include"../c2PartController/c2PartCamera.h"
#include"../Render/Shader.h"
#include"../Render/Render.h"

#include<GLFW/glfw3.h>

#include"VoxelOctree.h"

//FACE SLICE

//1/////////////////////////////////////////////////////////////////////////////
const c2VNode* tc2BuildVoxelOctree() {

	return nullptr;
}

//1/////////////////////////////////////////////////////////////////////////////
static void _fixme2clousure(Uint8 &Mask, void** pChildren, size_t &nSize,//XXX: alternate reference2pointer rather than pointer2pointer
	const int nGeomType, const Uint8 CompareBit) {
	static Voxel* tv = nullptr;
	static c2VNode* tn = nullptr;
	if (C2_VOXGEOM_node == nGeomType) {
		Mask |= CompareBit;
		*pChildren = realloc(pChildren, nSize + sizeof(c2VNode));//XXX: use a memory pool to avoid fragmentation
		tn = reinterpret_cast<c2VNode*>(reinterpret_cast<Uint8>(*pChildren) + nSize);//get the pointer to the new node after add(realloc)
		/*TODO: iterate into children*/
		
	}
#ifdef TEST_VOXELBUILD	//可以不需要，Mask被清零过。
	else if (C2_VOXGEOM_none == nGeomType) {
		Mask &= (~CompareBit);
	}
#endif
	else if (C2_VOXGEOM_solid <= nGeomType) {
		*pChildren = realloc(pChildren, nSize + sizeof(Voxel));//XXX: use a memory pool to avoid fragmentation
		tv = reinterpret_cast<Voxel*>(reinterpret_cast<Uint8>(*pChildren) + nSize);//get the pointer to the new leaf after add(realloc)
		tv->_nGeomType = nGeomType;
	}
}
/*TODO: big-endian & little endian*/
void c2VNode::encodeChildren(	int UP1,	int UP2,	int UP3,	int UP4,
								int DOWN1,	int DOWN2,	int DOWN3,	int DOWN4) {
	size_t size = 0;
	_ChMask = 0x00;
	_fixme2clousure(_ChMask, &_Children, size, UP1, C2_VOXPOS_UP1);
	_fixme2clousure(_ChMask, &_Children, size, UP2, C2_VOXPOS_UP2);
	_fixme2clousure(_ChMask, &_Children, size, UP3, C2_VOXPOS_UP3);
	_fixme2clousure(_ChMask, &_Children, size, UP4, C2_VOXPOS_UP4);
	_fixme2clousure(_ChMask, &_Children, size, DOWN1, C2_VOXPOS_DOWN1);
	_fixme2clousure(_ChMask, &_Children, size, DOWN2, C2_VOXPOS_DOWN2);
	_fixme2clousure(_ChMask, &_Children, size, DOWN3, C2_VOXPOS_DOWN3);
	_fixme2clousure(_ChMask, &_Children, size, DOWN4, C2_VOXPOS_DOWN4);
}

void c2VNode::decodeChildren() {
	/*根据MASK来解读children里的数据，是一个tuple样的东西*/
}


/*2****************************************************************************/
static void DrawVoxel(const Render &Rr, const glm::mat4 &MatModel);
void c2VNode::draw(const Render &Rr) {
#ifdef TEST_VOXELBUILD
	BOOST_ASSERT(_ChMask);	//如果没有一个children的话，本Node就不应该存在。
	BOOST_ASSERT(getChildrenAmount());
#endif

	glm::mat4 tmat(1.0f);
	DrawVoxel(Rr, tmat);


// 	/*4-----------------------------------------------------------------------*/
// 	for (auto ch : _pChildren) {
// 		if (!ch)
// 			continue;
// 		ch->draw(Rr);
// 	}
}

#ifdef TEST_VOXELBUILD
int	c2VNode::getChildrenAmount() {
	int count = 0;
	if (_ChMask & C2_VOXPOS_UP1)
		++count;
	if (_ChMask & C2_VOXPOS_UP2)
		++count;
	if (_ChMask & C2_VOXPOS_UP3)
		++count;
	if (_ChMask & C2_VOXPOS_UP4)
		++count;
	if (_ChMask & C2_VOXPOS_DOWN1)
		++count;
	if (_ChMask & C2_VOXPOS_DOWN2)
		++count;
	if (_ChMask & C2_VOXPOS_DOWN3)
		++count;
	if (_ChMask & C2_VOXPOS_DOWN4)
		++count;
	return count;
}
#endif//TEST_VOXELBUILD

//1/////////////////////////////////////////////////////////////////////////////
static GLuint VBO = 0, vao_block = 0;
static Shader lightingShader;
static glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);//FIXME
static void _BuildVAOVoxel() {
#ifdef C2_USE_OPENGLES
	lightingShader.create("es3block.vs", "es3block.fs");
#else
	lightingShader.create("330block.vs", "330block.fs");
	lampShader.create("330lamp.vs", "330lamp.fs");
#endif//C2_USE_OPENGLES
	VBO = c2GetBoxVBOFloat();
	/*4----------------------------------------------------------------------*/
	//block
	glGenVertexArrays(1, &vao_block);
	glBindVertexArray(vao_block);
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
static void DrawVoxel(const Render &Rr, const glm::mat4 &MatModel) {
	if (0 == vao_block)
		_BuildVAOVoxel();
	/*4----------------------------------------------------------------------*/
	lightingShader.use();
	lightingShader.setVec3("lightPos", lightPos);
	lightingShader.setMat4("model", MatModel);

	lightingShader.setVec3("viewPos", Rr._PosView);
	lightingShader.setMat4("projection", Rr._MatProjection);
	lightingShader.setMat4("view", Rr._MatView);

	lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
	lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	glBindVertexArray(vao_block);
	glDrawArrays(GL_TRIANGLES, 0, 36);
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
