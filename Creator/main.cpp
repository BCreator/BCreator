#include<iostream>
#include"../Runtime/Metas/Part.h"
#include"../Runtime/c2Application.h"

#include"./GPanelAssets/GPanelAssets.h"
////////////////////////////////////////////////////////////////////////////////

#include"../Runtime/c2Application.h"
class onSysInitialized : public c2IAction {
	class btAct : public BrainTree::Node {
		Status update() override {
			std::cout << "  -> printed by BrainTree::Node." << std::endl;
			return Node::Status::Success;
		}
	};
	BrainTree::BehaviorTree _BTree;
public:
	onSysInitialized() {
		auto repeater = std::make_shared<BrainTree::Repeater>(5);
		repeater->setChild(std::make_shared<btAct>());
		_BTree.setRoot(repeater);
	}
	virtual Status update(const c2IEvent &Evt, size_t EvtSize) {
		std::cout << "I can plugin my extensions here." << std::endl;
		_BTree.update();
		return c2IAction::update();
	}
};

static int main() {
//	bool b = C2RegistPartClass(GPanelAssets);
//	if (!b)
//		return 0;
////	c2::Part::ARPart ar = c2CreatePart("GPanelAssets");

	onSysInitialized osi;
	c2ActSubEvt(osi, c2GetSysEvtInitialized()._esTypeAddChunkOffset, sizeof(c2SysEvt::initialized));
	c2AppRun(false, 1);

	return 0;
}
