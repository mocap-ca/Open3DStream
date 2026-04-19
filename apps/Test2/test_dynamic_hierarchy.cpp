/*
 * test_dynamic_hierarchy.cpp
 * 
 * Creates a three-node transform hierarchy in MotionBuilder coordinate space
 * (X=Right, Y=Up, Z=Back, centimetres) and animates it in a loop:
 *
 *   Root      : raised 10cm along Y (Up), rotates slowly around Y (Up axis)
 *   Mid       : 5cm left of Root along X (Left=-X in Mobu), rotates around X (Right/Left axis)
 *   Tip       : 5cm forward from Mid along -Z (Forward=-Z in Mobu), rotates around Z (Back/Forward axis)
 *
 * Press Ctrl+C to exit.
 *
 * Build example:
 *   g++ -std=c++17 -I<o3ds_include_path> test_dynamic_hierarchy.cpp \
 *       -o test_dynamic_hierarchy -lflatbuffers
 *   ./test_dynamic_hierarchy
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <csignal>
#include <chrono>
#include <thread>
#include <iomanip>

#include "o3ds/model.h"
#include "o3ds/context.h"

// ---------------------------------------------------------------------------
// Shutdown flag — set by Ctrl+C handler
// ---------------------------------------------------------------------------

static volatile bool sRunning = true;

static void onSignal(int)
{
    sRunning = false;
}

// ---------------------------------------------------------------------------
// Rotation helpers
//
// Mobu coordinate system: X=Right, Y=Up, Z=Back
//
// Rotations are expressed as quaternions around a world axis.
// All angles in radians.
// ---------------------------------------------------------------------------

// Quaternion for rotation of `angle` radians around the given unit axis.
static Eigen::Quaterniond axisAngle(const Eigen::Vector3d& axis, double angle)
{
    return Eigen::Quaterniond(Eigen::AngleAxisd(angle, axis));
}

// Mobu axis unit vectors
static const Eigen::Vector3d kUp      ( 0,  1,  0);  // Mobu Y
static const Eigen::Vector3d kRight   ( 1,  0,  0);  // Mobu X
static const Eigen::Vector3d kForward ( 0,  0, -1);  // Mobu -Z (Back is +Z)

// ---------------------------------------------------------------------------
// buildHierarchy
//
// Creates a PerformerSubject with three transforms parented in a chain.
// Called once before the loop. Returns the three transform pointers for
// direct update each frame — no need to search by name repeatedly.
// ---------------------------------------------------------------------------

struct HierarchyNodes
{
    O3DS::Transform* root = nullptr;  // index 0
    O3DS::Transform* mid  = nullptr;  // index 1, parent 0
    O3DS::Transform* tip  = nullptr;  // index 2, parent 1
};

static HierarchyNodes buildHierarchy(O3DS::SubjectList& subjectList)
{
    subjectList.mContext = O3DS::Context::Mobu();

    O3DS::PerformerSubject* performer =
        subjectList.findOrAddSubject<O3DS::PerformerSubject>("dynamic-hierarchy-001");
    performer->mName = "DynamicHierarchy";

    // Root: raised 10cm along Mobu Y (Up). Rotates around Y.
    O3DS::Transform* root = performer->addTransform("Root", -1);
    root->translation.value = Eigen::Vector3d(0.0, 10.0, 0.0);
    root->rotation.value    = Eigen::Quaterniond::Identity();
    root->scale.value       = Eigen::Vector3d(1, 1, 1);
    root->setOrderTRS();

    // Mid: 5cm to the left of Root. Left in Mobu is -X.
    // Rotates around X (the Right/Left axis).
    O3DS::Transform* mid = performer->addTransform("Mid", 0);
    mid->translation.value = Eigen::Vector3d(-5.0, 0.0, 0.0);
    mid->rotation.value    = Eigen::Quaterniond::Identity();
    mid->scale.value       = Eigen::Vector3d(1, 1, 1);
    mid->setOrderTRS();

    // Tip: 5cm forward from Mid. Forward in Mobu is -Z.
    // Rotates around Z (the Back/Forward axis).
    O3DS::Transform* tip = performer->addTransform("Tip", 1);
    tip->translation.value = Eigen::Vector3d(0.0, 0.0, -5.0);
    tip->rotation.value    = Eigen::Quaterniond::Identity();
    tip->scale.value       = Eigen::Vector3d(1, 1, 1);
    tip->setOrderTRS();

    return { root, mid, tip };
}

// ---------------------------------------------------------------------------
// printWorldPositions
//
// After calcMatrices(), prints the world translation column for each node.
// ---------------------------------------------------------------------------

static void printWorldPositions(O3DS::Subject* subject, double t)
{
    auto pos = [&](const char* name) -> Eigen::Vector3d
    {
        O3DS::Transform* xf = subject->mTransforms.find(name);
        if (!xf || !xf->bWorldMatrix) return Eigen::Vector3d::Zero();
        return Eigen::Vector3d(
            xf->mWorldMatrix(0, 3),
            xf->mWorldMatrix(1, 3),
            xf->mWorldMatrix(2, 3));
    };

    auto p0 = pos("Root");
    auto p1 = pos("Mid");
    auto p2 = pos("Tip");

    std::cout << std::fixed << std::setprecision(2)
              << "t=" << std::setw(6) << t << "s"
              << "  Root("  << p0.x() << ", " << p0.y() << ", " << p0.z() << ")"
              << "  Mid("   << p1.x() << ", " << p1.y() << ", " << p1.z() << ")"
              << "  Tip("   << p2.x() << ", " << p2.y() << ", " << p2.z() << ")"
              << "\n";
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------

int dynamicHierarchyTest()
{
    std::signal(SIGINT, onSignal);

    std::cout << "O3DS Dynamic Hierarchy Test\n";
    std::cout << "Coordinate system: MotionBuilder (X=Right, Y=Up, Z=Back, cm)\n";
    std::cout << "Press Ctrl+C to exit.\n\n";

    // Rotation speeds (radians per second)
    const double kRootSpeed = 0.5;    // slow, around Mobu Y (Up)
    const double kMidSpeed  = 1.2;    // faster, around Mobu X (Right)
    const double kTipSpeed  = 2.0;    // fastest, around Mobu -Z (Forward)

    // Target frame rate
    const double kFrameRate = 60.0;
    const auto   kFrameTime = std::chrono::microseconds(
                                  static_cast<long long>(1e6 / kFrameRate));

    // Build subject list and hierarchy once
    O3DS::SubjectList subjectList;
    HierarchyNodes nodes = buildHierarchy(subjectList);

    O3DS::Subject* subject =
        subjectList.findSubjectByUuid<O3DS::Subject>("dynamic-hierarchy-001");

    auto startTime = std::chrono::steady_clock::now();
    auto nextFrame = startTime;

    while (sRunning)
    {
        // -- Timing ----------------------------------------------------------

        auto now = std::chrono::steady_clock::now();
        double t = std::chrono::duration<double>(now - startTime).count();

        // -- Update rotations ------------------------------------------------
        //
        // Each node rotates continuously around its own local axis.
        // The axis is expressed in the parent's space — since we are building
        // cumulative rotations from a fixed time origin, world-axis alignment
        // stays clean and the parent-child relationship is visible in how the
        // child paths trace compound arcs through space.

        nodes.root->rotation.value = axisAngle(kUp,      kRootSpeed * t);
        nodes.mid->rotation.value  = axisAngle(kRight,   kMidSpeed  * t);
        nodes.tip->rotation.value  = axisAngle(kForward, kTipSpeed  * t);

        // -- Compute world matrices ------------------------------------------

        if (!subjectList.calcMatrices())
        {
            std::cerr << "calcMatrices failed: " << subject->mError << "\n";
            break;
        }

        // -- Output ----------------------------------------------------------

        printWorldPositions(subject, t);

        // -- Frame pacing ----------------------------------------------------

        nextFrame += kFrameTime;
        std::this_thread::sleep_until(nextFrame);
    }

    std::cout << "\nStopped.\n";
    return 0;
}
