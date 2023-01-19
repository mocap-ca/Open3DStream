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

#ifndef TRANSFORM_COMPONENT_H
#define TRANSFORM_COMPONENT_H

#include "o3ds/math.h"

namespace O3DS
{
	enum ComponentType { TTranslation, TRotation, TOrientation, TScale, TMatrix };

	class TransformComponent
	{
	public:
		virtual ~TransformComponent() {};

		virtual Matrixd asMatrix() = 0;

		Matrixd operator *(Matrixd &other)
		{
			return this->asMatrix() * other;
		}

		virtual enum ComponentType transformType() = 0;
	};

	class TransformTranslation : public TransformComponent
	{
	public:
		TransformTranslation()
			: value(0.0, 0.0, 0.0) {}

		TransformTranslation(Vector3d v)
			: value(v) {}

		TransformTranslation(double x, double y, double z) 
			: value(x, y, z) {}

		virtual ~TransformTranslation() {};

		Matrixd asMatrix() override
		{
			return Matrixd::TranslateXYZ(value[0], value[1], value[2]);
		}

		enum ComponentType transformType() override { return TTranslation;  }

		double delta() { return dist(value, lastSentValue); }

		void sent() { lastSentValue = value; }

		Vector3d value;
		Vector3d lastSentValue;
	};

	class TransformRotation : public TransformComponent
	{
	public:
		TransformRotation()
			: value(0.0, 0.0, 0.0, 1.0) {}

		TransformRotation(Vector4d v)
			:value(v) {}

		TransformRotation(double x, double y, double z, double w)
			: value(x, y, z, w) {}

		virtual ~TransformRotation() {};

		Matrixd asMatrix() override
		{
			return Matrixd::Quaternion(value);
		}

		enum ComponentType transformType() override { return TRotation; }

		double delta() { return dist(value, lastSentValue); }

		void sent() { lastSentValue = value; }

		Vector4d value;
		Vector4d lastSentValue;
	};

	class TransformScale : public TransformComponent
	{
	public:
		TransformScale()
			: value(1.0, 1.0, 1.0) {}

		TransformScale(Vector3d v)
			: value(v) {}

		TransformScale(double x, double y, double z)
			: value(x, y, z) {}

		virtual ~TransformScale() {};

		Matrixd asMatrix() override
		{
			return Matrixd::Scale(value);
		}

		enum ComponentType transformType() override { return TScale; }

		double delta() { return dist(value, lastSentValue); }

		void sent() { lastSentValue = value; }

		Vector3d value;
		Vector3d lastSentValue;
	};

	class TransformMatrix : public TransformComponent
	{
	public:
		TransformMatrix(O3DS::Matrixd m)
			: value(m) {}

		TransformMatrix(double m[])
			: value(m) {}

		TransformMatrix()
			: value(1.0, 0.0, 0.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 1.0, 0.0,
				0.0, 0.0, 0.0, 1.0) {}

		virtual ~TransformMatrix() {};

		Matrixd asMatrix() override
		{
			return value;
		}

		enum ComponentType transformType() override { return TMatrix; }

		Matrixd value;
	};

}

#endif