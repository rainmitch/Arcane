
#include <arcane/UI/constraint.h>

#include <arcane/WindowRendering/window.h>
#include <arcane/UI/arcObject.h>

namespace arcane
{
	uint32_t Constraint::calculate (ArcObject *object) const
	{
		return 0;
	}

	uint32_t CenterScreenWidthConstraint::calculate (ArcObject *object) const
	{
		auto s = Window::size ();


		return (s.first / 2) - (object->size ().width () / 2);
	}

	uint32_t CenterScreenHeightConstraint::calculate (ArcObject *object) const
	{
		auto s = Window::size ();


		return (s.second / 2) - (object->size ().height() / 2);
	}

	PixelConstraint::PixelConstraint (const uint32_t &offset)
	{
		val = offset;
	}

	uint32_t PixelConstraint::calculate (ArcObject *object) const
	{
		return val;
	}

	ScreenWidthPercentConstraint::ScreenWidthPercentConstraint (const float &offset)
	{
		val = offset;
	}

	uint32_t ScreenWidthPercentConstraint::calculate (ArcObject *object) const
	{
		auto s = Window::size ();

		return val * s.first;
	}

	ScreenHeightPercentConstraint::ScreenHeightPercentConstraint (const float &offset)
	{
		val = offset;
	}

	uint32_t ScreenHeightPercentConstraint::calculate (ArcObject *object) const
	{
		auto s = Window::size ();
		return s.second * val;
	}

	ParentWidthPercentConstraint::ParentWidthPercentConstraint (const float &offset)
	{
		val = offset;
	}

	uint32_t ParentWidthPercentConstraint::calculate (ArcObject *object) const
	{
		return object->parent ()->parent ()->bodyWidth () * val;
	}

	uint32_t CenterParentWidthConstraint::calculate (ArcObject *object) const
	{
		return object->parent ()->parent ()->bodyWidth () / 2 - object->bodyWidth() / 2;
	}
}