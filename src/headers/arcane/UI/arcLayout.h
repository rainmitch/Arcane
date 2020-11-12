#ifndef ARCANE_ARCLAYOUT_H
#define ARCANE_ARCLAYOUT_H

#include <vector>

#include <arcane/UI/arcPoint.h>
#include <arcane/UI/arcSize.h>
#include <arcane/UI/arcBuffer.h>

namespace arcane
{
	class ArcObject;

	class ArcLayout
	{
	public:
		ArcLayout () = default;

		void size (const ArcSize &size);
		ArcSize size ();

		ArcObject *parent ();
		void setParent (ArcObject *parent);

		void addObject (ArcObject *obj);

		bool propogateMouseClick (const int &x, const int y);
		bool propogateMouseHover (const int &x, const int y);

		void render (ArcBuffer &buffer);

	private:
		std::vector<ArcObject *> m_objects;

		ArcObject *m_parent = nullptr;

		ArcSize m_size;
	};

}

#endif //ARCANE_ARCLAYOUT_H
