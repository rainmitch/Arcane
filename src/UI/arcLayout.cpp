
#include <arcane/UI/arcLayout.h>

#include <arcane/UI/arcObject.h>

#include <arcane/WindowRendering/window.h>

#include <stdexcept>
#include <iostream>

namespace arcane
{
	void ArcLayout::addObject (ArcObject *obj)
	{
		m_objects.push_back (obj);
		obj->setParent (this);
	}

	void ArcLayout::size (const ArcSize &size)
	{
		m_size = size;
	}

	ArcSize ArcLayout::size ()
	{
		return m_size;
	}

	void ArcLayout::render (ArcBuffer &buffer)
	{
		for (size_t i = 0; i < m_objects.size (); i++)
		{
			ArcBuffer *buff = m_objects[i]->render ();

			ArcPoint p = m_objects[i]->location ();
			ArcSize s = m_objects[i]->size ();
			ArcPoint flip = ArcPoint (p.x (), Window::size ().second - p.y () - s.height ());
			//buffer.fill (flip, buff);

			size_t x = flip.x ();
			size_t y = flip.y ();
			size_t width = buff->size ().width ();
			size_t height = buff->size ().height ();
			size_t myWidth = buffer.size ().width ();

			auto b = buffer.data ();
			auto c = buff->data ();

			for (size_t k = 0; k < width; k++)
			{
				for (size_t j = 0; j < height; j++)
				{
					ArcPixel pix = ArcPixel (c[j*width+k].r, c[j*width+k].g, c[j*width+k].b, 255);
					ArcPixel background = buffer.pixel (ArcPoint (x + k, y + j));
					background = ArcPixel (background.r, background.g, background.b, 255);
					float alpha = (float)(255 - c[j*width+k].a) / 255.0f;
					float invAlpha = 1.0f - alpha;
					if (parent () == nullptr)
					{
						b[(j+y)*myWidth+(k+x)] = c[j*width+k];
					}
					else
					{
						b[(j+y)*myWidth+(k+x)] = (pix * invAlpha) + (background) * alpha;
					}
				}
			}
		}
	}

	bool ArcLayout::propogateMouseClick (const int &x, const int y)
	{
		for (ArcObject *obj: m_objects)
		{
			ArcPoint position = obj->location ();
			ArcSize size = obj->size ();

			if ((x >= position.x () && x <= position.x () + size.width ()) &&
				(y >= position.y () && y <= position.y () + size.height ()))
			{
				if (!obj->layout ().propogateMouseClick (x, y))
				{
					if (obj->onClickFunction != nullptr)
						obj->onClickFunction ();

					if (!obj->focus ())
					{
						Window::currentFocus = obj;
						if (obj->onFocusFunction != nullptr)
							obj->onFocusFunction ();
					}
				}
				return true;
			}
		}

		return false;
	}

	bool ArcLayout::propogateMouseHover (const int &x, const int y)
	{
		for (ArcObject *obj: m_objects)
		{
			ArcPoint position = obj->location ();
			ArcSize size = obj->size ();

			if ((x >= position.x () && x <= position.x () + size.width ()) &&
				(y >= position.y () && y <= position.y () + size.height ()))
			{
				if (!obj->layout ().propogateMouseHover (x, y))
					if (obj->onHoverFunction != nullptr)
						obj->onHoverFunction ();
				return true;
			}
			else
			{
				if (obj->onNotHoverFunction != nullptr)
					obj->onNotHoverFunction ();
			}
		}

		return false;
	}

	ArcObject *ArcLayout::parent ()
	{
		return m_parent;
	}

	void ArcLayout::setParent (ArcObject *parent)
	{
		m_parent = parent;
	}
}