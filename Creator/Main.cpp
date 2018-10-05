#include"../Runtime/c2Event.h"
#include"../Runtime/Metas/Part.h"

#include"./GPanelAnimation/GPanelAnimation.h"

/************************************************************/
int main() {
	bool b = C2RegistPartClass(GPanelAnimation);
	if (!b)
		return 0;
	Part *p = c2CreatePart("GPanelAnimation");
	return 1;

	int current_frame = 0;
	c2Event te;
	while (true) {
//		c2PumpEvent(&te);
		c2WaitEvent(&te);
		switch (te._nType) {
		case c2ETKB_ESC:
			break;
		case c2ETRL_UPDATEFIX:
//			root.updateFix(current_frame++);
			break;
		}
		root.draw(elapsed);

	}
	return 0;
}
