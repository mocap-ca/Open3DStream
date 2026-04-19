/*
 * test_coordinate_conversion.cpp
 * Plain C++ unit tests for O3DS coordinate system conversion.
 *
 * Encodes subject data in MotionBuilder format (X=Right, Y=Up, Z=Back, cm)
 * and parses it into Unreal format (X=Forward, Y=Right, Z=Up, cm).
 *
 * Coordinate conversion is applied to world matrices in calcMatrices(), not
 * to individual TRS components. Tests therefore verify mWorldMatrix values,
 * not translation.value / rotation.value, which remain in the sender's space.
 *
 * Build example:
 *   g++ -std=c++17 -I<o3ds_include_path> test_coordinate_conversion.cpp \
 *       -o test_coordinate_conversion -lflatbuffers
 *   ./test_coordinate_conversion
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <array>

#include "o3ds/model.h"
#include "o3ds/context.h"

 // ---------------------------------------------------------------------------
 // Minimal test harness (matches existing O3DS test style)
 // ---------------------------------------------------------------------------

static int sTotal = 0;
static int sPassed = 0;
static int sFailed = 0;

static void beginSuite(const std::string& name)
{
    std::cout << "\n=== " << name << " ===\n";
}

static void check(bool condition, const std::string& label,
    const char* file, int line)
{
    ++sTotal;
    if (condition)
    {
        ++sPassed;
        std::cout << "  [PASS] " << label << "\n";
    }
    else
    {
        ++sFailed;
        std::cout << "  [FAIL] " << label
            << "  (" << file << ":" << line << ")\n";
    }
}

#define CHECK(cond)          check((cond), #cond,  __FILE__, __LINE__)
#define CHECK_MSG(cond, msg) check((cond), (msg),  __FILE__, __LINE__)

static void printSummary()
{
    std::cout << "\n----------------------------------------\n"
        << "Results: " << sPassed << "/" << sTotal << " passed";
    if (sFailed) std::cout << "  (" << sFailed << " FAILED)";
    std::cout << "\n";
}


// ---------------------------------------------------------------------------
// Numeric helpers
// ---------------------------------------------------------------------------

static const float kTol = 1e-4f;

static bool nearEq(float a, float b, float tol = kTol)
{
    return std::fabs(a - b) <= tol;
}

// Quaternions q and -q represent the same rotation — compare both forms.
static bool quatNearEq(float ax, float ay, float az, float aw,
    float bx, float by, float bz, float bw)
{
    bool pos = nearEq(ax, bx) && nearEq(ay, by) && nearEq(az, bz) && nearEq(aw, bw);
    bool neg = nearEq(ax, -bx) && nearEq(ay, -by) && nearEq(az, -bz) && nearEq(aw, -bw);
    return pos || neg;
}

static const double s45 = std::sqrt(2.0) / 2.0;  // sin/cos of 45 degrees


// ---------------------------------------------------------------------------
// Coordinate system reference
// ---------------------------------------------------------------------------
//
//  MotionBuilder : X=Right,   Y=Up,    Z=Back,    centimetres  (right-handed)
//  Unreal        : X=Forward, Y=Right, Z=Up,      centimetres  (left-handed)
//
//  Axis mapping (Mobu -> Unreal), applied to world matrices:
//    Unreal X (Forward) <- Mobu Z negated  (Back -> Forward : flip sign)
//    Unreal Y (Right)   <- Mobu X          (Right -> Right  : same)
//    Unreal Z (Up)      <- Mobu Y          (Up -> Up        : same)
//
//  World matrix translation column rule for Mobu position (mx, my, mz):
//    Unreal X = -mz
//    Unreal Y =  mx
//    Unreal Z =  my
//
//  TRS components (translation.value, rotation.value) are NOT converted —
//  they remain in the sender's space after parse. Only mWorldMatrix is
//  converted, via C * M * C^-1 applied in calcMatrices().
//
//  Rotation in world matrix:
//    The 3x3 rotation submatrix of mWorldMatrix is similarly reoriented.
//    For a 90 deg rotation around Mobu Y (world up), the world matrix
//    rotation submatrix in Unreal space represents a 90 deg rotation around
//    Unreal Z (also world up), with winding reversed due to handedness flip.


// ---------------------------------------------------------------------------
// Helper: build and serialize three performers in Mobu context
//
//  performer-uuid-001  "TestPerformer1"
//    Hips    (root,  parent=-1): translation (100, 90, -50), identity rotation
//    Spine   (child, parent= 0): translation (  0, 20,   0), identity rotation
//    LeftArm (child, parent= 1): translation ( 30,  5,   0), identity rotation
//
//  performer-uuid-002  "TestPerformer2"
//    Hips    (root,  parent=-1): translation (0,0,0), 90 deg around Mobu Y
//
//  performer-uuid-003  "TestPerformer3"
//    Hips    (root,  parent=-1): translation (0,0,0), 90 deg around Mobu X
// ---------------------------------------------------------------------------

static bool buildMobuPerformers(O3DS::SubjectList& src,
    std::vector<char>& outbuf)
{
    src.mContext = O3DS::Context::Mobu();

    // -- Performer 1: hierarchy with identity rotations ----------------------

    O3DS::PerformerSubject* performer1 =
        src.findOrAddSubject<O3DS::PerformerSubject>("performer-uuid-001");
    performer1->mName = "TestPerformer1";

    O3DS::Transform* hips = performer1->addTransform("Hips", -1);
    hips->translation.value = Eigen::Vector3d(100.0, 90.0, -50.0);
    hips->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);  // w,x,y,z identity
    hips->scale.value = Eigen::Vector3d(1, 1, 1);
    hips->setOrderTRS();

    O3DS::Transform* spine = performer1->addTransform("Spine", 0);
    spine->translation.value = Eigen::Vector3d(0.0, 20.0, 0.0);
    spine->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);
    spine->scale.value = Eigen::Vector3d(1, 1, 1);
    spine->setOrderTRS();

    O3DS::Transform* leftArm = performer1->addTransform("LeftArm", 1);
    leftArm->translation.value = Eigen::Vector3d(30.0, 5.0, 0.0);
    leftArm->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);
    leftArm->scale.value = Eigen::Vector3d(1, 1, 1);
    leftArm->setOrderTRS();

    // -- Performer 2: 90 deg around Mobu Y -----------------------------------

    O3DS::PerformerSubject* performer2 =
        src.findOrAddSubject<O3DS::PerformerSubject>("performer-uuid-002");
    performer2->mName = "TestPerformer2";

    O3DS::Transform* hips2 = performer2->addTransform("Hips", -1);
    hips2->translation.value = Eigen::Vector3d(0.0, 0.0, 0.0);
    hips2->rotation.value = Eigen::Quaterniond(s45, 0.0, s45, 0.0);  // w,x,y,z
    hips2->scale.value = Eigen::Vector3d(1, 1, 1);
    hips2->setOrderTRS();

    // -- Performer 3: 90 deg around Mobu X -----------------------------------

    O3DS::PerformerSubject* performer3 =
        src.findOrAddSubject<O3DS::PerformerSubject>("performer-uuid-003");
    performer3->mName = "TestPerformer3";

    O3DS::Transform* hips3 = performer3->addTransform("Hips", -1);
    hips3->translation.value = Eigen::Vector3d(0.0, 0.0, 0.0);
    hips3->rotation.value = Eigen::Quaterniond(s45, s45, 0.0, 0.0);  // w,x,y,z
    hips3->scale.value = Eigen::Vector3d(1, 1, 1);
    hips3->setOrderTRS();

    return src.serialize(outbuf, 1.0, "00:00:00:00");
}


// ---------------------------------------------------------------------------
// Suite 1: Context validity and conversion setup
// ---------------------------------------------------------------------------

static void suite_Context_ValidityAndConversion()
{
    beginSuite("Context: validity and conversion computation");

    O3DS::Context mobu = O3DS::Context::Mobu();
    O3DS::Context unreal = O3DS::Context::Unreal();
    O3DS::Context empty;

    CHECK_MSG(mobu.valid(), "Mobu context is valid");
    CHECK_MSG(unreal.valid(), "Unreal context is valid");
    CHECK_MSG(!empty.valid(), "Empty context is not valid");
    CHECK_MSG(mobu == mobu, "Mobu context equals itself");
    CHECK_MSG(!(mobu == unreal), "Mobu context differs from Unreal");

    std::array<int, 3>   remap;
    std::array<float, 3> sign;
    float                scale;

    bool ok = unreal.computeConversion(mobu, remap, sign, scale);
    CHECK_MSG(ok, "computeConversion succeeds for Mobu->Unreal");

    // Both presets are centimetres -> unit scale is 1.0
    CHECK_MSG(nearEq(scale, 1.0f), "Unit scale is 1.0 (both centimetres)");

    // Unreal X (Forward) <- Mobu Z (Back): source index 2, sign -1
    CHECK_MSG(remap[0] == 2, "Unreal X sourced from Mobu Z (index 2)");
    CHECK_MSG(nearEq(sign[0], -1.0f), "Unreal X sign is -1 (Back->Forward)");

    // Unreal Y (Right) <- Mobu X (Right): source index 0, sign +1
    CHECK_MSG(remap[1] == 0, "Unreal Y sourced from Mobu X (index 0)");
    CHECK_MSG(nearEq(sign[1], 1.0f), "Unreal Y sign is +1");

    // Unreal Z (Up) <- Mobu Y (Up): source index 1, sign +1
    CHECK_MSG(remap[2] == 1, "Unreal Z sourced from Mobu Y (index 1)");
    CHECK_MSG(nearEq(sign[2], 1.0f), "Unreal Z sign is +1");

    bool bad = unreal.computeConversion(empty, remap, sign, scale);
    CHECK_MSG(!bad, "computeConversion fails with invalid source context");
}


// ---------------------------------------------------------------------------
// Suite 2: Unit scaling
// ---------------------------------------------------------------------------

static void suite_Context_UnitScaling()
{
    beginSuite("Context: unit scaling");

    O3DS::Context cmCtx(O3DS::Direction::Right, O3DS::Direction::Up,
        O3DS::Direction::Back, O3DS::DistanceUnit::Centimeter);
    O3DS::Context mCtx(O3DS::Direction::Right, O3DS::Direction::Up,
        O3DS::Direction::Back, O3DS::DistanceUnit::Meter);
    O3DS::Context inCtx(O3DS::Direction::Right, O3DS::Direction::Up,
        O3DS::Direction::Back, O3DS::DistanceUnit::Inch);

    std::array<int, 3>   remap;
    std::array<float, 3> sign;
    float                scale;

    mCtx.computeConversion(cmCtx, remap, sign, scale);
    CHECK_MSG(nearEq(scale, 0.01f), "cm->m scale is 0.01");

    cmCtx.computeConversion(mCtx, remap, sign, scale);
    CHECK_MSG(nearEq(scale, 100.0f), "m->cm scale is 100.0");

    cmCtx.computeConversion(inCtx, remap, sign, scale);
    CHECK_MSG(nearEq(scale, 2.54f), "inch->cm scale is 2.54");

    cmCtx.computeConversion(cmCtx, remap, sign, scale);
    CHECK_MSG(nearEq(scale, 1.0f), "cm->cm scale is 1.0");
}


// ---------------------------------------------------------------------------
// Suite 3: ConversionContext matrix conversion arithmetic
//
// Verifies that convertMatrix correctly applies C * M * C^-1.
// Uses a pure translation matrix and a pure rotation matrix as test cases
// since these have analytically predictable results.
// ---------------------------------------------------------------------------

static void suite_ConversionContext_Matrix()
{
    beginSuite("ConversionContext: matrix conversion");

    O3DS::ConversionContext conv(O3DS::Context::Mobu(), O3DS::Context::Unreal());
    CHECK_MSG(conv.IsValid(), "ConversionContext constructed successfully");

    // Pure translation matrix: Mobu position (100, 90, -50)
    // Expected after conversion: Unreal (50, 100, 90)
    //   Unreal X = -mz = -(-50) = 50
    //   Unreal Y =  mx = 100
    //   Unreal Z =  my = 90
    {
        Eigen::Matrix4d m = Eigen::Matrix4d::Identity();
        m(0, 3) = 100.0;   // Mobu X
        m(1, 3) = 90.0;   // Mobu Y
        m(2, 3) = -50.0;   // Mobu Z

        conv.convertMatrix(m);

        CHECK_MSG(nearEq((float)m(0, 3), 50.0f), "Matrix translation X =  50");
        CHECK_MSG(nearEq((float)m(1, 3), 100.0f), "Matrix translation Y = 100");
        CHECK_MSG(nearEq((float)m(2, 3), 90.0f), "Matrix translation Z =  90");
        CHECK_MSG(nearEq((float)m(3, 3), 1.0f), "Homogeneous element unchanged");
    }

    // Pure rotation matrix: 90 deg around Mobu Y
    // In Mobu space the matrix rows are:
    //   [ 0,  0, 1, 0 ]   (Mobu X points along Mobu Z after rotation)
    //   [ 0,  1, 0, 0 ]   (Mobu Y unchanged)
    //   [-1,  0, 0, 0 ]   (Mobu Z points along -Mobu X after rotation)
    //   [ 0,  0, 0, 1 ]
    // After C * M * C^-1 this should represent 90 deg around Unreal Z.
    // The translation column stays zero throughout.
    {
        Eigen::Matrix4d m = Eigen::Matrix4d::Identity();
        m(0, 0) = 0.0;  m(0, 2) = 1.0;
        m(2, 0) = -1.0;  m(2, 2) = 0.0;

        conv.convertMatrix(m);

        // Translation column should remain zero
        CHECK_MSG(nearEq((float)m(0, 3), 0.f), "Rotation matrix translation X = 0");
        CHECK_MSG(nearEq((float)m(1, 3), 0.f), "Rotation matrix translation Y = 0");
        CHECK_MSG(nearEq((float)m(2, 3), 0.f), "Rotation matrix translation Z = 0");

        // The converted matrix should be a rotation around Unreal Z (index 2).
        // For 90 deg around Z in a left-handed system the rotation submatrix is:
        //   [ 0, 1, 0 ]
        //   [-1, 0, 0 ]
        //   [ 0, 0, 1 ]
        CHECK_MSG(nearEq((float)m(0, 0), 0.f), "R[0][0] = 0");
        CHECK_MSG(nearEq((float)m(0, 1), 1.f), "R[0][1] = 1");
        CHECK_MSG(nearEq((float)m(1, 0), -1.f), "R[1][0] = -1");
        CHECK_MSG(nearEq((float)m(1, 1), 0.f), "R[1][1] = 0");
        CHECK_MSG(nearEq((float)m(2, 2), 1.f), "R[2][2] = 1");
    }

    // Identity matrix should survive unchanged
    {
        Eigen::Matrix4d m = Eigen::Matrix4d::Identity();
        conv.convertMatrix(m);
        CHECK_MSG(nearEq((float)m(0, 0), 1.f) &&
            nearEq((float)m(1, 1), 1.f) &&
            nearEq((float)m(2, 2), 1.f) &&
            nearEq((float)m(3, 3), 1.f),
            "Identity matrix diagonal preserved");
        CHECK_MSG(nearEq((float)m(0, 3), 0.f) &&
            nearEq((float)m(1, 3), 0.f) &&
            nearEq((float)m(2, 3), 0.f),
            "Identity matrix translation column is zero");
    }
}


// ---------------------------------------------------------------------------
// Suite 4: TRS components are NOT converted — they stay in sender space
// ---------------------------------------------------------------------------

static void suite_TRS_NotConverted()
{
    beginSuite("TRS components: unchanged after parse (conversion is on world matrix)");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Unreal();
    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse succeeded");

    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found");
    if (subj1)
    {
        O3DS::Transform* hips = subj1->mTransforms.find("Hips");
        CHECK_MSG(hips != nullptr, "Hips found");
        if (hips)
        {
            // translation.value should still be in Mobu space
            auto& t = hips->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 100.0f), "Hips translation.value X = 100 (Mobu space)");
            CHECK_MSG(nearEq((float)t.y(), 90.0f), "Hips translation.value Y =  90 (Mobu space)");
            CHECK_MSG(nearEq((float)t.z(), -50.0f), "Hips translation.value Z = -50 (Mobu space)");

            // rotation.value should still be identity in Mobu space
            auto& r = hips->rotation.value;
            CHECK_MSG(quatNearEq((float)r.x(), (float)r.y(), (float)r.z(), (float)r.w(),
                0.f, 0.f, 0.f, 1.f),
                "Hips rotation.value is identity (Mobu space, unconverted)");
        }

        O3DS::Transform* spine = subj1->mTransforms.find("Spine");
        if (spine)
        {
            CHECK_MSG(spine->mParentId == 0, "Spine parent id preserved");
            auto& t = spine->translation.value;
            CHECK_MSG(nearEq((float)t.y(), 20.0f), "Spine translation.value Y = 20 (Mobu space)");
        }

        O3DS::Transform* leftArm = subj1->mTransforms.find("LeftArm");
        if (leftArm)
        {
            CHECK_MSG(leftArm->mParentId == 1, "LeftArm parent id preserved");
            auto& t = leftArm->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 30.0f), "LeftArm translation.value X = 30 (Mobu space)");
        }
    }
}


// ---------------------------------------------------------------------------
// Suite 5: Unknown receiver context adopts sender context
// ---------------------------------------------------------------------------

static void suite_UnknownContext_AdoptsSender()
{
    beginSuite("Unknown receiver context: adopts sender context");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    CHECK_MSG(!dst.mContext.valid(), "Receiver context starts as unknown");

    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse with unknown context succeeded");
    CHECK_MSG(dst.mContext.valid(), "Receiver context is now valid");
    CHECK_MSG(dst.mContext == O3DS::Context::Mobu(),
        "Receiver context adopted Mobu from stream");

    // No conversion — world matrix translation should match Mobu values directly
    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found");
    if (subj1)
    {
        O3DS::Transform* hips = subj1->mTransforms.find("Hips");
        if (hips && hips->bWorldMatrix)
        {
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(0, 3), 100.0f),
                "Hips world X = 100 (no conversion, Mobu space)");
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(1, 3), 90.0f),
                "Hips world Y =  90 (no conversion, Mobu space)");
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(2, 3), -50.0f),
                "Hips world Z = -50 (no conversion, Mobu space)");
        }
    }
}


// ---------------------------------------------------------------------------
// Suite 6: Matching contexts — no conversion applied to world matrices
// ---------------------------------------------------------------------------

static void suite_MatchingContext_NoConversion()
{
    beginSuite("Matching contexts: no conversion applied");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Mobu();
    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse with matching context succeeded");

    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found");
    if (subj1)
    {
        O3DS::Transform* hips = subj1->mTransforms.find("Hips");
        if (hips && hips->bWorldMatrix)
        {
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(0, 3), 100.0f),
                "Hips world X = 100 (no conversion)");
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(1, 3), 90.0f),
                "Hips world Y =  90 (no conversion)");
            CHECK_MSG(nearEq((float)hips->mWorldMatrix(2, 3), -50.0f),
                "Hips world Z = -50 (no conversion)");
        }
    }
}


// ---------------------------------------------------------------------------
// Suite 7: World matrix translation after conversion
//
//  Hips    Mobu (100, 90, -50) -> Unreal world ( 50, 100,  90)
//  Spine   Mobu local (0, 20, 0) -> Unreal world ( 50, 100, 110)
//  LeftArm Mobu local (30, 5, 0) -> Unreal world ( 50, 130, 115)
//
//  All joints have identity rotations so world positions are additive.
//  The conversion is C * Mworld * C^-1 so the translation column reflects
//  the full Mobu->Unreal axis remap on the accumulated world position.
// ---------------------------------------------------------------------------

static void suite_WorldMatrix_Translation()
{
    beginSuite("World matrices: translation correct after Mobu->Unreal conversion");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Unreal();
    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse succeeded");

    // calcMatrices is called inside parse, but call again to confirm idempotence
    bool matOk = dst.calcMatrices();
    CHECK_MSG(matOk, "calcMatrices() succeeded");
    CHECK_MSG(dst.mError.empty(), "No error from calcMatrices()");


    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found");
    if (!subj1) return;


    O3DS::Transform* hips = subj1->mTransforms.find("Hips");
    CHECK_MSG(hips != nullptr, "Hips found");
    if (hips && hips->bWorldMatrix)
    {
        float x = (float)hips->mWorldMatrix(0, 3);
        float y = (float)hips->mWorldMatrix(1, 3);
        float z = (float)hips->mWorldMatrix(2, 3);
        std::cout << x << " " << y << " " << z << "\n";
        CHECK_MSG(nearEq(x, 50.0f),  "Hips world X =  50");
        CHECK_MSG(nearEq(y, 100.0f), "Hips world Y = 100");
        CHECK_MSG(nearEq(z, 90.0f),  "Hips world Z =  90");
    }

    O3DS::Transform* spine = subj1->mTransforms.find("Spine");
    CHECK_MSG(spine != nullptr, "Spine found");
    if (spine && spine->bWorldMatrix)
    {
        CHECK_MSG(nearEq((float)spine->mWorldMatrix(0, 3), 50.0f), "Spine world X =  50");
        CHECK_MSG(nearEq((float)spine->mWorldMatrix(1, 3), 100.0f), "Spine world Y = 100");
        CHECK_MSG(nearEq((float)spine->mWorldMatrix(2, 3), 110.0f), "Spine world Z = 110 (Hips 90 + local 20)");
    }

    O3DS::Transform* leftArm = subj1->mTransforms.find("LeftArm");
    CHECK_MSG(leftArm != nullptr, "LeftArm found");
    if (leftArm && leftArm->bWorldMatrix)
    {
        CHECK_MSG(nearEq((float)leftArm->mWorldMatrix(0, 3), 50.0f), "LeftArm world X =  50");
        CHECK_MSG(nearEq((float)leftArm->mWorldMatrix(1, 3), 130.0f), "LeftArm world Y = 130 (Spine 100 + local 30)");
        CHECK_MSG(nearEq((float)leftArm->mWorldMatrix(2, 3), 115.0f), "LeftArm world Z = 115 (Spine 110 + local 5)");
    }
}


// ---------------------------------------------------------------------------
// Suite 8: World matrix rotation after conversion
//
//  Performer 2: 90 deg around Mobu Y -> world matrix should represent
//    90 deg around Unreal Z (world up in both systems).
//    The [0][2] and [2][0] elements of the 3x3 rotation submatrix encode this.
//
//  Performer 3: 90 deg around Mobu X (Right) -> world matrix should represent
//    90 deg around Unreal Y (Right in Unreal).
//    The [1][2] and [2][1] elements encode this.
// ---------------------------------------------------------------------------

static void suite_WorldMatrix_Rotation()
{
    beginSuite("World matrices: rotation correct after Mobu->Unreal conversion");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Unreal();
    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse succeeded");

    // -- Performer 2: 90 deg Mobu Y -> should appear as 90 deg Unreal Z -----
    //
    //  Mobu rotation matrix for 90 deg around Y:
    //    [ 0,  0,  1,  0 ]
    //    [ 0,  1,  0,  0 ]
    //    [-1,  0,  0,  0 ]
    //    [ 0,  0,  0,  1 ]
    //
    //  After C * M * C^-1 (Mobu->Unreal), represents 90 deg around Unreal Z:
    //    [ 0, -1,  0,  0 ]   (handedness reversal flips off-diagonal signs)
    //    [ 1,  0,  0,  0 ]
    //    [ 0,  0,  1,  0 ]
    //    [ 0,  0,  0,  1 ]

    O3DS::Subject* subj2 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-002");
    CHECK_MSG(subj2 != nullptr, "Performer 2 found");
    if (subj2)
    {
        O3DS::Transform* hips2 = subj2->mTransforms.find("Hips");
        CHECK_MSG(hips2 != nullptr, "P2 Hips found");
        if (hips2 && hips2->bWorldMatrix)
        {
            auto& wm = hips2->mWorldMatrix;
            std::cout << "P2 WM:\n"
                << wm(0, 0) << " " << wm(0, 1) << " " << wm(0, 2) << "\n"
                << wm(1, 0) << " " << wm(1, 1) << " " << wm(1, 2) << "\n"
                << wm(2, 0) << " " << wm(2, 1) << " " << wm(2, 2) << "\n";
            CHECK_MSG(nearEq((float)wm(0, 0), 0.f), "P2 WM[0][0] = 0");
            CHECK_MSG(nearEq((float)wm(0, 1), 1.f), "P2 WM[0][1] =  1 (90 deg around Unreal Z)");
            CHECK_MSG(nearEq((float)wm(1, 0), -1.f), "P2 WM[1][0] = -1 (90 deg around Unreal Z)");
            CHECK_MSG(nearEq((float)wm(1, 1), 0.f), "P2 WM[1][1] = 0");
            CHECK_MSG(nearEq((float)wm(2, 2), 1.f), "P2 WM[2][2] = 1 (Z axis unchanged)");
        }
    }

    // -- Performer 3: 90 deg Mobu X -> should appear as 90 deg Unreal Y -----
    //
    //  Mobu rotation matrix for 90 deg around X:
    //    [ 1,  0,  0,  0 ]
    //    [ 0,  0, -1,  0 ]
    //    [ 0,  1,  0,  0 ]
    //    [ 0,  0,  0,  1 ]
    //
    //  After C * M * C^-1 (Mobu->Unreal), represents 90 deg around Unreal Y:
    //    [ 0,  0,  1,  0 ]
    //    [ 0,  1,  0,  0 ]
    //    [-1,  0,  0,  0 ]
    //    [ 0,  0,  0,  1 ]

    O3DS::Subject* subj3 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-003");
    CHECK_MSG(subj3 != nullptr, "Performer 3 found");
    if (subj3)
    {
        O3DS::Transform* hips3 = subj3->mTransforms.find("Hips");
        CHECK_MSG(hips3 != nullptr, "P3 Hips found");
        if (hips3 && hips3->bWorldMatrix)
        {
            auto& wm = hips3->mWorldMatrix;
            std::cout << "P3 WM:\n"
                << wm(0, 0) << " " << wm(0, 1) << " " << wm(0, 2) << "\n"
                << wm(1, 0) << " " << wm(1, 1) << " " << wm(1, 2) << "\n"
                << wm(2, 0) << " " << wm(2, 1) << " " << wm(2, 2) << "\n";
            CHECK_MSG(nearEq((float)wm(0, 0), 0.f), "P3 WM[0][0] = 0");
            CHECK_MSG(nearEq((float)wm(0, 2), -1.f), "P3 WM[0][2] =  1 (90 deg around Unreal Y)");
            CHECK_MSG(nearEq((float)wm(2, 0), 1.f), "P3 WM[2][0] = -1 (90 deg around Unreal Y)");
            CHECK_MSG(nearEq((float)wm(2, 2), 0.f), "P3 WM[2][2] = 0");
            CHECK_MSG(nearEq((float)wm(1, 1), 1.f), "P3 WM[1][1] = 1 (Y axis unchanged)");
        }
    }
}


// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int conversionTest()
{
    std::cout << "O3DS Coordinate Conversion Tests\n";
    std::cout << "Sender:   MotionBuilder (X=Right, Y=Up, Z=Back, cm)\n";
    std::cout << "Receiver: Unreal        (X=Forward, Y=Right, Z=Up, cm)\n";
    std::cout << "Note: conversion applied to mWorldMatrix in calcMatrices(),\n";
    std::cout << "      not to individual TRS components.\n";

    suite_Context_ValidityAndConversion();
    suite_Context_UnitScaling();
    suite_ConversionContext_Matrix();
    suite_TRS_NotConverted();
    suite_UnknownContext_AdoptsSender();
    suite_MatchingContext_NoConversion();
    suite_WorldMatrix_Translation();
    suite_WorldMatrix_Rotation();

    printSummary();
    return sFailed ? 1 : 0;
}