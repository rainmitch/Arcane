#ifndef ARCANE_ARCSIZE_H
#define ARCANE_ARCSIZE_H

#include <cstdint>

namespace arcane
{
	class ArcSize
	{
	public:
		ArcSize () : m_width (0), m_height (0) {}
		ArcSize (const size_t &width, const size_t &height)
		{
			this->m_width = width;
			this->m_height = height;
		}

		size_t width () const
		{
			return m_width;
		}

		void width (const size_t &newWidth)
		{
			m_width = newWidth;
		}

		size_t height () const
		{
			return m_height;
		}

		void height (const size_t &newHeight)
		{
			m_height = newHeight;
		}

	private:
		size_t m_width;
		size_t m_height;
	};

}

#endif //ARCANE_ARCSIZE_H
