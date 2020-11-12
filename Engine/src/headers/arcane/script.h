
#ifndef ARCANE_SCRIPT_H
#define ARCANE_SCRIPT_H

namespace arcane
{
	class Object;
	class Script
	{
	public:
		virtual void Start ();
		virtual void Update ();

		virtual void onGUI ();

		Object *object = nullptr;
	};
}
#endif //ARCANE_SCRIPT_H
