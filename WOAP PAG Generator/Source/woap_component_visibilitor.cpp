#include "woap_component_visibilitor.h"

Array<ComponentVisibilitor*> ComponentVisibilitor::visibilitors = Array<ComponentVisibilitor*>();

ComponentVisibilitor::ComponentVisibilitor(Component* componentToHandle)
	: component(componentToHandle)
{
	component->setVisible(true);
	visibilitors.add(this);
}

ComponentVisibilitor::~ComponentVisibilitor()
{
	if (!active) return;

	visibilitors.removeFirstMatchingValue(this);

	/* Dont set visibility if there is another visibilitor referencing the same component */
	for (ComponentVisibilitor* other : ComponentVisibilitor::visibilitors)
	{
		if (other->component == component)
			return;
	}

	component->setVisible(false);
}
