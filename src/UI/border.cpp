
#include <arcane/UI/border.h>

namespace arcane
{
	void BorderProperties::size (const unsigned int &size)
	{
		m_size = size;
	}

	unsigned int BorderProperties::size ()
	{
		return m_size;
	}

	ArcPixel BorderProperties::color ()
	{
		return m_color;
	}

	void BorderProperties::color (const ArcPixel &color)
	{
		m_color = color;
	}

	unsigned int BorderProperties::radius ()
	{
		return m_radius;
	}

	void BorderProperties::radius (const unsigned int &radius)
	{
		m_radius = radius;
	}
}