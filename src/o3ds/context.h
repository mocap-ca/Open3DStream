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

#include <array>
#include <Eigen/Dense>

namespace O3DS
{

	class Context;

	//! Precomputed conversion between two coordinate contexts.
	//! Produced by Context::computeConversion(), consumed during parsing.
	struct ConversionContext
	{
		ConversionContext();

		ConversionContext::ConversionContext(const Context& from, const Context& to);

		std::array<int, 3>   axisRemap = { 0, 1, 2 };  // identity
		std::array<float, 3> axisSign = { 1, 1, 1 };  // no negation
		float                unitScale = 1.0f;

		//! Convert a matrix 
		void convertMatrix(Eigen::Matrix<double, 4, 4, Eigen::ColMajor> & m) const;

		//! Apply to a translation vector
		void convertTranslation(Eigen::Vector3d&) const;

		//! Apply to a quaternion (remaps xyz, adjusts w sign for handedness flip)
		void convertRotation(Eigen::Quaterniond&) const;

		//! Apply to a scale vector
		void ConversionContext::convertScale(Eigen::Vector3d& s) const;

		//! True if this conversion is a no-op
		bool isIdentity() const;

		//! True if this conversion is valid (i.e. contexts were valid and conversion could be computed).
		bool IsValid() const { return mValid; }

		//! Calculate the change-of-basis matrix for this conversion, and its inverse.
		void Initialize();

	private:

		//! The change-of-basis matrix, built by buildBasis().
		Eigen::Matrix<double, 4, 4, Eigen::ColMajor> mBasis;

		//! Transpose of mBasis (== inverse for a signed permutation matrix).
		Eigen::Matrix<double, 4, 4, Eigen::ColMajor> mBasisInv;

		//! Valid 
		bool mValid;

	};

	enum class Direction { Unknown = 0, Left, Right, Forward, Back, Up, Down, Last };

	enum class DistanceUnit { Unknown = 0, Millimeter, Centimeter, Meter, Kilometer, Inch, Foot, Mile, LightYear, Parsec };

	//! Describes the axis alignment of the coordinate system
	/*! \class Context context.h o3ds/context.h
	 */
	class Context
	{
	public:
		Context(Direction x, Direction y, Direction z, DistanceUnit d)
			: mX(x)
			, mY(y)
			, mZ(z)
			, mDistance(d)
		{};

		//! All axis set to None
		Context()
			: mX(Direction::Unknown)
			, mY(Direction::Unknown)
			, mZ(Direction::Unknown)
			, mDistance(DistanceUnit::Unknown)
		{};

		//! Copy constructor
		Context(const Context &other)
			: mX(other.mX)
			, mY(other.mY)
			, mZ(other.mZ)
			, mDistance(other.mDistance)
		{}

		//! Returns true if all the axis are defined and orthogonal 
		bool valid() const
		{
			if (mX == Direction::Unknown || mY == Direction::Unknown || mZ == Direction::Unknown || mDistance == DistanceUnit::Unknown)
			{
				return false;
			}
		
			int aX = axisId(mX);
			int aY = axisId(mY);
			int aZ = axisId(mZ);

			if (aX == 0 || aY == 0 || aZ == 0) return false;
			if (aX == aY || aX == aZ || aY == aZ) return false;
			return true;
		}

		//! Numerical representation for each axis Left/Right =1, Up/Down=2, Forward/Back = 3
		int axisId(enum Direction d) const
		{
			if (d == Direction::Left || d == Direction::Right) return 1;
			if (d == Direction::Up || d == Direction::Down)  return 2;
			if (d == Direction::Forward || d == Direction::Back) return 3;
			return 0;
		}

		//! Compute the conversion from another context into this one.
		//! Returns false if either context is invalid.
		//! axisRemap[i] = which source axis (0,1,2) maps to destination axis i
		//! axisSign[i]  = +1.0 or -1.0 for that mapping
		//! unitScale    = multiply source distances by this to get destination distances
		bool computeConversion(const Context& from,
			std::array<int, 3>& axisRemap,
			std::array<float, 3>& axisSign,
			float& unitScale) const;

		//! True if this context equals another (same axes, same unit)
		bool operator==(const Context& other) const;

		//! X = Right, Y = Up, Z = Back
		static Context Mobu()
		{
			return Context(Direction::Right, Direction::Up, Direction::Back,
				DistanceUnit::Centimeter);
		}

		//! X  = Forward, Y = Right, Z = Up
		static Context Unreal()
		{
			return Context(Direction::Forward, Direction::Right, Direction::Up,
				DistanceUnit::Centimeter);
		}

		enum Direction mX;
		enum Direction mY;
		enum Direction mZ;

		DistanceUnit mDistance;
	};

}// namespace

#endif