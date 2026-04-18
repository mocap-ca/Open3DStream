/*
 * test_model.cpp
 * Plain C++ unit tests for O3DS model.h
 *
 * Build example:
 *   g++ -std=c++17 -I<o3ds_include_path> test_model.cpp -o test_model -lflatbuffers
 *   ./test_model
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <limits>

#include "o3ds/model.h"

 // ---------------------------------------------------------------------------
 // Minimal test harness
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
// Helpers
// ---------------------------------------------------------------------------

// Populate a Transform with known, deterministic values.
static void fillTransform(O3DS::Transform& t,
    double tx, double ty, double tz,
    double rx, double ry, double rz, double rw,
    double sx, double sy, double sz)
{
    t.mTransformOrder.clear();
    t.mTransformOrder.push_back(O3DS::TTranslation);
    t.mTransformOrder.push_back(O3DS::TRotation);
    t.mTransformOrder.push_back(O3DS::TScale);
    t.translation.value = Eigen::Vector3d(tx, ty, tz);
    t.rotation.value = Eigen::Quaterniond(rw, rx, ry, rz).normalized();
    t.scale.value = Eigen::Vector3d(sx, sy, sz);
}

// Serialize src then parse into dst.  Returns false on either failure.
static bool roundTrip(O3DS::SubjectList& src, O3DS::SubjectList& dst)
{
    std::vector<char> buf;
    if (!src.serialize(buf)) {
        std::cerr << "roundTrip: serialize failed: " << src.mError << "\n";
        return false;
    }

    if(!dst.parse(buf.data(), buf.size())) {
		std::cerr << "roundTrip: parse failed: " << dst.mError << "\n";
		return false;
	}
    
    return true;
}


// ---------------------------------------------------------------------------
// Suite 1 – Serialization / parse round-trips
// ---------------------------------------------------------------------------

static void suite_RoundTrip_EmptySubjectList()
{
    beginSuite("RoundTrip: empty SubjectList");

    O3DS::SubjectList src, dst;
    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse returns true for empty list");
    CHECK_MSG(dst.size() == 0, "parsed list is empty");
}

static void suite_RoundTrip_SingleSubjectNoTransforms()
{
    beginSuite("RoundTrip: single subject, no transforms");

    O3DS::SubjectList src, dst;
    O3DS::PerformerSubject*srcSubject = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor1");
    srcSubject->mName = "Actor1";
    auto t1 = srcSubject->addTransform();
    t1->mName = "Root";

    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");
    CHECK_MSG(dst.size() == 1, "one subject recovered");

    O3DS::PerformerSubject* s = dst.findSubjectByName<O3DS::PerformerSubject>("Actor1");
    CHECK_MSG(s != nullptr, "subject found by name");
    if (!s) return;
    CHECK_MSG(s->mName == "Actor1", "subject name matches");
    CHECK_MSG(s->mUuid == "uuid-actor1", "subject uuid matches");
    CHECK_MSG(s->mTransforms.size() == 1, "one transforms");
}

static void suite_RoundTrip_SingleTransformValues()
{
    beginSuite("RoundTrip: single transform value fidelity");

    O3DS::SubjectList src, dst;
    O3DS::PerformerSubject* subj = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-a");
    subj->mName = "Actor";
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Hips";

    const double tx = 1.5, ty = -2.0, tz = 3.25;
    const double rx = 0.0, ry = 0.707, rz = 0.0, rw = 0.707;
    const double sx = 1.0, sy = 1.0, sz = 1.0;
    fillTransform(*t, tx, ty, tz, rx, ry, rz, rw, sx, sy, sz);

    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");

    O3DS::PerformerSubject* ps = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    CHECK_MSG(ps != nullptr, "subject recovered");
    if (!ps) return;

    CHECK_MSG(ps->mTransforms.size() == 1, "one transform recovered");
    O3DS::Transform* pt = ps->mTransforms.at(0);
    if (!pt) return;

    CHECK_MSG(pt->mName == "Hips", "transform name matches");
    CHECK_MSG(pt->mParentId == -1, "parent id matches");

    const double kEps = 1e-5;
    CHECK_MSG(std::abs(pt->translation.value.x() - tx) < kEps, "translation X");
    CHECK_MSG(std::abs(pt->translation.value.y() - ty) < kEps, "translation Y");
    CHECK_MSG(std::abs(pt->translation.value.z() - tz) < kEps, "translation Z");
    CHECK_MSG(std::abs(pt->scale.value.x() - sx) < kEps, "scale X");
    CHECK_MSG(std::abs(pt->scale.value.y() - sy) < kEps, "scale Y");
    CHECK_MSG(std::abs(pt->scale.value.z() - sz) < kEps, "scale Z");
}

static void suite_RoundTrip_MultipleSubjects()
{
    beginSuite("RoundTrip: multiple subjects");

    O3DS::SubjectList src, dst;
    auto s1 = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-1");
    auto t1 = s1->addTransform();
    t1->mName = "Root";
    s1->mName = "Actor1";
    
    auto s2 = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-2");
    auto t2 = s2->addTransform();
    t2->mName = "Root";
    s2->mName = "Actor2";

    auto s3 = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-3");
    auto t3 = s3->addTransform();
    t3->mName = "Root";
    s3->mName = "Actor3";

    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");
    CHECK_MSG(dst.size() == 3, "three subjects recovered");
    CHECK_MSG(dst.findSubjectByName<O3DS::PerformerSubject>("Actor1") != nullptr, "Actor1 found by name");
    CHECK_MSG(dst.findSubjectByName<O3DS::PerformerSubject>("Actor2") != nullptr, "Actor2 found by name");
    CHECK_MSG(dst.findSubjectByName<O3DS::PerformerSubject>("Actor3") != nullptr, "Actor3 found by name");
    CHECK_MSG(dst.findSubjectByUuid<O3DS::PerformerSubject>("uuid-2") != nullptr, "Actor2 found by uuid");
}

static void suite_RoundTrip_ParentChildHierarchy()
{
    beginSuite("RoundTrip: parent-child transform hierarchy");

    O3DS::SubjectList src, dst;
    O3DS::Subject* subj = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-s");
    subj->mName = "Skeleton";
    auto t1 = subj->addTransform();
    t1->mName = "Root";
    auto t2 = subj->addTransform();
    t2->mName = "Spine";
    t2->mParentId = 0;
    auto t3 = subj->addTransform();
    t3->mName = "Head";
    t3->mParentId = 1;


    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");

    O3DS::PerformerSubject* ps = dst.findSubjectByName<O3DS::PerformerSubject>("Skeleton");
    CHECK_MSG(ps != nullptr, "subject recovered");
    if (!ps) return;

    CHECK_MSG(ps->mTransforms.size() == 3, "three transforms recovered");
    CHECK_MSG(ps->mTransforms.at(0)->mParentId == -1, "Root parentId == -1");
    CHECK_MSG(ps->mTransforms.at(1)->mParentId == 0, "Spine parentId == 0");
    CHECK_MSG(ps->mTransforms.at(2)->mParentId == 1, "Head parentId == 1");
    CHECK_MSG(ps->mTransforms.find("Head") != nullptr, "TransformList::find() works");
}

static void suite_RoundTrip_Camera()
{
    beginSuite("RoundTrip: camera lens properties");

    O3DS::SubjectList src, dst;
    O3DS::CameraSubject* cam = src.findOrAddSubject<O3DS::CameraSubject>("uuid-cam");
    cam->mName = "MainCam";
    cam->focalLength = 35.0f;
    cam->filmBackWidth = 36.0f;
    cam->filmBackHeight = 24.0f;
    cam->aperture = 2.8f;
    cam->focusDistance = 5000.0f;

    cam->addTransform()->mName = "CameraRoot";

    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");

    O3DS::CameraSubject* pc = dst.findSubjectByName<O3DS::CameraSubject>("MainCam");
    CHECK_MSG(pc != nullptr, "camera recovered by name");
    if (!pc) return;

    CHECK_MSG(dst.findSubjectByUuid<O3DS::CameraSubject>("uuid-cam") != nullptr, "camera found by uuid");

    /*
    const float kEps = 1e-4f;
    CHECK_MSG(std::abs(pc->focalLength - 35.0f) < kEps, "focalLength");
    CHECK_MSG(std::abs(pc->filmBackWidth - 36.0f) < kEps, "filmBackWidth");
    CHECK_MSG(std::abs(pc->filmBackHeight - 24.0f) < kEps, "filmBackHeight");
    CHECK_MSG(std::abs(pc->aperture - 2.8f) < kEps, "aperture");
    CHECK_MSG(std::abs(pc->focusDistance - 5000.0f) < kEps, "focusDistance");
    */
}

