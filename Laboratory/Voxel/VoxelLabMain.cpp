#include<iostream>
#include <stdio.h>

#include<boost/log/trivial.hpp>

#include"../../Runtime/c2Foundation/c2Part.h"
#include"../../Runtime/c2PreDefined.h"
#include"../../Runtime/c2DefEvent.h"
#include"../../Runtime/c2Application.h"

#include"VoxGame.h"

////////////////////////////////////////////////////////////////////////////////
static VoxGame* pVoxGame;

////////////////////////////////////////////////////////////////////////////////
class btAct4test : public BrainTree::Node {
	Status update() override {
		BOOST_LOG_TRIVIAL(info) << "  -> printed by BrainTree::Node.";
		return Node::Status::Success;
	}
};
static void ShowExampleAppCustomNodeGraph(bool* opened);
class onUpdateFixFrameVoxel : public c2IAction {
public:
	int _b_showsameline;
	onUpdateFixFrameVoxel() {
#if 1//just test
		auto repeater = std::make_shared<BrainTree::Repeater>(5);
		repeater->setChild(std::make_shared<btAct4test>());
		setRoot(repeater);
#endif
		_b_showsameline = 0;
	}
	virtual Status update() {
		ImGui::Begin("C2 Director");
		ImGui::Text("NLP");
		if (ImGui::Button("open"))
			_b_showsameline++;
		if (_b_showsameline >= 3)
			ImGui::Text("more than 3 times.");
		ImGui::SameLine();
		ImGui::End();
		return BehaviorTree::update();
	}
};

class onUpdateFrameVoxel : public c2IAction {
public:
	virtual Status update() {
		static const c2SysEvt::updateframe*  tpevt;
		BOOST_ASSERT(_pEvt);
		tpevt = static_cast<const c2SysEvt::updateframe*>(_pEvt);
		pVoxGame->Update(static_cast<int>(tpevt->_esElapsed));
		pVoxGame->PreRender();
		pVoxGame->Render();
		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
class onSysInitializedVoxel : public c2IAction {
	virtual Status update() {
		//TODO: I can plugin my extensions here.
		BOOST_LOG_TRIVIAL(info) << "C2engine intialized.";
		pVoxGame = VoxGame::GetInstance();
		pVoxGame->Init(1024, 768, 32, 8);

		return Status::Success;
	}
};

////////////////////////////////////////////////////////////////////////////////
int main_VoxelLab() {
	Uint32 syset_chunkoffet = 0;
	onSysInitializedVoxel osi;
	c2asActSubEvt(osi, syset_chunkoffet + c2SysET::initialized,
		sizeof(c2SysEvt::initialized));

	onUpdateFixFrameVoxel ouff;
	c2asActSubEvt(ouff, syset_chunkoffet + c2SysET::updatefixframe,
		sizeof(c2SysEvt::updatefixframe));

	onUpdateFrameVoxel ouf;
	c2asActSubEvt(ouf, syset_chunkoffet + c2SysET::updateframe,
		sizeof(c2SysEvt::updateframe));

	c2AppRun(true, 1, 1024, 768, "C2engine.Creator");

	return 0;
}
