#pragma once


namespace WOAP
{
	class SelfDeletable
	{
	public:
		virtual void selfDelete() const = 0;
	};
}