static void suite_RoundTrip_DisabledSubjectSkipped()
{
    beginSuite("RoundTrip: disabled subject excluded from encoding");

    O3DS::SubjectList src, dst;
    O3DS::Subject* a = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-a");
    a->mName = "Active";
    a->mEnabled = true;
    auto t = a->addTransform();
    t->mName = "Root";

    O3DS::Subject* b = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-d");
    b->mName = "Disabled";
    b->mEnabled = false;   // must not appear in serialized output

    bool ok = roundTrip(src, dst);
    CHECK_MSG(ok, "serialize+parse succeeds");
    CHECK_MSG(dst.findSubjectByName<O3DS::PerformerSubject>("Active") != nullptr, "active subject present");
    CHECK_MSG(dst.findSubjectByName<O3DS::PerformerSubject>("Disabled") == nullptr, "disabled subject absent");
}


// ---------------------------------------------------------------------------
// Suite 2 – Transform hierarchy & world matrices
// ---------------------------------------------------------------------------

static void suite_WorldMatrix_SingleTransform()
{
    beginSuite("WorldMatrix: single root transform");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Root";

    // Identity rotation, unit scale, translation only
    fillTransform(*t, 1.0, 2.0, 3.0, 0, 0, 0, 1, 1, 1, 1);

    bool ok = subj->calcMatrices();
    CHECK_MSG(ok, "calcMatrices returns true");
    CHECK_MSG(t->bWorldMatrix, "bWorldMatrix flag is set");

    const double kEps = 1e-6;
    CHECK_MSG(std::abs(t->mWorldMatrix(0, 3) - 1.0) < kEps, "world Tx == 1");
    CHECK_MSG(std::abs(t->mWorldMatrix(1, 3) - 2.0) < kEps, "world Ty == 2");
    CHECK_MSG(std::abs(t->mWorldMatrix(2, 3) - 3.0) < kEps, "world Tz == 3");
    CHECK_MSG(std::abs(t->mWorldMatrix(0, 0) - 1.0) < kEps, "world scale X == 1");
    CHECK_MSG(std::abs(t->mWorldMatrix(1, 1) - 1.0) < kEps, "world scale Y == 1");
    CHECK_MSG(std::abs(t->mWorldMatrix(2, 2) - 1.0) < kEps, "world scale Z == 1");
}

