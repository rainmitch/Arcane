
#include <arcane/UI/arcObject.h>

#include <arcane/WindowRendering/window.h>

namespace arcane
{
	void ArcEvents::onClick (const std::function<void ()> &func)
	{
		onClickFunction = func;
	}

	void ArcEvents::onHover (const std::function<void ()> &func)
	{
		onHoverFunction = func;
	}

	void ArcEvents::onNotHover (const std::function<void ()> &func)
	{
		onNotHoverFunction = func;
	}

	void ArcEvents::onFocus (const std::function<void ()> &func)
	{
		onFocusFunction = func;
	}

	bool ArcEvents::focus ()
	{
		return Window::currentFocus == this;
	}

	ArcObject::ArcObject () : m_buffer (ArcBuffer ({0, 0}))
	{
		m_layout.setParent (this);
	}

	ArcObject::~ArcObject ()
	{
		delete xContrainst;
		delete yContrainst;
	}

	void ArcObject::location (const ArcPoint &point)
	{
		m_location = point;
	}

	ArcPoint ArcObject::location ()
	{
		return m_location;
	}

	void ArcObject::size (const ArcSize &size)
	{
		m_buffer.size (size);
		update ();
	}

	ArcSize ArcObject::size ()
	{
		return m_buffer.size ();
	}

	void ArcObject::update ()
	{

	}

	ArcBuffer &ArcObject::buffer ()
	{
		return m_buffer;
	}

	ArcLayout &ArcObject::layout ()
	{
		return m_layout;
	}

	bool ArcObject::changed (const bool &reset)
	{
		return m_buffer.changed (reset);
	}

	void ArcObject::x (Constraint *constraint)
	{
		delete xContrainst;
		xContrainst = constraint;
	}

	void ArcObject::y (Constraint *constraint)
	{
		delete yContrainst;
		yContrainst = constraint;
	}

	void ArcObject::width (Constraint *constraint)
	{
		delete wConstraint;
		wConstraint = constraint;
	}

	void ArcObject::height (Constraint *constraint)
	{
		delete hConstraint;
		hConstraint = constraint;
	}

	void ArcObject::backgroundColor (const ArcPixel &arcPixel)
	{
		m_backgroundColor = arcPixel;
	}

	ArcPixel ArcObject::backgroundcolor ()
	{
		return m_backgroundColor;
	}

	void ArcObject::borderColor (const ArcPixel &arcPixel)
	{
		borderProperties.color (arcPixel);
	}

	ArcPixel ArcObject::bordercolor ()
	{
		return borderProperties.color ();
	}

	void ArcObject::borderSize (const size_t &borderSize)
	{
		borderProperties.size (borderSize);
	}

	size_t ArcObject::borderSize ()
	{
		return borderProperties.size ();
	}

	ArcBuffer *ArcObject::render ()
	{
		if (xContrainst != nullptr)
			m_location.x (xContrainst->calculate (this));
		if (yContrainst != nullptr)
			m_location.y (yContrainst->calculate (this));
		ArcSize s = size ();
		size_t w = 0;
		size_t h = 0;
		if (wConstraint != nullptr) w = wConstraint->calculate (this);
		if (hConstraint != nullptr) h = hConstraint->calculate (this);
		if (w != s.width () || h != s.height ()) size ({w, h});
		s = size ();

		auto borderWidth = borderProperties.size ();

		ArcPixel *ptr = m_buffer.data ();
		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				unsigned int radius = 8;//borderProperties.radius ();

				int px = 0;
				int py = 0;
				if (x < radius)
					px = radius;
				else if (s.width () - x <= radius)
					px = s.width() - radius;

				if (y < radius)
					py = radius;
				else if (s.height () - y <= radius)
					py = s.height () - radius;

				int dist = sqrt (pow (px - x, 2) + pow (py - y, 2));
				if ((x < radius || s.width () - x <= radius) &&
					(y < radius || s.height () - y <= radius))
				{
					if (dist > radius)
						continue;
				}


				if (((x < borderWidth || s.width () - x <= borderWidth) ||
					(y < borderWidth || s.height () - y <= borderWidth)))

					ptr[y*w + x] = borderProperties.color ();
				else
					ptr[y*w + x] = m_backgroundColor;
			}
		}

		update ();


		m_layout.render (m_buffer);

		return &m_buffer;
	}

	uint32_t ArcObject::bodyWidth ()
	{
		return size ().width () - borderProperties.size ();
	}

	uint32_t ArcObject::bodyHeight ()
	{
		return size ().height () - borderProperties.size ();
	}

	ArcLayout *ArcObject::parent ()
	{
		return m_parent;
	}

	void ArcObject::setParent (ArcLayout *parent)
	{
		m_parent = parent;
	}

	void ArcObject::add (ArcObject *obj)
	{
		layout ().addObject (obj);
	}
}