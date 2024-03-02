/*
Open 3D Stream

Copyright 2020 Alastair Macleod

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

#ifndef O3DS_MATRIX_H
#define O3DS_MATRIX_H

#include <math.h>

namespace O3DS
{

	double rad(double deg);

	/*! \class Vector4 math.h o3ds\math.h */
	//! A templated type 3D vector4 
	template<typename T>
	class Vector4
	{
	public:
		Vector4(T x, T y, T z, T w) : v{ x, y, z, w } {}

		Vector4() : v{0, 0, 0, 1} {}

		T v[4];
		T& operator[](int n) { return v[n]; }
	};

	typedef Vector4 <double> Vector4d;

	template<typename T>
	double dist(const Vector4<T> a, const Vector4<T> b)
	{
		double x = a.v[0] - b.v[0];
		double y = a.v[1] - b.v[1];
		double z = a.v[2] - b.v[2];
		double w = a.v[3] - b.v[3];
		double r = x * x + y * y + z * z + w * w;
		if (r == 0) return 0;
		return sqrt(r);
	}

	/*! \class Vector math.h o3ds\math.h */
	//! A templated type 3D vector4 
	template<typename T>
	class Vector3
	{
	public:
		Vector3(T x, T y, T z) : v{ x, y, z } {}

		Vector3() : v{ 0, 0, 0 } {}

		T v[3];
		T& operator[](int n) { return v[n]; }
	};

	typedef Vector3<double> Vector3d;

	template<typename T>
	double dist(const Vector3<T> a, const Vector3<T> b)
	{
		double x = a.v[0] - b.v[0];
		double y = a.v[1] - b.v[1];
		double z = a.v[2] - b.v[2];
		double r = x * x + y * y + z * z;
		if (r == 0) return 0;
		return sqrt(r);
	}

	/*! class Matrix math.h o3ds\math.h */
	//! A templated 4x4 matrix for transform operations
	template<typename T>
	class Matrix
	{
	public:
		Matrix(T m11, T m12, T m13, T m14,
		       T m21, T m22, T m23, T m24,
		       T m31, T m32, T m33, T m34,
		   	   T m41, T m42, T m43, T m44)
		{
			m[0][0] = m11; 	m[0][1] = m12;	m[0][2] = m13;	m[0][3] = m14;
			m[1][0] = m21; 	m[1][1] = m22;	m[1][2] = m23;	m[1][3] = m24;
			m[2][0] = m31; 	m[2][1] = m32;	m[2][2] = m33;	m[2][3] = m34;
			m[3][0] = m41; 	m[3][1] = m42;	m[3][2] = m43;	m[3][3] = m44;
		}

		Matrix()
		{
			m[0][0] = 1; 	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
			m[1][0] = 0; 	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
			m[2][0] = 0; 	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
			m[3][0] = 0; 	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
		}

		Matrix(T value[])
		{
			for(int u=0,i=0; u < 4; u++)
			{
				for (int v = 0; v < 4; v++, i++)
				{
					m[u][v] = value[i];
				}
			}

		}

		bool HasNan() const
		{
			for(int u=0; u < 4; u++) {
				for(int v = 0; v < 4; v++) {
					if(m[u][v] != m[u][v]) return true;
				}
			}
			return false;
		}
					

		double& operator()(int u, int v) { return m[u][v]; }

		const Matrix Transpose() const
		{
			return CMatrix4x4(
				m[0][0], m[1][0], m[2][0], m[3][0],
				m[0][1], m[1][1], m[2][1], m[3][1],
				m[0][2], m[1][2], m[2][2], m[3][2],
				m[0][3], m[1][3], m[2][3], m[3][3]);
		}

		const Vector4<T> GetTranslation()
		{
			Vector4<T> v;
			v.v[0] = m[3][0];
			v.v[1] = m[3][1];
			v.v[2] = m[3][2];
			v.v[3] = 0;

			return v;
		}

		static
		const Matrix TranslateXYZ(T tx, T ty, T tz)
		{
			return Matrix(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				tx, ty, tz, 1);
		}

		static Matrix TranslateXYZ(Vector3<T> v)
		{
			return Matrix(1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				v.v[0], v.v[1], v.v[2], 1);
		}

		static
		const Matrix RotateX(T a)
		{
			return Matrix(
				1, 0, 0, 0,
				0,  cos(a), sin(a), 0,
				0, -sin(a), cos(a), 0,
				0, 0, 0, 1);
		}

		static
		const Matrix RotateY(T a)
		{
			return Matrix(
				cos(a), 0, -sin(a), 0,
				0,      1,    0,    0,
				sin(a), 0, cos(a),  0,
				0,      0,   0,     1);
		}

		static
			const Matrix RotateZ(T a)
		{
			return Matrix(
				cos(a), sin(a), 0, 0,
				-sin(a), cos(a), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
		}

		static
		const Matrix Quaternion(Vector4<T> q)
		{
			T x2 = q.v[0] + q.v[0];
			T y2 = q.v[1] + q.v[1];
			T z2 = q.v[2] + q.v[2];
			T xx = q.v[0] * x2;
			T xy = q.v[0] * y2;
			T xz = q.v[0] * z2;
			T yy = q.v[1] * y2;
			T yz = q.v[1] * z2;
			T zz = q.v[2] * z2;
			T wx = q.v[3] * x2;
			T wy = q.v[3] * y2;
			T wz = q.v[3] * z2;

			return Matrix(
				1.0 - (yy + zz), xy + wz, xz - wy, 0.0,
				xy - wz, 1.0 - (xx + zz), yz + wx, 0.0,
				xz + wy, yz - wx, 1.0 - (xx + yy), 0.0,
				0, 0, 0, 1);
		}

		static
			const Matrix Scale(Vector3<T> s)
		{
			return Matrix(
				s[0], 0,    0,    0,
				0,    s[1], 0,    0,
				0,    0,    s[2], 0,
				0,    0,    0,    1);
		}

		//! Contructs a new matrix as the inverse of this matrix
		const Matrix Inverse()
		{
			T m22_33_23_32 = m[2][2] * m[3][3] - m[2][3] * m[3][2];
			T m23_30_20_33 = m[2][3] * m[3][0] - m[2][0] * m[3][3];
			T m20_31_21_30 = m[2][0] * m[3][1] - m[2][1] * m[3][0];
			T m21_32_22_31 = m[2][1] * m[3][2] - m[2][2] * m[3][1];
			T m23_31_21_33 = m[2][3] * m[3][1] - m[2][1] * m[3][3];
			T m20_32_22_30 = m[2][0] * m[3][2] - m[2][2] * m[3][0];

			T d00 = m[1][1] *  m22_33_23_32 + m[1][2] *  m23_31_21_33 + m[1][3] * m21_32_22_31;
			T d01 = m[1][0] *  m22_33_23_32 + m[1][2] *  m23_30_20_33 + m[1][3] * m20_32_22_30;
			T d02 = m[1][0] * -m23_31_21_33 + m[1][1] *  m23_30_20_33 + m[1][3] * m20_31_21_30;
			T d03 = m[1][0] *  m21_32_22_31 + m[1][1] * -m20_32_22_30 + m[1][2] * m20_31_21_30;

			T d10 = m[0][1] * m22_33_23_32 + m[0][2] *  m23_31_21_33 + m[0][3] * m21_32_22_31;
			T d11 = m[0][0] * m22_33_23_32 + m[0][2] *  m23_30_20_33 + m[0][3] * m20_32_22_30;
			T d12 = m[0][0] *-m23_31_21_33 + m[0][1] *  m23_30_20_33 + m[0][3] * m20_31_21_30;
			T d13 = m[0][0] * m21_32_22_31 + m[0][1] * -m20_32_22_30 + m[0][2] * m20_31_21_30;

			T m02_13_03_12 = m[0][2] * m[1][3] - m[0][3] * m[1][2];
			T m03_10_00_13 = m[0][3] * m[1][0] - m[0][0] * m[1][3];
			T m00_11_01_10 = m[0][0] * m[1][1] - m[0][1] * m[1][0];
			T m01_12_02_11 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
			T m03_11_01_13 = m[0][3] * m[1][1] - m[0][1] * m[1][3];
			T m00_12_02_10 = m[0][0] * m[1][2] - m[0][2] * m[1][0];

			T d20 = m[3][1] * m02_13_03_12 + m[3][2] * m03_11_01_13 + m[3][3] * m01_12_02_11;
			T d21 = m[3][0] * m02_13_03_12 + m[3][2] * m03_10_00_13 + m[3][3] * m00_12_02_10;
			T d22 = m[3][0] *-m03_11_01_13 + m[3][1] * m03_10_00_13 + m[3][3] * m00_11_01_10;
			T d23 = m[3][0] * m01_12_02_11 + m[3][1] *-m00_12_02_10 + m[3][2] * m00_11_01_10;

			T d30 = m[2][1] * m02_13_03_12 + m[2][2] * m03_11_01_13 + m[2][3] * m01_12_02_11;
			T d31 = m[2][0] * m02_13_03_12 + m[2][2] * m03_10_00_13 + m[2][3] * m00_12_02_10;
			T d32 = m[2][0] *-m03_11_01_13 + m[2][1] * m03_10_00_13 + m[2][3] * m00_11_01_10;
			T d33 = m[2][0] * m01_12_02_11 + m[2][1] *-m00_12_02_10 + m[2][2] * m00_11_01_10;

			T D = m[0][0] *d00 - m[0][1] *d01 + m[0][2] *d02 - m[0][3] *d03;

			return Matrix(
			  d00 / D, -d10 / D,  d20 / D, -d30 / D,
			 -d01 / D,  d11 / D, -d21 / D,  d31 / D,
			  d02 / D, -d12 / D,  d22 / D, -d32 / D,
			 -d03 / D,  d13 / D, -d23 / D,  d33 / D);
		}
		
		Vector4<T> GetQuaternion()
		{
			T qx, qy, qz, qw;
			T tr = m[0][0] + m[1][1] + m[2][2];
			T q[4], s;
			int i, j, k;
			const int nxt[3] = { 1, 2, 0 };

			if (tr > 0.0)
			{
				s  = sqrt(tr + 1.0);
				qw = s / 2.0;
				s  = 0.5 / s;
				qx = (m[1][2] - m[2][1]) * s;
				qy = (m[2][0] - m[0][2]) * s;
				qz = (m[0][1] - m[1][0]) * s;
			}
			else
			{			
				i = 0;
				
				if (m[1][1] > m[0][0])
					i = 1;
				
				if (m[2][2] > m[i][i])
					i = 2;

				j = nxt[i];
				k = nxt[j];

				s = sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0);

				q[i] = s * 0.5;

				if (s != 0.0)
					s = 0.5 / s;

				q[3] = (m[j][k] - m[k][j]) * s;
				q[j] = (m[i][j] + m[j][i]) * s;
				q[k] = (m[i][k] + m[k][i]) * s;

				qx = q[0];
				qy = q[1];
				qz = q[2];
				qw = q[3];
			}

			// Normalize
			T denom = sqrt(qx*qx + qy*qy + qz*qz + qw*qw);

			if (denom > 1.0e-7)
			{
				qx /= denom;
				qy /= denom;
				qz /= denom;
				qw /= denom;
			}
			else
			{
				qx = qy = qz = 0;
				qw = 1;
			}

			Vector4<T> ret;
			ret.v[0] = qx;
			ret.v[1] = qy;
			ret.v[2] = qz;
			ret.v[3] = qw;
			return ret;
		}

#if 0
		void pr(const char *lbl)
		{
			printf("==== %s ====\n", this->lbl);
			for (int u = 0; u < 4; u++)
			{
				for (int v = 0; v < 4; v++)
				{
					printf("%f  ", this->m[u][v]);
				}
				printf("\n");
			}

		}
#endif

		T m[4][4];
	}; // Matrix

	typedef Matrix<double> Matrixd;

