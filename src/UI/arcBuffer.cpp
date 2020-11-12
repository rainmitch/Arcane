
#include <arcane/UI/arcBuffer.h>
#include <algorithm>
#include <stdexcept>

namespace arcane
{
	ArcPixel::ArcPixel ()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 255;
	}

	ArcPixel::ArcPixel (const double &r, const double &g, const double &b, const double &a)
	{
		this->r = (unsigned char)(r * 255);
		this->g = (unsigned char)(g * 255);
		this->b = (unsigned char)(b * 255);
		this->a = (unsigned char)(a * 255);
	}

	ArcPixel::ArcPixel (const int &r, const int &g, const int &b, const int &a)
	{
		this->r = (unsigned char)(r);
		this->g = (unsigned char)(g);
		this->b = (unsigned char)(b);
		this->a = (unsigned char)(a);
	}

	int colorHexToDecInt (char a, char b)
	{
		int A = 0;
		int B = 0;
		if (a > '0' && a < '9')
		{
			A = a - '0';
		}
		else if (a >= 'a' && a < 'f')
		{
			A = 10 + a - 'a';
		}
		else if (a >= 'A' && a < 'F')
		{
			A = 10 + a - 'A';
		}

		if (b > '0' && b < '9')
		{
			B = b - '0';
		}
		else if (b >= 'a' && b < 'f')
		{
			B = 10 + b - 'a';
		}
		else if (b >= 'A' && b < 'F')
		{
			B = 10 + b - 'A';
		}


		return A * 16 + B;
	}

	ArcPixel::ArcPixel (const std::string &str)
	{
		std::string input = str;
		if (input[0] == '#')
			input = input.substr (1, input.size() - 1);

		if (input.size () != 6 && input.size () != 8)
		{
			throw std::runtime_error ("This is not a valid hex color code");
		}

		r = colorHexToDecInt (input[0], input[1]);
		g = colorHexToDecInt (input[2], input[3]);
		b = colorHexToDecInt (input[4], input[5]);

		if (input.size () == 8)
		{
			a = colorHexToDecInt (input[6], input[7]);
		}
		else
		{
			a = 255;
		}
	}

	ArcBuffer::ArcBuffer (const ArcSize &size) : m_size (size)
	{
		auto width = size.width ();
		auto height = size.height ();
		m_buffer = new ArcPixel[width * height];
		for (size_t i = 0; i < width * height; i++)
			m_buffer[i].a = 0;
		m_changed = true;
	}

	ArcBuffer::ArcBuffer (const ArcBuffer &buffer)
	{
		m_size = buffer.m_size;
		m_buffer = new ArcPixel[m_size.width () * m_size.height ()];
		for (size_t i = 0; i < m_size.width () * m_size.height (); i++)
			m_buffer[i].a = 0;
		std::copy (buffer.m_buffer, buffer.m_buffer + m_size.width () * m_size.height (), m_buffer);
		m_changed = true;
	}

	ArcBuffer::~ArcBuffer ()
	{
		delete m_buffer;
	}

	void ArcBuffer::size (const ArcSize &size)
	{
		m_size = size;
		delete m_buffer;
		m_buffer = new ArcPixel[size.width () * size.height ()];
		for (size_t i = 0; i < m_size.width () * m_size.height (); i++)
			m_buffer[i].a = 0;
		m_changed = true;
	}

	ArcSize ArcBuffer::size () const
	{
		return m_size;
	}

	void ArcBuffer::pixel (const ArcPoint &point, const ArcPixel &pixel)
	{
		m_buffer[point.y () * m_size.width () + point.x ()] = pixel;
	}

	ArcPixel ArcBuffer::pixel (const ArcPoint &point) const
	{
		return m_buffer[point.y () * m_size.width () + point.x ()];
	}

	void ArcBuffer::fill (const ArcPoint &a, const ArcPoint &b, const ArcPixel &fill)
	{
		size_t x = a.x ();
		size_t y = a.y ();
		size_t width = b.x () - x;
		size_t height = b.y () - y;

		for (size_t Y = y; Y < y + height; Y++)
		{
			for (size_t X = x; X < x + width; X++)
			{
				m_buffer[Y*m_size.width ()+X] = fill;
			}
		}
	}

	bool ArcBuffer::changed (const bool &reset)
	{
		bool v = m_changed;
		if (reset)
			m_changed = false;
		return v;
	}

	void ArcBuffer::fill (const ArcPoint &p, ArcBuffer *buffer)
	{
		size_t x = p.x ();
		size_t y = p.y ();
		size_t width = buffer->m_size.width ();
		size_t height = buffer->m_size.height ();

		for (size_t i = 0; i < width; i++)
		{
			for (size_t j = 0; j < height; j++)
			{
				m_buffer[(j+y)*m_size.width ()+(i+x)] = buffer->m_buffer[j*width+i];
			}
		}
	}

	ArcPixel *ArcBuffer::data () const
	{
		return m_buffer;
	}
}