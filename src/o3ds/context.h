/*
Open 3D Stream

Copyright 2022 Alastair Macleod

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef O3DS_CONTEXT_H
#define O3DS_CONTEXT_H

namespace O3DS
{
	enum class Direction { None = 0, Left, Right, Forward, Back, Up, Down, Last };


	//! Describes the axis alignment of the coordinate system
	/*! \class Context context.h o3ds/context.h
	 */
	class Context
	{
	public:
		//! All axis set to None
		Context()
			: mX(Direction::None)
			, mY(Direction::None)
			, mZ(Direction::None)
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
			if (d == Direction::Left || d == Direction::Right) return 1;
			if (d == Direction::Up || d == Direction::Down)  return 2;
			if (d == Direction::Forward || d == Direction::Back) return 3;
			return 0;
		}

		//! Constuctor to define each axis
		Context(Direction x, Direction y, Direction z)
			: mX(x), mY(y), mZ(z)
		{}

		//! X = Right, Y = Up, Z = Back
		Context Mobu()
		{
			return Context(Direction::Right, Direction::Up, Direction::Back);
		}

		//! X  = Forward, Y = Right, Z = Up
		Context Unreal()
		{
			return Context(Direction::Forward, Direction::Right, Direction::Up);
		}

		enum Direction mX;
		enum Direction mY;
		enum Direction mZ;
		std::string mFormat;

	};

}// namespace

#endif