#define CALC(i, j)	lhs.m[i][0] * rhs.m[0][j] + lhs.m[i][1] * rhs.m[1][j] + \
	 				lhs.m[i][2] * rhs.m[2][j] + lhs.m[i][3] * rhs.m[3][j]

	template<typename T>
	const Matrix<T> operator *(const Matrix<T> &lhs, const Matrix<T> &rhs)
	{
		return Matrix<T>(
			CALC(0, 0), CALC(0, 1), CALC(0, 2), CALC(0, 3),
			CALC(1, 0), CALC(1, 1), CALC(1, 2), CALC(1, 3),
			CALC(2, 0), CALC(2, 1), CALC(2, 2), CALC(2, 3),
			CALC(3, 0), CALC(3, 1), CALC(3, 2), CALC(3, 3));
	}

	template<typename T>
	const Vector4<T> operator *(const Matrix<T> &m, const Vector4<T> &v)
	{
		Vector4<T> res;
		res.v[0] = v.v[0] * m.m[0][0] + v.v[1] * m.m[1][0] + v.v[2] * m.m[2][0] + m.m[3][0];
		res.v[1] = v.v[0] * m.m[0][1] + v.v[1] * m.m[1][1] + v.v[2] * m.m[2][1] + m.m[3][1];
		res.v[2] = v.v[0] * m.m[0][2] + v.v[1] * m.m[1][2] + v.v[2] * m.m[2][2] + m.m[3][2];
		res.v[3] = v.v[0] * m.m[0][3] + v.v[1] * m.m[1][3] + v.v[2] * m.m[2][3] + m.m[3][3];
		return res;
	}
} // O3DS

#endif // O3DS_MATRIX_H
