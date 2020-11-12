#ifndef ARCANE_ARCOBJECT_H
#define ARCANE_ARCOBJECT_H

#include <arcane/UI/arcBuffer.h>
#include <arcane/UI/arcLayout.h>
#include <arcane/UI/constraint.h>

#include <arcane/UI/border.h>
#include <functional>


namespace arcane
{
	class ArcEvents
	{
	public:
		void onClick (const std::function<void ()> &func);
		void onHover (const std::function<void ()> &func);
		void onNotHover (const std::function<void ()> &func);
		void onFocus (const std::function<void ()> &func);

		std::function<void ()> onClickFunction;
		std::function<void ()> onHoverFunction;
		std::function<void ()> onNotHoverFunction;
		std::function<void ()> onFocusFunction;

		bool focus ();
	};

	class ArcObject : public ArcEvents
	{
	public:
		ArcObject ();
		~ArcObject ();

		void location (const ArcPoint &size);
		ArcPoint location ();
		void size (const ArcSize &size);
		ArcSize size ();

		virtual void update ();

		ArcBuffer &buffer ();
		ArcLayout &layout ();

		ArcLayout *parent ();
		void setParent (ArcLayout *parent);
		void add (ArcObject *obj);

		bool changed (const bool &reset=false);

		void x (Constraint *constraint);
		void y (Constraint *constraint);
		void width (Constraint *constraint);
		void height (Constraint *constraint);

		void backgroundColor (const ArcPixel &arcPixel);
		ArcPixel backgroundcolor ();

		void borderColor (const ArcPixel &arcPixel);
		ArcPixel bordercolor ();
		void borderSize (const size_t &borderSize);
		size_t borderSize ();

		uint32_t bodyWidth ();
		uint32_t bodyHeight ();

		ArcBuffer *render ();

	private:
		ArcBuffer m_buffer;
		ArcLayout *m_parent = nullptr;
		ArcLayout m_layout;
		ArcPoint m_location;
		BorderProperties borderProperties;
		ArcPixel m_backgroundColor=DEFAULT_BACKGROUND;

		Constraint *xContrainst = nullptr;
		Constraint *yContrainst = nullptr;
		Constraint *wConstraint = nullptr;
		Constraint *hConstraint = nullptr;
	};
	namespace UI
	{

	}
}


#endif //ARCANE_ARCOBJECT_H
