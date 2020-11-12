
#include <arcane/object.h>

namespace arcane
{
	Object::Object (const bool &renderableObject)
	{
		m_renderableObject = renderableObject;
	}

	Object::~Object ()
	{
		for (Script *script : m_scripts)
			delete script;
	}

	void Object::add (Script *script)
	{
		script->object = this;
		m_scripts.push_back (script);
	}

	bool Object::renderableObject ()
	{
		return m_renderableObject;
	}
}