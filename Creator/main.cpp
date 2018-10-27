#include<iostream>
#include <stdio.h>

#include<boost/log/trivial.hpp>

#include "../Runtime/c2PresentationCG/imgui/imgui.h"

#include"../Runtime/Metas/Part.h"
#include"../Runtime/c2Application.h"

#include"./GPanelAssets/GPanelAssets.h"
////////////////////////////////////////////////////////////////////////////////


class btAct4test : public BrainTree::Node {
	Status update() override {
		BOOST_LOG_TRIVIAL(info) << "  -> printed by BrainTree::Node.";
		return Node::Status::Success;
	}
};
class onUpdateFixFrame : public c2IAction {
public:
	onUpdateFixFrame() {
#if 1//just test
		auto repeater = std::make_shared<BrainTree::Repeater>(5);
		repeater->setChild(std::make_shared<btAct4test>());
		setRoot(repeater);
#endif
		_b_showsameline = 0;
	}
	int _b_showsameline;
	virtual Status update() {
		ImGui::Begin("C2 Director");
		ImGui::Text("NLP");
		if (ImGui::Button("open"))
			_b_showsameline++;
		if(_b_showsameline >=3 )
			ImGui::Text("more than 3 times.");
		ImGui::End();
		return BehaviorTree::update();
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitialized : public c2IAction {
	virtual Status update() {
		//TODO: I can plugin my extensions here.
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		return Status::Success;
	}
 };
 
////////////////////////////////////////////////////////////////////////////////
static int main() {
 //	bool b = C2RegistPartClass(GPanelAssets);
 //	if (!b)
 //		return 0;
 ////	c2::Part::ARPart ar = c2CreatePart("GPanelAssets");

	Uint32 syset_chunkoffet = 0;
 	onSysInitialized osi;
	c2asActSubEvt(osi, syset_chunkoffet +c2SysET::initialized,
		sizeof(c2SysEvt::initialized));

	onUpdateFixFrame ouff;
	c2asActSubEvt(ouff, syset_chunkoffet +c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	c2AppRun(true, 1, 1280, 720, "C2engine.Creator");
 
 	return 0;
 }
