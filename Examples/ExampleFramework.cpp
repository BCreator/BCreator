#include"../Runtime/Meta/Part.h"

int main() {
	bool b= C2RegistPartClass(PartClassTest0);
	Part *p = c2CreatePart("PartClassTest0");
	return 1;
}

# if 0
#include"../Runtime/c2engine.h"

C2ImportPackage(Event)


//==============================================================================
static void main() {
	//运行期
	Node root;

	GUID guid;
	Slot slot;
	Object *ptran= c2CreateObject(NULL, "PartTransform");
	root.plugPart(ptran, &slot);

	Node xiaoxing;
	xiaoxing
	ptran->plugPart(root);

	//==========================================================================
	int current_frame = 0;
	c2::Event::Event event;
	while (c2::Event::WaitEvent(&event))
	{
		switch (event._Type)
		{
		case EVENT_UPDATEFIX:
			root.updateFix(current_frame++);
			break;
		};
		root.draw(elapsed);
	}

	c2AssetDestory(p_space);
}

////粗颗粒度LOAD
//Asset space;
//space.load("d:/lab/TestRuntimeData/1.space");
#endif