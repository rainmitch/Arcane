#ifndef ARCANE_ARCBUTTON_H
#define ARCANE_ARCBUTTON_H

#include <arcane/UI/arcObject.h>

namespace arcane
{

	class ArcButton : public ArcObject
	{
	public:
		ArcButton ();

		void update() override;
	};
}

#endif //ARCANE_ARCBUTTON_H
