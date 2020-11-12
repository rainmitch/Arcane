#ifndef ARCANE_ARCBUFFER_H
#define ARCANE_ARCBUFFER_H

#include <arcane/UI/arcSize.h>
#include <arcane/UI/arcPoint.h>

#include <iostream>

namespace arcane
{

	class ArcPixel
	{
	public:
		ArcPixel ();
		ArcPixel (const double &r, const double &g, const double &b, const double &a=1.0);
		ArcPixel (const int &r, const int &g, const int &b, const int &a=255);
		explicit ArcPixel (const std::string &string);

		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	template<typename T>
	inline T min (const T &a, const T &b)
	{
		return (a < b ? a : b);
	}

	template<typename T>
	inline ArcPixel operator* (const ArcPixel &pixel, const T &val)
	{
		return ArcPixel ((unsigned char)(pixel.r * val), (unsigned char)(pixel.g * val), (unsigned char)(pixel.b *
		val), pixel.a);
	}

	inline ArcPixel operator+ (const ArcPixel &pixel1, const ArcPixel &pixel2)
	{
		return ArcPixel (min (pixel1.r + pixel2.r, 255), min (pixel1.g + pixel2.g, 255), min (pixel1.b + pixel2.b,255),
				   min (pixel1.a + pixel2.a, 255));
	}

	typedef ArcPixel ArcColor;

	class ArcBuffer
	{
	public:
		explicit ArcBuffer (const ArcSize &size);
		ArcBuffer (const ArcBuffer &buffer);
		~ArcBuffer ();

		void size (const ArcSize &size);
		ArcSize size () const;

		void pixel (const ArcPoint &point, const ArcPixel &pixel);
		ArcPixel pixel (const ArcPoint &point) const;
		void fill (const ArcPoint &a, const ArcPoint &b, const ArcPixel &fill);
		void fill (const ArcPoint &p, ArcBuffer *buffer);

		bool changed (const bool &reset=false);

		ArcPixel *data () const;

	private:
		ArcPixel *m_buffer = nullptr;
		ArcSize m_size;
		bool m_changed;
	};
}

const arcane::ArcPixel DEFAULT_BACKGROUND = arcane::ArcPixel (255, 255, 250, 255);

#endif //ARCANE_ARCBUFFER_H
