/*
 * test_coordinate_conversion.cpp
 * Plain C++ unit tests for O3DS coordinate system conversion.
 *
 * Encodes subject data in MotionBuilder format (X=Right, Y=Up, Z=Back, cm)
 * and parses it into Unreal format (X=Forward, Y=Right, Z=Up, cm),
 * verifying translations, rotations, and parent-child relationships.
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
//  Axis mapping (Mobu -> Unreal):
//    Unreal X (Forward) <- Mobu Z negated  (Back -> Forward : flip sign)
//    Unreal Y (Right)   <- Mobu X          (Right -> Right  : same)
//    Unreal Z (Up)      <- Mobu Y          (Up -> Up        : same)
//
//  Translation rule for Mobu (mx, my, mz):
//    Unreal X = -mz
//    Unreal Y =  mx
//    Unreal Z =  my
//
//  Quaternion rule:
//    Imaginary part follows the same signed permutation as translation.
//    Handedness changes (right -> left) so determinant = -1 -> negate w.
//
//  Rotation examples:
//    90 deg Mobu Y : (x=0,   y=s45, z=0,   w=s45) -> Unreal (x=0,    y=0,   z=s45, w=-s45)
//    90 deg Mobu X : (x=s45, y=0,   z=0,   w=s45) -> Unreal (x=0,    y=s45, z=0,   w=-s45)
//    90 deg Mobu Z : (x=0,   y=0,   z=s45, w=s45) -> Unreal (x=-s45, y=0,   z=0,   w=-s45)


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

    // Mobu (100, 90, -50) -> Unreal (50, 100, 90)
    O3DS::Transform* hips = performer1->addTransform("Hips", -1);
    hips->translation.value = Eigen::Vector3d(100.0, 90.0, -50.0);    
    hips->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);  // w,x,y,z identity
    hips->scale.value = Eigen::Vector3d(1, 1, 1);
    hips->setOrderTRS();

    // Mobu (0, 20, 0) -> Unreal (0, 0, 20)
    O3DS::Transform* spine = performer1->addTransform("Spine", 0);
    spine->translation.value = Eigen::Vector3d(0.0, 20.0, 0.0);
    spine->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);
    spine->scale.value = Eigen::Vector3d(1, 1, 1);
    spine->setOrderTRS();

    // Mobu (30, 5, 0) -> Unreal (0, 30, 5)
    O3DS::Transform* leftArm = performer1->addTransform("LeftArm", 1);
    leftArm->translation.value = Eigen::Vector3d(30.0, 5.0, 0.0);
    leftArm->rotation.value = Eigen::Quaterniond(1, 0, 0, 0);
    leftArm->scale.value = Eigen::Vector3d(1, 1, 1);
    leftArm->setOrderTRS();

    // -- Performer 2: 90 deg around Mobu Y -----------------------------------

    O3DS::PerformerSubject* performer2 =
        src.findOrAddSubject<O3DS::PerformerSubject>("performer-uuid-002");
    performer2->mName = "TestPerformer2";

    // Mobu quat 90 deg Y: (w=s45, x=0, y=s45, z=0)
    // -> Unreal: (x=0, y=0, z=s45, w=-s45)
    O3DS::Transform* hips2 = performer2->addTransform("Hips", -1);
    hips2->translation.value = Eigen::Vector3d(0.0, 0.0, 0.0);
    hips2->rotation.value = Eigen::Quaterniond(s45, 0.0, s45, 0.0);  // w,x,y,z
    hips2->scale.value = Eigen::Vector3d(1, 1, 1);
    hips2->setOrderTRS();

    // -- Performer 3: 90 deg around Mobu X -----------------------------------

    O3DS::PerformerSubject* performer3 =
        src.findOrAddSubject<O3DS::PerformerSubject>("performer-uuid-003");
    performer3->mName = "TestPerformer3";

    // Mobu quat 90 deg X: (w=s45, x=s45, y=0, z=0)
    // Remap: ux = mz*(-1) = 0, uy = mx*(+1) = s45, uz = my*(+1) = 0
    // Handedness flip -> w = -s45
    // -> Unreal: (x=0, y=s45, z=0, w=-s45)
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

    // computeConversion fails if either context is invalid
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
// Suite 3: ConversionContext translation and rotation arithmetic
// ---------------------------------------------------------------------------

static void suite_ConversionContext_Values()
{
    beginSuite("ConversionContext: translation and rotation conversion");

    O3DS::ConversionContext conv;
    float unitScale;
    O3DS::Context::Unreal().computeConversion(
        O3DS::Context::Mobu(), conv.axisRemap, conv.axisSign, unitScale);
    conv.unitScale = unitScale;

    CHECK_MSG(!conv.isIdentity(), "Mobu->Unreal conversion is not identity");

    // Translation: Mobu (100, 90, -50) -> Unreal (50, 100, 90)
    {
        Eigen::Vector3d t(100.0, 90.0, -50.0);
        conv.convertTranslation(t);
        CHECK_MSG(nearEq(t.x(), 50.0f), "Translation X: -mz = -(-50) = 50");
        CHECK_MSG(nearEq(t.y(), 100.0f), "Translation Y:  mx = 100");
        CHECK_MSG(nearEq(t.z(), 90.0f), "Translation Z:  my = 90");
    }

    // Identity rotation: (0,0,0,1) — double-cover means (0,0,0,-1) is also valid
    {
        Eigen::Quaterniond q(1.0, 0.0, 0.0, 0.0);
        conv.convertRotation(q);
        CHECK_MSG(quatNearEq(q.x(), q.y(), q.z(), q.w(), 0.f, 0.f, 0.f, 1.f),
            "Identity quaternion preserved (double-cover aware)");
    }

    // 90 deg Mobu Y -> Unreal Z, handedness flips w
    {
        float rx = 0.f, ry = (float)s45, rz = 0.f, rw = (float)s45;
        Eigen::Quaterniond q(rw, rx, ry, rz);
        conv.convertRotation(q);
        CHECK_MSG(quatNearEq(q.x(), q.y(), q.z(), q.w(), 0.f, 0.f, (float)s45, -(float)s45),
            "90 deg Mobu-Y -> Unreal Z with handedness flip");
    }

    // 90 deg Mobu X -> Unreal Y, handedness flips w
    {
        float rx = (float)s45, ry = 0.f, rz = 0.f, rw = (float)s45;
        Eigen::Quaterniond q(rw, rx, ry, rz);
        conv.convertRotation(q);
        CHECK_MSG(quatNearEq(q.x(), q.y(), q.z(), q.w(), 0.f, (float)s45, 0.f, -(float)s45),
            "90 deg Mobu-X -> Unreal Y with handedness flip");
    }

    // 90 deg Mobu Z -> Unreal X negated, handedness flips w
    {
        float rx = 0.f, ry = 0.f, rz = (float)s45, rw = (float)s45;
        Eigen::Quaterniond q(rw, rx, ry, rz);
        conv.convertRotation(q);
        CHECK_MSG(quatNearEq(q.x(), q.y(), q.z(), q.w(), -(float)s45, 0.f, 0.f, -(float)s45),
            "90 deg Mobu-Z -> Unreal X negated with handedness flip");
    }
}


// ---------------------------------------------------------------------------
// Suite 4: Full serialize -> parse round-trip with conversion
// ---------------------------------------------------------------------------

static void suite_RoundTrip_MobuToUnreal()
{
    beginSuite("Round-trip: Mobu serialize -> Unreal parse");

    O3DS::SubjectList src;
    std::vector<char> buf;
    bool serOk = buildMobuPerformers(src, buf);
    CHECK_MSG(serOk, "Mobu performers serialized without error");
    CHECK_MSG(!buf.empty(), "Serialized buffer is non-empty");

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Unreal();

    bool parseOk = dst.parse(buf.data(), buf.size());
    CHECK_MSG(parseOk, "Parse into Unreal context succeeded");
    CHECK_MSG(dst.size() == 3, "Three performers present after parse");

    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    O3DS::Subject* subj2 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-002");
    O3DS::Subject* subj3 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-003");

    CHECK_MSG(subj1 != nullptr, "Performer 1 found by uuid");
    CHECK_MSG(subj2 != nullptr, "Performer 2 found by uuid");
    CHECK_MSG(subj3 != nullptr, "Performer 3 found by uuid");

    if (subj1) CHECK_MSG(subj1->mName == "TestPerformer1", "Performer 1 name correct");
    if (subj2) CHECK_MSG(subj2->mName == "TestPerformer2", "Performer 2 name correct");
    if (subj3) CHECK_MSG(subj3->mName == "TestPerformer3", "Performer 3 name correct");

    if (subj1) CHECK_MSG(subj1->size() == 3, "Performer 1 has 3 transforms");
    if (subj2) CHECK_MSG(subj2->size() == 1, "Performer 2 has 1 transform");
    if (subj3) CHECK_MSG(subj3->size() == 1, "Performer 3 has 1 transform");

    // -- Performer 1: Hips translation and rotation --------------------------
    // Mobu (100, 90, -50) -> Unreal X=50, Y=100, Z=90
    if (subj1)
    {
        O3DS::Transform* hips = subj1->mTransforms.find("Hips");
        CHECK_MSG(hips != nullptr, "P1 Hips transform found");
        if (hips)
        {
            auto& t = hips->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 50.0f), "P1 Hips X =  50 (was Mobu Z=-50 negated)");
            CHECK_MSG(nearEq((float)t.y(), 100.0f), "P1 Hips Y = 100 (was Mobu X=100)");
            CHECK_MSG(nearEq((float)t.z(), 90.0f), "P1 Hips Z =  90 (was Mobu Y=90)");

            auto& r = hips->rotation.value;
            CHECK_MSG(quatNearEq((float)r.x(), (float)r.y(),
                (float)r.z(), (float)r.w(),
                0.f, 0.f, 0.f, 1.f),
                "P1 Hips identity rotation preserved");
        }

        // Spine: parent index and translation
        // Mobu (0, 20, 0) -> Unreal (0, 0, 20)
        O3DS::Transform* spine = subj1->mTransforms.find("Spine");
        CHECK_MSG(spine != nullptr, "P1 Spine transform found");
        if (spine)
        {
            CHECK_MSG(spine->mParentId == 0, "Spine parent id is 0 (Hips)");
            auto& t = spine->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 0.0f), "P1 Spine X =  0");
            CHECK_MSG(nearEq((float)t.y(), 0.0f), "P1 Spine Y =  0");
            CHECK_MSG(nearEq((float)t.z(), 20.0f), "P1 Spine Z = 20 (was Mobu Y=20)");
        }

        // LeftArm: parent index and translation
        // Mobu (30, 5, 0) -> Unreal (0, 30, 5)
        O3DS::Transform* leftArm = subj1->mTransforms.find("LeftArm");
        CHECK_MSG(leftArm != nullptr, "P1 LeftArm transform found");
        if (leftArm)
        {
            CHECK_MSG(leftArm->mParentId == 1, "LeftArm parent id is 1 (Spine)");
            auto& t = leftArm->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 0.0f), "P1 LeftArm X =  0");
            CHECK_MSG(nearEq((float)t.y(), 30.0f), "P1 LeftArm Y = 30 (was Mobu X=30)");
            CHECK_MSG(nearEq((float)t.z(), 5.0f), "P1 LeftArm Z =  5 (was Mobu Y=5)");
        }
    }

    // -- Performer 2: 90 deg Mobu Y -> Unreal Z ------------------------------
    if (subj2)
    {
        O3DS::Transform* hips2 = subj2->mTransforms.find("Hips");
        CHECK_MSG(hips2 != nullptr, "P2 Hips transform found");
        if (hips2)
        {
            CHECK_MSG(hips2->mParentId == -1, "P2 Hips is a root joint");
            auto& r = hips2->rotation.value;
            CHECK_MSG(quatNearEq((float)r.x(), (float)r.y(),
                (float)r.z(), (float)r.w(),
                0.f, 0.f, (float)s45, -(float)s45),
                "P2: 90 deg Mobu-Y -> Unreal Z with handedness flip");
        }
    }

    // -- Performer 3: 90 deg Mobu X -> Unreal Y ------------------------------
    if (subj3)
    {
        O3DS::Transform* hips3 = subj3->mTransforms.find("Hips");
        CHECK_MSG(hips3 != nullptr, "P3 Hips transform found");
        if (hips3)
        {
            CHECK_MSG(hips3->mParentId == -1, "P3 Hips is a root joint");
            auto& r = hips3->rotation.value;
            CHECK_MSG(quatNearEq((float)r.x(), (float)r.y(),
                (float)r.z(), (float)r.w(),
                0.f, (float)s45, 0.f, -(float)s45),
                "P3: 90 deg Mobu-X -> Unreal Y with handedness flip");
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

    // Data should arrive unchanged — no conversion applied
    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found after adopt-context parse");
    if (subj1)
    {
        O3DS::Transform* hips = subj1->mTransforms.find("Hips");
        CHECK_MSG(hips != nullptr, "Hips found");
        if (hips)
        {
            auto& t = hips->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 100.0f), "Hips X = 100 (no conversion)");
            CHECK_MSG(nearEq((float)t.y(), 90.0f), "Hips Y =  90 (no conversion)");
            CHECK_MSG(nearEq((float)t.z(), -50.0f), "Hips Z = -50 (no conversion)");
        }
    }
}


// ---------------------------------------------------------------------------
// Suite 6: Matching contexts — no conversion applied
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
        CHECK_MSG(hips != nullptr, "Hips found");
        if (hips)
        {
            auto& t = hips->translation.value;
            CHECK_MSG(nearEq((float)t.x(), 100.0f), "Hips X = 100 (no conversion)");
            CHECK_MSG(nearEq((float)t.y(), 90.0f), "Hips Y =  90 (no conversion)");
            CHECK_MSG(nearEq((float)t.z(), -50.0f), "Hips Z = -50 (no conversion)");
        }
    }
}


// ---------------------------------------------------------------------------
// Suite 7: Parent-child world matrices after conversion
// ---------------------------------------------------------------------------

static void suite_WorldMatrix_AfterConversion()
{
    beginSuite("World matrices: correct after Mobu->Unreal conversion");

    O3DS::SubjectList src;
    std::vector<char> buf;
    buildMobuPerformers(src, buf);

    O3DS::SubjectList dst;
    dst.mContext = O3DS::Context::Unreal();
    bool ok = dst.parse(buf.data(), buf.size());
    CHECK_MSG(ok, "Parse failed in world matrix test");

    // All joints in performer 1 have identity rotations so world positions
    // are purely additive and easy to verify by hand:
    //
    //   Hips    world: ( 50, 100,  90)  <- converted from Mobu (100, 90, -50)
    //   Spine   world: ( 50, 100, 110)  <- Hips + local (0, 0, 20)
    //   LeftArm world: ( 50, 130, 115)  <- Spine + local (0, 30, 5)

    O3DS::Subject* subj1 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-001");
    CHECK_MSG(subj1 != nullptr, "Performer 1 found for world matrix test");
    if (!subj1) return;

    bool matOk = subj1->calcMatrices();
    CHECK_MSG(matOk, "calcMatrices() succeeded");
    CHECK_MSG(subj1->mError.empty(), "No error from calcMatrices()");

    O3DS::Transform* spine = subj1->mTransforms.find("Spine");
    CHECK_MSG(spine != nullptr, "Spine found for world matrix check");
    if (spine && spine->bWorldMatrix)
    {
        float wx = (float)spine->mWorldMatrix(0, 3);
        float wy = (float)spine->mWorldMatrix(1, 3);
        float wz = (float)spine->mWorldMatrix(2, 3);
        CHECK_MSG(nearEq(wx, 50.0f), "Spine world X =  50");
        CHECK_MSG(nearEq(wy, 100.0f), "Spine world Y = 100");
        CHECK_MSG(nearEq(wz, 110.0f), "Spine world Z = 110 (Hips 90 + local 20)");
    }

    O3DS::Transform* leftArm = subj1->mTransforms.find("LeftArm");
    CHECK_MSG(leftArm != nullptr, "LeftArm found for world matrix check");
    if (leftArm && leftArm->bWorldMatrix)
    {
        float wx = (float)leftArm->mWorldMatrix(0, 3);
        float wy = (float)leftArm->mWorldMatrix(1, 3);
        float wz = (float)leftArm->mWorldMatrix(2, 3);
        CHECK_MSG(nearEq(wx, 50.0f), "LeftArm world X =  50");
        CHECK_MSG(nearEq(wy, 130.0f), "LeftArm world Y = 130 (Spine 100 + local 30)");
        CHECK_MSG(nearEq(wz, 115.0f), "LeftArm world Z = 115 (Spine 110 + local 5)");
    }

    // Performers 2 and 3 are single-joint — calcMatrices should still succeed
    O3DS::Subject* subj2 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-002");
    O3DS::Subject* subj3 = dst.findSubjectByUuid<O3DS::Subject>("performer-uuid-003");
    if (subj2) CHECK_MSG(subj2->calcMatrices(), "P2 calcMatrices() succeeded");
    if (subj3) CHECK_MSG(subj3->calcMatrices(), "P3 calcMatrices() succeeded");
}


// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int conversionTest()
{
    std::cout << "O3DS Coordinate Conversion Tests\n";
    std::cout << "Sender:   MotionBuilder (X=Right, Y=Up, Z=Back, cm)\n";
    std::cout << "Receiver: Unreal        (X=Forward, Y=Right, Z=Up, cm)\n";

    suite_Context_ValidityAndConversion();
    suite_Context_UnitScaling();
    suite_ConversionContext_Values();
    suite_RoundTrip_MobuToUnreal();
    suite_UnknownContext_AdoptsSender();
    suite_MatchingContext_NoConversion();
    suite_WorldMatrix_AfterConversion();

    printSummary();
    return sFailed ? 1 : 0;
}
