#include "woap_singleton.h"

OwnedArray<Singleton::Deletable>* Singleton::instances = nullptr;

void Singleton::deleteInstances()
{
	if (instances != nullptr)
	{
		ScopedPointer<OwnedArray<Deletable>> bye(instances);
	}
}
