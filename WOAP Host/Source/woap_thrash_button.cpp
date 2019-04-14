#include "woap_thrash_button.h"
#include "woap_self_deletable.h"

using namespace WOAP;
using namespace WOAP::GUI;

ThrashButton::ThrashButton()
	: IconButton(FontAwesome_Trash)
{

}

ThrashButton::~ThrashButton()
{
}

bool ThrashButton::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
	return nullptr != dynamic_cast<const SelfDeletable*>(dragSourceDetails.sourceComponent.get());
}

void ThrashButton::itemDragEnter(const SourceDetails&)
{
	setToggleState(true, NotificationType::dontSendNotification);
}

void ThrashButton::itemDropped(const SourceDetails& dragSourceDetails)
{	
	if (const SelfDeletable* selfDeletable = dynamic_cast<const SelfDeletable*>(dragSourceDetails.sourceComponent.get()))
	{
		selfDeletable->selfDelete(); 
	}

	setEnabled(false);
	setToggleState(false, NotificationType::dontSendNotification);
}

void ThrashButton::itemDragExit(const SourceDetails&)
{
	setToggleState(false, NotificationType::dontSendNotification);
}
