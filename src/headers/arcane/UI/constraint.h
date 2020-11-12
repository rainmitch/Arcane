#ifndef ARCANE_CONSTRAINT_H
#define ARCANE_CONSTRAINT_H

#include <cstdint>

namespace arcane
{
	class ArcObject;
	class Constraint
	{
	public:
		virtual uint32_t calculate (ArcObject *object) const;

	};

	class CenterScreenWidthConstraint : public Constraint
	{
	public:
		uint32_t calculate(ArcObject *object) const override;

	};

	class CenterScreenHeightConstraint : public Constraint
	{
	public:
		uint32_t calculate(ArcObject *object) const override;

	};

	class CenterParentWidthConstraint : public Constraint
	{
	public:
		uint32_t calculate(ArcObject *object) const override;

	};

	class PixelConstraint : public Constraint
	{
	public:
		explicit PixelConstraint (const uint32_t &offset);
		uint32_t calculate(ArcObject *object) const override;

	private:
		uint32_t val;
	};

	class ScreenWidthPercentConstraint : public Constraint
	{
	public:
		explicit ScreenWidthPercentConstraint (const float &offset);
		uint32_t calculate(ArcObject *object) const override;

	private:
		float val;
	};

	class ScreenHeightPercentConstraint : public Constraint
	{
	public:
		explicit ScreenHeightPercentConstraint (const float &offset);
		uint32_t calculate(ArcObject *object) const override;

	private:
		float val;
	};

	class ParentWidthPercentConstraint : public Constraint
	{
	public:
		explicit ParentWidthPercentConstraint (const float &offset);
		uint32_t calculate(ArcObject *object) const override;

	private:
		float val;
	};
}

#endif //ARCANE_CONSTRAINT_H
