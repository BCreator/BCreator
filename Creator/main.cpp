#include"../Runtime/Metas/Part.h"
#include"../Runtime/c2Application.h"

#include"./GPanelAssets/GPanelAssets.h"
////////////////////////////////////////////////////////////////////////////////

static int main1() {
	bool b = C2RegistPartClass(GPanelAssets);
	if (!b)
		return 0;
	c2::Part::ARPart ar = c2CreatePart("GPanelAssets");

	/*------------------------------------------------------------------------*/
	int current_frame = 0;
	c2IEvent te;
	while (true) {
////		c2PumpEvent(&te);
//		c2WaitEvent(&te);
//		switch (te._nType) {
//		case c2ETKB_ESC:
//			break;
//		case c2ETRL_UPDATEFIX:
////			root.updateFix(current_frame++);
//			break;
//		}
////		root.draw(elapsed);

	}
	return 0;
}
