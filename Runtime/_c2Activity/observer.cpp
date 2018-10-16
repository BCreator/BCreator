#include"../Metas/Part.h"
#include"../c2Event.h"
////////////////////////////////////////////////////////////////////////////////

//static tsMemoryQueueEx	EventQueue;
//static std::q	EventQueue;

/*============================================================================*/
C2Interface void c2WaitEvent(c2Event *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
//	EventQueue.get
}

C2Interface void c2PumpEvent(c2Event *pEvent) {
	if (!pEvent)
		return;
	/*------------------------------------------------------------------------*/
}

/******************************************************************************/
C2Interface void c2SubscribeEvent(const c2::Part::ARPart &AR, c2EventType EType){
}

C2Interface void c2UnsubscribeEvent(const c2::Part::ARPart &AR, c2EventType EType) {

}

/******************************************************************************/
