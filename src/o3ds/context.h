#ifndef O3DS_CONTEXT_H
#define O3DS_CONTEXT_H

namespace O3DS
{


	enum Direction { None = 0, Left, Right, Forward, Back, Up, Down, Last };


	//! Describes the axis alignment of the coordinate system
	/*! \class Context context.h o3ds/context.h
	 */
	class Context
	{
	public:
		//! All axis set to None
		Context()
			: mX(None)
			, mY(None)
			, mZ(None)
		{};

		//! Copy constructor
		Context(const Context &other)
			: mX(other.mX)
			, mY(other.mY)
			, mZ(other.mZ)
		{}

		//! Returns true if all the axis are defined and orthogonal 
		bool valid()
		{
			int aX = axisId(mX);
			int aY = axisId(mY);
			int aZ = axisId(mZ);

			if (aX == 0 || aY == 0 || aZ == 0) return false;
			if (aX == aY || aX == aZ || aY == aZ) return false;
			return true;
		}

		//! Numerical representation for each axis Left/Right =1, Up/Down=2, Forward/Back = 3
		int axisId(enum Direction d)
		{
			if (d == Left || d == Right) return 1;
			if (d == Up || d == Down)  return 2;
			if (d == Forward || d == Back) return 3;
			return 0;
		}

		//! Constuctor to define each axis
		Context(Direction x, Direction y, Direction z)
			: mX(x), mY(y), mZ(z)
		{}

		//! X = Right, Y = Up, Z = Back
		Context Mobu()
		{
			return Context(Right, Up, Back);
		}

		//! X  = Forward, Y = Right, Z = Up
		Context Unreal()
		{
			return Context(Forward, Right, Up);
		}

		enum Direction mX;
		enum Direction mY;
		enum Direction mZ;

	};

}// namespace

#endif