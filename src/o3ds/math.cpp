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

#include "o3ds/math.h"

double O3DS::rad(double deg)
{
	return deg * 3.14159265358979323846 / 180.0;
};

O3DS::Matrix  O3DS::rotateX(double a)
{
	Eigen::AngleAxisd r(a, Eigen::Vector3d::UnitX());
	return Eigen::Affine3d(r).matrix();
};

O3DS::Matrix  O3DS::rotateY(double a)
{
	Eigen::AngleAxisd r(a, Eigen::Vector3d::UnitY());
	return Eigen::Affine3d(r).matrix();
};

O3DS::Matrix  O3DS::rotateZ(double a)
{
	Eigen::AngleAxisd r(a, Eigen::Vector3d::UnitZ());
	return Eigen::Affine3d(r).matrix();
};

O3DS::Matrix  O3DS::fromQuaternion(const Eigen::Quaterniond& q)
{
	Eigen::Matrix3d R = q.toRotationMatrix();
	Eigen::Matrix4d M = Eigen::Matrix4d::Identity();
	M.block<3, 3>(0, 0) = R;
	return M;
};

Eigen::Quaterniond O3DS::toQuaternion(const Matrix& m)
{
	Eigen::Matrix3d r = m.block<3, 3>(0, 0);
	r = r.normalized();
	return Eigen::Quaterniond(r);
};

O3DS::Matrix  O3DS::scale(const Eigen::Vector3d& s)
{
	O3DS::Matrix  m = Matrix::Identity();
	m(0, 0) = s.x();
	m(1, 1) = s.y();
	m(2, 2) = s.z();
	return m;
};

O3DS::Matrix  O3DS::translate(double tx, double ty, double tz)
{
	Matrix  m = Matrix::Identity();
	m(0, 3) = tx;
	m(1, 3) = ty;
	m(2, 3) = tz;
	return m;
};

O3DS::Matrix  O3DS::translate(const Eigen::Vector3d& value)
{
	O3DS::Matrix  m = Matrix::Identity();
	m(0, 3) = value.x();
	m(1, 3) = value.y();
	m(2, 3) = value.z();
	return m;
};
