#include "context.h"
#include <cmath>
#include <array>

namespace O3DS
{
    ConversionContext::ConversionContext()
		: axisRemap{ 0, 1, 2 }
		, axisSign{ 1.0f, 1.0f, 1.0f }
		, unitScale(1.0f)
		, mValid(false)
	{
	}

    ConversionContext::ConversionContext(const Context& from, const Context& to)
        : axisRemap{ 0, 1, 2 }
        , axisSign{ 1.0f, 1.0f, 1.0f }
        , unitScale(1.0f)
    {
        if (!to.computeConversion(from, axisRemap, axisSign, unitScale))
        {
            // One or both contexts invalid — leave as identity and signal failure
            axisRemap = { 0, 1, 2 };
            axisSign = { 1.0f, 1.0f, 1.0f };
            unitScale = 1.0f;
            mValid = false;
        }
        else
        {
            mValid = true;
        }

        Initialize();
    }

    // Metres per unit for each DistanceUnit value
    static float unitToMetres(DistanceUnit u)
    {
        switch (u)
        {
        case DistanceUnit::Millimeter:  return 0.001f;
        case DistanceUnit::Centimeter:  return 0.01f;
        case DistanceUnit::Meter:       return 1.0f;
        case DistanceUnit::Kilometer:   return 1000.0f;
        case DistanceUnit::Inch:        return 0.0254f;
        case DistanceUnit::Foot:        return 0.3048f;
        case DistanceUnit::Mile:        return 1609.344f;
        case DistanceUnit::LightYear:   return 9.461e15f;
        case DistanceUnit::Parsec:      return 3.086e16f;
        default:                        return 1.0f;
        }
    }

    // Returns 0=X, 1=Y, 2=Z slot for a given direction on this context.
    // Returns -1 if the direction (or its opposite) is not found.
    static int findAxis(const Context& ctx, Direction d)
    {
        int id = ctx.axisId(d);
        if (ctx.axisId(ctx.mX) == id) return 0;
        if (ctx.axisId(ctx.mY) == id) return 1;
        if (ctx.axisId(ctx.mZ) == id) return 2;
        return -1;
    }

    // Returns +1 if the direction carried by axis slot i points the same way as d,
    // -1 if opposite, 0 if not found.
    static float axisSign(const Context& ctx, int slot, Direction d)
    {
        Direction axisDir = (slot == 0) ? ctx.mX
            : (slot == 1) ? ctx.mY
            : ctx.mZ;

        if (axisDir == d) return  1.0f;

        // Check if they are opposites (same axisId, different direction)
        if (ctx.axisId(axisDir) == ctx.axisId(d)) return -1.0f;

        return 0.0f;
    }

    bool Context::operator==(const Context& other) const
    {
        return mX == other.mX
            && mY == other.mY
            && mZ == other.mZ
            && mDistance == other.mDistance;
    }

    bool Context::computeConversion(const Context& from,
        std::array<int, 3>& axisRemap,
        std::array<float, 3>& axisSign,
        float& unitScale) const
    {
        if (!valid() || !from.valid()) return false;

        // For each destination axis, find which source axis carries that direction
        const Direction srcAxes[3] = { from.mX, from.mY, from.mZ };

        for (int dst = 0; dst < 3; ++dst)
        {
            Direction dstDir = (dst == 0) ? mX : (dst == 1) ? mY : mZ;
            int dstAxisId = axisId(dstDir);

            bool found = false;
            for (int src = 0; src < 3; ++src)
            {
                if (from.axisId(srcAxes[src]) == dstAxisId)
                {
                    axisRemap[dst] = src;

                    // Same direction ? +1, opposite ? -1
                    axisSign[dst] = (srcAxes[src] == dstDir) ? 1.0f : -1.0f;

                    found = true;
                    break;
                }
            }

            if (!found) return false;  // contexts are not orthogonal
        }

        // Unit scale: convert from source units to destination units via metres
        unitScale = unitToMetres(from.mDistance) / unitToMetres(mDistance);

        return true;
    }

    // -- ConversionContext --------------------------------------------------------

    bool ConversionContext::isIdentity() const
    {
        return axisRemap[0] == 0 && axisRemap[1] == 1 && axisRemap[2] == 2
            && axisSign[0] == 1.0f && axisSign[1] == 1.0f && axisSign[2] == 1.0f
            && unitScale == 1.0f;
    }

    void ConversionContext::convertTranslation(Eigen::Vector3d& t) const
    {
        double src[3] = { t.x(), t.y(), t.z() };
        double x = src[axisRemap[0]] * axisSign[0] * unitScale;
        double y = src[axisRemap[1]] * axisSign[1] * unitScale;
        double z = src[axisRemap[2]] * axisSign[2] * unitScale;
        t = Eigen::Vector3d(x, y, z);
    }

    void ConversionContext::convertRotation(Eigen::Quaterniond& q) const
    {
        // Remap imaginary components using the same signed permutation as translation.
        // No unit scaling — quaternion components are dimensionless.
        double src[3] = { q.x(), q.y(), q.z() };
        double x = src[axisRemap[0]] * axisSign[0];
        double y = src[axisRemap[1]] * axisSign[1];
        double z = src[axisRemap[2]] * axisSign[2];
        double w = q.w();

        // If the permutation flips handedness (determinant = -1), negate w to
        // preserve rotation direction.
        // Determinant = sign(permutation) * product of axis signs.
        float signProduct = axisSign[0] * axisSign[1] * axisSign[2];

        int inv = 0;
        const auto& r = axisRemap;
        if (r[0] > r[1]) ++inv;
        if (r[0] > r[2]) ++inv;
        if (r[1] > r[2]) ++inv;
        float permSign = (inv % 2 == 0) ? 1.0f : -1.0f;

        if (signProduct * permSign < 0.0f) w = -w;

        q = Eigen::Quaterniond(w, x, y, z);
    }

    void ConversionContext::convertScale(Eigen::Vector3d& s) const
    {
        // Permute axes to match the destination coordinate system.
        // Signs are not applied — scale magnitude is always positive.
        double src[3] = { s.x(), s.y(), s.z() };
        s.x() = src[axisRemap[0]];
        s.y() = src[axisRemap[1]];
        s.z() = src[axisRemap[2]];
    }

    void ConversionContext::Initialize()
    {
        mBasis = Eigen::Matrix4d::Zero();

        // Each destination axis i draws from source axis axisRemap[i] with axisSign[i].
        // The translation column is handled separately — unit scale goes in [3][3].
        for (int dst = 0; dst < 3; ++dst)
        {
            int   src = axisRemap[dst];
            float sign = axisSign[dst];
            mBasis(dst, src) = sign;
        }
        mBasis(3, 3) = 1.0;

        // For a signed permutation matrix, transpose == inverse.
        mBasisInv = mBasis.transpose();
    }

    void ConversionContext::convertMatrix(Eigen::Matrix<double, 4, 4, Eigen::ColMajor>& m) const
    {
        // Apply unit scale to the translation column before reorienting.
        // Rotation and scale components are dimensionless so unitScale doesn't
        // apply to them — only to the translation (column 3, rows 0-2).
        m(0, 3) *= unitScale;
        m(1, 3) *= unitScale;
        m(2, 3) *= unitScale;

        m = (mBasis * m * mBasisInv).eval();
    }


} // namespace O3DS
