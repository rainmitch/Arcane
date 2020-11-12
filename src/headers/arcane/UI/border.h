#ifndef ARCANE_BORDER_H
#define ARCANE_BORDER_H

#include <arcane/UI/arcBuffer.h>


namespace arcane
{
	class BorderProperties
	{
	public:
		void size (const unsigned int &size);
		unsigned int size ();

		void radius (const unsigned int &radius);
		unsigned int radius ();

		void color (const ArcPixel &color);
		ArcPixel color ();

	private:
		unsigned int m_size = 4;
		unsigned int m_radius = 15;
		ArcPixel m_color = DEFAULT_BACKGROUND;
	};
}

#endif //ARCANE_BORDER_H
