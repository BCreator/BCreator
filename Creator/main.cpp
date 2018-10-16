#include<iostream>

#include<boost/signals2.hpp>

#include"../Runtime/Metas/Part.h"
#include"../Runtime/c2Event.h"
#include"../Runtime/c2Factory.h"

#include"./GPanelAssets/GPanelAssets.h"
////////////////////////////////////////////////////////////////////////////////

static void helloworld() {
	std::cout << "hello, w" << std::endl;
}

static int _main() {
	boost::signals2::signal<void ()>sig;
	sig.connect(&helloworld);

	sig();

	bool b = C2RegistPartClass(GPanelAssets);
	if (!b)
		return 0;
	c2::Part::ARPart ar = c2CreatePart("GPanelAssets");
	c2SubscribeEvent(ar, c2ETRL_UPDATEFIX);

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