static void suite_WorldMatrix_ParentChildTranslation()
{
    beginSuite("WorldMatrix: parent-child translation accumulation");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");

    O3DS::Transform* parent = subj->addTransform();
    parent->mName = "Parent";
    O3DS::Transform* child = subj->addTransform();
    child->mName = "Child";
    child->mParentId = 0;

    // Parent at (10,0,0), child local at (5,0,0) -> world (15,0,0)
    fillTransform(*parent, 10.0, 0.0, 0.0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*child, 5.0, 0.0, 0.0, 0, 0, 0, 1, 1, 1, 1);

    bool ok = subj->calcMatrices();
    CHECK_MSG(ok, "calcMatrices returns true");

    const double kEps = 1e-5;
    CHECK_MSG(std::abs(child->mWorldMatrix(0, 3) - 15.0) < kEps,
        "child world Tx == parent Tx + child local Tx");
    CHECK_MSG(std::abs(child->mWorldMatrix(1, 3)) < kEps, "child world Ty == 0");
    CHECK_MSG(std::abs(child->mWorldMatrix(2, 3)) < kEps, "child world Tz == 0");
}

static void suite_WorldMatrix_ThreeLevelHierarchy()
{
    beginSuite("WorldMatrix: three-level translation chain");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");

    O3DS::Transform* a = subj->addTransform();
    a->mName = "A";
    a->mParentId = -1;
    O3DS::Transform* b = subj->addTransform();
    b->mName = "B";
    b->mParentId = 0;
    O3DS::Transform* c = subj->addTransform();
    c->mName = "C";
    c->mParentId = 1;

    fillTransform(*a, 1.0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*b, 2.0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*c, 3.0, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    bool ok = subj->calcMatrices();
    CHECK_MSG(ok, "calcMatrices returns true");

    const double kEps = 1e-5;
    CHECK_MSG(std::abs(c->mWorldMatrix(0, 3) - 6.0) < kEps,
        "grandchild world Tx == 1+2+3 == 6");
}

static void suite_WorldMatrix_ScaleInherited()
{
    beginSuite("WorldMatrix: parent scale is inherited by child");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");

    O3DS::Transform* parent = subj->addTransform();
    parent->mName = "Parent";
    O3DS::Transform* child = subj->addTransform();
    child->mName = "Child";
    child->mParentId = 0;

    // Parent 2x on X, child local at (1,0,0) -> world Tx == 2
    fillTransform(*parent, 0, 0, 0, 0, 0, 0, 1, 2.0, 1.0, 1.0);
    fillTransform(*child, 1, 0, 0, 0, 0, 0, 1, 1.0, 1.0, 1.0);

    bool ok = subj->calcMatrices();
    CHECK_MSG(ok, "calcMatrices returns true");

    const double kEps = 1e-5;
    CHECK_MSG(std::abs(child->mWorldMatrix(0, 3) - 2.0) < kEps,
        "child world Tx == parent_scale_x * child_local_tx");
}

static void suite_WorldMatrix_90DegRotation()
{
    beginSuite("WorldMatrix: 90-degree parent rotation re-maps child axis");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");

    O3DS::Transform* parent = subj->addTransform();
    parent->mName = "Parent";
    O3DS::Transform* child = subj->addTransform();
    child->mName = "Child";
    child->mParentId = 0;


    // Rotate parent 90° around Y: local +X becomes world -Z
    // Quaternion for 90° around Y: (w=cos45°, x=0, y=sin45°, z=0)
    const double s45 = std::sqrt(2.0) / 2.0;
    fillTransform(*parent, 0, 0, 0, 0, s45, 0, s45, 1, 1, 1);
    // Child sitting at local (1, 0, 0)
    fillTransform(*child, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    bool ok = subj->calcMatrices();
    CHECK_MSG(ok, "calcMatrices returns true");

    const double kEps = 1e-5;
    // After 90° Y rotation: world X ≈ 0, world Z ≈ -1
    CHECK_MSG(std::abs(child->mWorldMatrix(0, 3)) < kEps,
        "child world Tx ~= 0 after 90 Y rotation");
    CHECK_MSG(std::abs(child->mWorldMatrix(2, 3) - (-1.0)) < kEps,
        "child world Tz ~= -1 after 90 Y rotation");
}


// ---------------------------------------------------------------------------
// Suite 3 – Edge cases
// ---------------------------------------------------------------------------

static void suite_Edge_ParseEmptyBuffer()
{
    beginSuite("Edge: parse empty / malformed buffer");

    O3DS::SubjectList sl;
    std::vector<char> empty;
    CHECK_MSG(!sl.parse(empty.data(), 0),
        "parse of zero-length buffer returns false");

    std::vector<char> tiny(4, 0);
    CHECK_MSG(!sl.parse(tiny.data(), tiny.size()),
        "parse of 4-byte garbage returns false");
}

static void suite_Edge_ParseNullPointer()
{
    beginSuite("Edge: parse null pointer");

    O3DS::SubjectList sl;
    bool ok = sl.parse(nullptr, 8);
    CHECK_MSG(!ok, "parse(nullptr, 8) returns false without crashing");
}

static void suite_Edge_AllFinite_Clean()
{
    beginSuite("Edge: allFinite passes for valid data");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Root";
    fillTransform(*t, 1, 2, 3, 0, 0, 0, 1, 1, 1, 1);

    CHECK_MSG(t->allFinite(), "Transform::allFinite() true for clean data");
    CHECK_MSG(subj->allFinite(), "Subject::allFinite() true for clean data");
    CHECK_MSG(sl.allFinite(), "SubjectList::allFinite() true for clean data");
}

static void suite_Edge_AllFinite_NaN()
{
    beginSuite("Edge: allFinite fails when NaN present in translation");

    const double nan = std::numeric_limits<double>::quiet_NaN();

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Hips";
    fillTransform(*t, nan, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    CHECK_MSG(!t->allFinite(), "Transform::allFinite() false for NaN tx");
    CHECK_MSG(!subj->allFinite(), "Subject::allFinite() propagates NaN");
    CHECK_MSG(!sl.allFinite(), "SubjectList::allFinite() propagates NaN");
    // mError must identify the offending transform
    CHECK_MSG(!subj->mError.empty(), "Subject::mError is set after NaN");
}

static void suite_Edge_AllFinite_NaN_Scale()
{
    beginSuite("Edge: allFinite fails when NaN present in scale");

    const double nan = std::numeric_limits<double>::quiet_NaN();

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Root";
    fillTransform(*t, 0, 0, 0, 0, 0, 0, 1, nan, 1, 1);

    CHECK_MSG(!t->allFinite(), "Transform::allFinite() false for NaN scale");
}

static void suite_Edge_AllFinite_Inf()
{
    beginSuite("Edge: allFinite fails when Inf present");

    const double inf = std::numeric_limits<double>::infinity();

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("u");
    O3DS::Transform* t = subj->addTransform();
    t->mName = "Root";
    fillTransform(*t, 0, inf, 0, 0, 0, 0, 1, 1, 1, 1);

    CHECK_MSG(!t->allFinite(), "Transform::allFinite() false for +Inf ty");
    CHECK_MSG(!subj->allFinite(), "Subject::allFinite() propagates +Inf");
}

static void suite_Edge_FindNonexistent()
{
    beginSuite("Edge: find methods return nullptr for unknown keys");

    O3DS::SubjectList sl;
    auto s = sl.findOrAddSubject<O3DS::PerformerSubject>("uuid-real");
    s->mName = "Real";

    CHECK_MSG(sl.findSubjectByName<O3DS::PerformerSubject>("Ghost") == nullptr, "unknown name -> nullptr");
    CHECK_MSG(sl.findSubjectByUuid<O3DS::PerformerSubject>("uuid-ghost") == nullptr, "unknown uuid -> nullptr");
    CHECK_MSG(sl.findSubjectByName<O3DS::PerformerSubject>("NoCamera") == nullptr, "unknown camera -> nullptr");
    CHECK_MSG(sl.findSubjectByUuid<O3DS::CameraSubject>("uuid-nc") == nullptr, "unknown camera uuid -> nullptr");

    O3DS::Subject* subj = sl.findSubjectByName<O3DS::PerformerSubject>("Real");
    CHECK_MSG(subj->mTransforms.find("missing") == nullptr,
        "TransformList::find() returns nullptr for unknown name");
}

static void suite_Edge_ClearTransforms()
{
    beginSuite("Edge: clearTransforms keeps subject metadata");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    subj->mName = "Actor";
    auto t1 = subj->addTransform();
    t1->mName = "Hips";
    auto t2 = subj->addTransform();
    t2->mName = "Spine";
    t2->mParentId = 0;

    CHECK_MSG(subj->size() == 2, "two transforms before clear");
    subj->clearTransforms();
    CHECK_MSG(subj->size() == 0, "zero transforms after clearTransforms");
    CHECK_MSG(subj->mName == "Actor", "name preserved after clearTransforms");
    CHECK_MSG(subj->mUuid == "uuid-actor", "uuid preserved after clearTransforms");
}

static void suite_Edge_ClearAll()
{
    beginSuite("Edge: clearAll resets transforms and metadata");

    O3DS::SubjectList sl;
    O3DS::Subject* subj = sl.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    subj->mName = "Actor";
    auto t= subj->addTransform();
    t->mName = "Hips";

    subj->clearAll();
    CHECK_MSG(subj->size() == 0, "no transforms after clearAll");
    // mName / mUuid behaviour after clearAll is implementation-defined;
    // just verify it doesn't crash.
}

static void suite_Edge_ActiveCount()
{
    beginSuite("Edge: activeCount reflects mEnabled flag");

    O3DS::SubjectList sl;
    O3DS::Subject* a = sl.findOrAddSubject<O3DS::PerformerSubject>("ua");
    O3DS::Subject* b = sl.findOrAddSubject<O3DS::PerformerSubject>("ub");
    O3DS::Subject* c = sl.findOrAddSubject<O3DS::PerformerSubject>("uc");
    a->mEnabled = true;
    b->mEnabled = false;
    c->mEnabled = true;

    CHECK_MSG(sl.size() == 3, "total size == 3");
    CHECK_MSG(sl.activeCount() == 2, "activeCount == 2");

    b->mEnabled = true;
    CHECK_MSG(sl.activeCount() == 3, "activeCount == 3 after re-enabling B");
}

static void suite_Edge_DeltaThreshold()
{
    beginSuite("Edge: setDeltaThreshold stores value");

    O3DS::SubjectList sl;
    sl.setDeltaThreshold(0.001);
    CHECK_MSG(sl.mDeltaThreshold == 0.001, "threshold set to 0.001");

    sl.setDeltaThreshold(0.0);
    CHECK_MSG(sl.mDeltaThreshold == 0.0, "threshold can be set to zero");
}

static void suite_Edge_TransformEquality()
{
    beginSuite("Edge: Transform operator== and copy constructor");

    O3DS::Transform a("Joint", 0);
    fillTransform(a, 1, 2, 3, 0, 0, 0, 1, 1, 1, 1);

    O3DS::Transform b(a);   // copy constructor
    CHECK_MSG(a == b, "copy-constructed transform equals original");

    b.translation.value.x() = 99.0;
    CHECK_MSG(!(a == b), "modified copy no longer equals original");
}


// ---------------------------------------------------------------------------
// Suite 4 – Definition then delta update
//
// Simulates the real streaming workflow:
//   1. Sender broadcasts a full SubjectList (skeleton + camera) so the
//      receiver can build the hierarchy.
//   2. Sender broadcasts a sparse serializeUpdate() containing only the
//      transforms that moved beyond the delta threshold.
//   3. Receiver applies the update via parse() and the poses are merged
//      into the already-known hierarchy.
// ---------------------------------------------------------------------------

// Helper: send a full definition from src into dst via serialize/parse.
static bool sendDefinition(O3DS::SubjectList& src, O3DS::SubjectList& dst)
{
    std::vector<char> buf;
    if (!src.serialize(buf)) return false;
    return dst.parse(buf.data(), buf.size());
}

// Helper: send a delta update from src into dst via serializeUpdate/parse.
// Returns the number of transforms included in the update packet.
static bool sendUpdate(O3DS::SubjectList& src, O3DS::SubjectList& dst,
    size_t& updateCount)
{
    std::vector<char> buf;
    if (!src.serializeUpdate(buf, updateCount)) return false;
    return dst.parse(buf.data(), buf.size());
}

// ---- Test 1: definition round-trip preserves full skeleton + camera --------

static void suite_DefUpdate_DefinitionPreservesHierarchy()
{
    beginSuite("DefUpdate: full definition preserves skeleton and camera");

    // --- source ---
    O3DS::SubjectList src;

    O3DS::Subject* actor = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    actor->mName = "Actor";
    O3DS::Transform* root = actor->addTransform("Root", -1);
    O3DS::Transform* spine = actor->addTransform("Spine", 0);
    O3DS::Transform* head = actor->addTransform("Head", 1);
    fillTransform(*root, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*spine, 0, 10, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*head, 0, 5, 0, 0, 0, 0, 1, 1, 1, 1);

    O3DS::CameraSubject* cam = src.findOrAddSubject<O3DS::CameraSubject>("uuid-vcam");
    cam->mName = "VCam";
    cam->focalLength = 50.0f;
    cam->filmBackWidth = 36.0f;
    cam->filmBackHeight = 24.0f;
    cam->aperture = 4.0f;
    cam->focusDistance = 3000.0f;
    cam->addTransform("CameraRoot", -1);

    // --- receiver ---
    O3DS::SubjectList dst;
    bool ok = sendDefinition(src, dst);
    if (!ok) { std::cerr << dst.mError << std::endl; }
    CHECK_MSG(ok, "definition serialize+parse succeeds");

    // Skeleton
    O3DS::Subject* pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    CHECK_MSG(pa != nullptr, "Actor subject received");
    if (pa)
    {
        CHECK_MSG(pa->mTransforms.size() == 3, "three transforms received");
        CHECK_MSG(pa->mTransforms.at(0)->mName == "Root", "Root name");
        CHECK_MSG(pa->mTransforms.at(0)->mParentId == -1, "Root parentId");
        CHECK_MSG(pa->mTransforms.at(1)->mName == "Spine", "Spine name");
        CHECK_MSG(pa->mTransforms.at(1)->mParentId == 0, "Spine parentId");
        CHECK_MSG(pa->mTransforms.at(2)->mName == "Head", "Head name");
        CHECK_MSG(pa->mTransforms.at(2)->mParentId == 1, "Head parentId");
    }

    size_t count = 0;
    ok = sendUpdate(src, dst, count);
    CHECK_MSG(ok, "update serialize+parse succeeds");

    // Camera
    O3DS::CameraSubject* pc = dst.findSubjectByName<O3DS::CameraSubject>("VCam");
    CHECK_MSG(pc != nullptr, "VCam camera received");
    if (pc)
    {
        const float kEps = 1e-4f;
        CHECK_MSG(std::abs(pc->focalLength - 50.0f) < kEps, "focalLength");
        CHECK_MSG(std::abs(pc->filmBackWidth - 36.0f) < kEps, "filmBackWidth");
        CHECK_MSG(std::abs(pc->aperture - 4.0f) < kEps, "aperture");
        CHECK_MSG(std::abs(pc->focusDistance - 3000.0f) < kEps, "focusDistance");
    }
}

// ---- Test 2: update packet changes pose, hierarchy stays intact ------------

static void suite_DefUpdate_UpdateChangesPose()
{
    beginSuite("DefUpdate: update packet changes pose values");

    const double kEps = 1e-5;

    // --- source: send definition with initial T-pose ---
    O3DS::SubjectList src;
    O3DS::Subject* actor = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    actor->mName = "Actor";
    O3DS::Transform* root = actor->addTransform("Root", -1);
    O3DS::Transform* spine = actor->addTransform("Spine", 0);
    O3DS::Transform* head = actor->addTransform("Head", 1);
    fillTransform(*root, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*spine, 0, 10, 0, 0, 0, 0, 1, 1, 1, 1);
    fillTransform(*head, 0, 5, 0, 0, 0, 0, 1, 1, 1, 1);

    // Use a small threshold so any movement triggers an update
    src.setDeltaThreshold(0.0001);

    O3DS::SubjectList dst;
    bool ok = sendDefinition(src, dst);
    CHECK_MSG(ok, "definition sent successfully");

    // Verify initial pose on receiver
    O3DS::Subject* pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    CHECK_MSG(pa != nullptr, "Actor present after definition");
    if (pa)
    {
        CHECK_MSG(std::abs(pa->mTransforms.at(0)->translation.value.x()) < kEps,
            "initial Root Tx == 0");
    }

    // --- source: move Root and Spine, leave Head unchanged ---
    fillTransform(*root, 5, 0, 0, 0, 0, 0, 1, 1, 1, 1);   // Root moved on X
    fillTransform(*spine, 5, 12, 0, 0, 0, 0, 1, 1, 1, 1);   // Spine moved on X and Y
    // head is unchanged — should not appear in the update packet

    size_t updateCount = 0;
    ok = sendUpdate(src, dst, updateCount);
    CHECK_MSG(ok, "update serialize+parse succeeds");

    // At least Root and Spine should have been included
    CHECK_MSG(updateCount >= 2,
        "update packet contains at least the two moved transforms");

    // Verify updated values on receiver
    pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    CHECK_MSG(pa != nullptr, "Actor still present after update");
    if (!pa) return;

    O3DS::Transform* pRoot = pa->mTransforms.find("Root");
    O3DS::Transform* pSpine = pa->mTransforms.find("Spine");
    O3DS::Transform* pHead = pa->mTransforms.find("Head");

    CHECK_MSG(pRoot != nullptr, "Root transform still present");
    CHECK_MSG(pSpine != nullptr, "Spine transform still present");
    CHECK_MSG(pHead != nullptr, "Head transform still present");

    if (pRoot)
    {
        CHECK_MSG(std::abs(pRoot->translation.value.x() - 5.0) < kEps,
            "Root Tx updated to 5");
        CHECK_MSG(std::abs(pRoot->translation.value.y()) < kEps,
            "Root Ty still 0");
    }
    if (pSpine)
    {
        CHECK_MSG(std::abs(pSpine->translation.value.x() - 5.0) < kEps,
            "Spine Tx updated to 5");
        CHECK_MSG(std::abs(pSpine->translation.value.y() - 12.0) < kEps,
            "Spine Ty updated to 12");
    }
    if (pHead)
    {
        // Head did not move — its values should be unchanged from definition
        CHECK_MSG(std::abs(pHead->translation.value.x()) < kEps,
            "Head Tx unchanged at 0");
        CHECK_MSG(std::abs(pHead->translation.value.y() - 5.0) < kEps,
            "Head Ty unchanged at 5");
    }
}

// ---- Test 3: update below threshold produces empty packet ------------------

static void suite_DefUpdate_BelowThresholdProducesNoUpdate()
{
    beginSuite("DefUpdate: movement below threshold produces empty update");

    // --- source ---
    O3DS::SubjectList src;
    O3DS::Subject* actor = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    actor->mName = "Actor";
    O3DS::Transform* root = actor->addTransform();
    root->mName = "Root";
    fillTransform(*root, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    // Large threshold — tiny movements should not trigger an update
    src.setDeltaThreshold(1.0);

    O3DS::SubjectList dst;
    sendDefinition(src, dst);

    // Move by much less than the threshold
    fillTransform(*root, 0.0001, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    size_t updateCount = 0;
    // verify that count == 0.
    std::vector<char> buf;
    bool ok = src.serializeUpdate(buf, updateCount);
    CHECK_MSG(ok, "serializeUpdate returns true even when empty");
    CHECK_MSG(updateCount == 0, "zero transforms included when below threshold");
}

// ---- Test 4: sequential updates accumulate correctly -----------------------

static void suite_DefUpdate_SequentialUpdates()
{
    beginSuite("DefUpdate: sequential updates accumulate on receiver");

    const double kEps = 1e-5;

    O3DS::SubjectList src;
    O3DS::Subject* actor = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    actor->mName = "Actor";
    O3DS::Transform* root = actor->addTransform();
    root->mName = "Root";
    fillTransform(*root, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    src.setDeltaThreshold(0.0001);

    O3DS::SubjectList dst;
    sendDefinition(src, dst);

    // Frame 1: move to X=1
    fillTransform(*root, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    size_t count = 0;
    sendUpdate(src, dst, count);
    CHECK_MSG(count == 1, "frame 1: one transform in update");

    O3DS::Subject* pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    if (pa && pa->mTransforms.at(0))
        CHECK_MSG(std::abs(pa->mTransforms.at(0)->translation.value.x() - 1.0) < kEps,
            "frame 1: receiver Root Tx == 1");

    // Frame 2: move to X=2
    fillTransform(*root, 2, 0, 0, 0, 0, 0, 1, 1, 1, 1);
    sendUpdate(src, dst, count);
    CHECK_MSG(count == 1, "frame 2: one transform in update");

    pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    if (pa && pa->mTransforms.at(0))
        CHECK_MSG(std::abs(pa->mTransforms.at(0)->translation.value.x() - 2.0) < kEps,
            "frame 2: receiver Root Tx == 2");

    // Frame 3: no movement — should produce no update
    sendUpdate(src, dst, count);
    CHECK_MSG(count == 0, "frame 3: no update when pose unchanged");
}

// ---- Test 5: camera update changes lens values without resending skeleton --

static void suite_DefUpdate_CameraUpdateOnly()
{
    beginSuite("DefUpdate: camera update changes lens without full definition");

    const float kEps = 1e-4f;

    O3DS::SubjectList src;

    // Skeleton
    O3DS::Subject* actor = src.findOrAddSubject<O3DS::PerformerSubject>("uuid-actor");
    actor->mName = "Actor";
    O3DS::Transform* root = actor->addTransform();
    root->mName = "Root";
    fillTransform(*root, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1);

    // Camera
    O3DS::CameraSubject* cam = src.findOrAddSubject<O3DS::CameraSubject>("uuid-vcam");
    cam->mName = "VCam";
    cam->focalLength = 35.0f;
    cam->filmBackWidth = 36.0f;
    cam->filmBackHeight = 24.0f;
    cam->focusDistance = 5000.0f;  // initial, will be changed
    src.setDeltaThreshold(0.0001);
    cam->addTransform("CameraRoot", -1);

    // Send full definition
    O3DS::SubjectList dst;
    bool ok = sendDefinition(src, dst);
    if(!ok) { std::cerr << dst.mError << std::endl; }
    CHECK_MSG(ok, "definition sent successfully");

    // Change focal length and focus distance on source, send update
    cam->focalLength = 85.0f;
    cam->focusDistance = 1500.0f;
    cam->aperture = 2.8f;

    size_t updateCount = 0;
    ok = sendUpdate(src, dst, updateCount);
    CHECK_MSG(ok, "update sent successfully");

    // Receiver camera should reflect new lens values
    O3DS::CameraSubject* pc = dst.findSubjectByName<O3DS::CameraSubject>("VCam");
    CHECK_MSG(pc != nullptr, "VCam still present after update");
    if (pc)
    {
        CHECK_MSG(std::abs(pc->focalLength - 85.0f) < kEps,
            "focalLength updated to 85");
        CHECK_MSG(std::abs(pc->focusDistance - 1500.0f) < kEps,
            "focusDistance updated to 1500");
        // Unchanged values should be preserved
        CHECK_MSG(std::abs(pc->filmBackWidth - 36.0f) < kEps,
            "filmBackWidth unchanged");
        CHECK_MSG(std::abs(pc->aperture - 2.8f) < kEps,
            "aperture unchanged");
    }

    // Skeleton should still be intact
    O3DS::Subject* pa = dst.findSubjectByName<O3DS::PerformerSubject>("Actor");
    CHECK_MSG(pa != nullptr, "Actor skeleton intact after camera-only update");
    if (pa)
        CHECK_MSG(pa->mTransforms.size() == 1, "one skeleton transform intact");
}


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main()
{
    std::cout << "O3DS Model Unit Tests\n";

    // Round-trips
    suite_RoundTrip_EmptySubjectList();
    suite_RoundTrip_SingleSubjectNoTransforms();
    suite_RoundTrip_SingleTransformValues();
    suite_RoundTrip_MultipleSubjects();
    suite_RoundTrip_ParentChildHierarchy();
    suite_RoundTrip_Camera();
    suite_RoundTrip_DisabledSubjectSkipped();

    // World matrices
    suite_WorldMatrix_SingleTransform();
    suite_WorldMatrix_ParentChildTranslation();
    suite_WorldMatrix_ThreeLevelHierarchy();
    suite_WorldMatrix_ScaleInherited();
    suite_WorldMatrix_90DegRotation();

    // Edge cases
    suite_Edge_ParseEmptyBuffer();
    suite_Edge_ParseNullPointer();
    suite_Edge_AllFinite_Clean();
    suite_Edge_AllFinite_NaN();
    suite_Edge_AllFinite_NaN_Scale();
    suite_Edge_AllFinite_Inf();
    suite_Edge_FindNonexistent();
    suite_Edge_ClearTransforms();
    suite_Edge_ClearAll();
    suite_Edge_ActiveCount();
    suite_Edge_DeltaThreshold();
    suite_Edge_TransformEquality();

    // Definition + delta update
    suite_DefUpdate_DefinitionPreservesHierarchy();
    suite_DefUpdate_UpdateChangesPose();
    suite_DefUpdate_BelowThresholdProducesNoUpdate();
    suite_DefUpdate_SequentialUpdates();
    suite_DefUpdate_CameraUpdateOnly();

    printSummary();
    return sFailed ? 1 : 0;
}