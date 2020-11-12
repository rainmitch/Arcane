//
// Created by Rain on 9/21/2020.
//

#ifndef ARCANE_ARCPOINT_H
#define ARCANE_ARCPOINT_H

#include <cstdint>

namespace arcane
{
	class ArcPoint
	{
	public:
		ArcPoint (const size_t &x=0, const size_t &y=0)
		{
			this->m_x = x;
			this->m_y = y;
		}

		size_t x () const
		{
			return m_x;
		}

		void x (const size_t &x)
		{
			m_x = x;
		}

		size_t y () const
		{
			return m_y;
		}

		void y (const size_t &y)
		{
			m_y = y;
		}

	private:
		size_t m_x;
		size_t m_y;
	};
}

#endif //ARCANE_ARCPOINT_H
