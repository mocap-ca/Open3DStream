/*
Open 3D Stream

Copyright 2026 Alastair Macleod

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

#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "o3ds/math.h"
#include <cassert>

namespace O3DS
{
	enum ComponentType { TTranslation, TRotation, TOrientation, TScale, TMatrix };

	class TransformComponent
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	public:
		virtual ~TransformComponent() = default;

		virtual Matrix asMatrix() const = 0;

		Matrix operator * (const Matrix& other) const
		{
			return this->asMatrix() * other;
		}

		virtual enum ComponentType transformType() = 0;
	};

	class TransformTranslation : public TransformComponent
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	public:
		TransformTranslation()
			: value(Eigen::Vector3d::Zero())
			, lastSentValue(Eigen::Vector3d::Zero())
		{}

		TransformTranslation(const Eigen::Vector3d& v)
			: value(v)
			, lastSentValue(Eigen::Vector3d::Zero())
		{
			assert(v.allFinite());
		}
		
		TransformTranslation(double x, double y, double z)
			: value(Eigen::Vector3d(x, y, z))
			, lastSentValue(Eigen::Vector3d::Zero())
		{
			assert(x == x);
			assert(y == y);
			assert(z == z);
		}

		virtual ~TransformTranslation() {};
		
		Matrix asMatrix() const override
		{
			return translate(Eigen::Vector3d(value[0], value[1], value[2]));
		}

		enum ComponentType transformType() override { return TTranslation;  }

		double delta() { return (value - lastSentValue).norm(); }

		void sent() { lastSentValue = value; assert(value.allFinite()); }

		Eigen::Vector3d value;
		Eigen::Vector3d lastSentValue;
	};

	class TransformRotation : public TransformComponent
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	public:
		// Note eigein has w first, we use w last here.
		TransformRotation()
			: value(1.0, 0.0, 0.0, 0.0)
		    , lastSentValue(0.0, 0.0, 0.0, 0.0) 
		{}

		TransformRotation(const Eigen::Quaterniond& v)
			:value(v) {}

		// Note eigein has w first, we use w last here.
		TransformRotation(double x, double y, double z, double w)
			: value(w, x, y, z)
			, lastSentValue(0.0, 0.0, 0.0, 0.0)
		{}

		virtual ~TransformRotation() {};

		Matrix asMatrix() const override
		{
			return fromQuaternion(value);
		}

		enum ComponentType transformType() override { return TRotation; }

		double delta() { 	
			return std::abs(lastSentValue.angularDistance(value));
			// return Eigen::AngleAxisd(lastSentValue * value.conjugate()).angle();
		}

		void sent() { lastSentValue = Eigen::Quaterniond(value);
		}

		Eigen::Quaterniond value;
		Eigen::Quaterniond lastSentValue;
	};

	class TransformScale : public TransformComponent
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	public:
		TransformScale()
			: value(1.0, 1.0, 1.0)
		    , lastSentValue(0.0, 0.0, 0.0)
		{}

		TransformScale(const Eigen::Vector3d& v)
			: value(v)
			, lastSentValue(0.0, 0.0, 0.0) {}

		TransformScale(double x, double y, double z)
			: value(x, y, z)
			, lastSentValue(0.0, 0.0, 0.0)
		{}

		virtual ~TransformScale() {};

		Matrix asMatrix() const override
		{
			return scale(value);
		}

		enum ComponentType transformType() override { return TScale; }

		double delta() { return (value - lastSentValue).norm(); }

		void sent() { lastSentValue = value; }

		Eigen::Vector3d value;
		Eigen::Vector3d lastSentValue;
	};

	class TransformMatrix : public TransformComponent
	{
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	public:
		TransformMatrix(Matrix m)
			: value(m) {}

		TransformMatrix(const double* m)
		{
			value <<
				m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15];
		}

		TransformMatrix()
			: value(Matrix::Identity()) {}

		virtual ~TransformMatrix() {};

		Matrix asMatrix() const override
		{
			return value;
		}

		enum ComponentType transformType() override { return TMatrix; }

		Matrix value;
	};
}


#endif