#include"../Runtime/Metas/Part.h"
#include"../Runtime/c2Event.h"
#include"../Runtime/c2Factory.h"

#include"./GPanelAssets/GPanelAssets.h"
////////////////////////////////////////////////////////////////////////////////

int main() {
	bool b = C2RegistPartClass(GPanelAssets);
	if (!b)
		return 0;
	c2::ARPart ar = c2CreatePart("GPanelAssets");
	c2ListenEvent(ar, c2ETRL_UPDATEFIX);

	/*------------------------------------------------------------------------*/
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
//		root.draw(elapsed);

	}
	return 0;
}
