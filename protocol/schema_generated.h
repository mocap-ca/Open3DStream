// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SCHEMA_MYGAME_SAMPLE_H_
#define FLATBUFFERS_GENERATED_SCHEMA_MYGAME_SAMPLE_H_

#include "flatbuffers/flatbuffers.h"

namespace MyGame {
namespace Sample {

struct Translation;

struct Rotation;

struct Transform;
struct TransformBuilder;

struct Subject;
struct SubjectBuilder;

struct SubjectList;
struct SubjectListBuilder;

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Translation FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Translation()
      : x_(0),
        y_(0),
        z_(0) {
  }
  Translation(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Translation, 12);

FLATBUFFERS_MANUALLY_ALIGNED_STRUCT(4) Rotation FLATBUFFERS_FINAL_CLASS {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Rotation()
      : x_(0),
        y_(0),
        z_(0) {
  }
  Rotation(float _x, float _y, float _z)
      : x_(flatbuffers::EndianScalar(_x)),
        y_(flatbuffers::EndianScalar(_y)),
        z_(flatbuffers::EndianScalar(_z)) {
  }
  float x() const {
    return flatbuffers::EndianScalar(x_);
  }
  float y() const {
    return flatbuffers::EndianScalar(y_);
  }
  float z() const {
    return flatbuffers::EndianScalar(z_);
  }
};
FLATBUFFERS_STRUCT_END(Rotation, 12);

struct Transform FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef TransformBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_TRANSLATION = 4,
    VT_ROTATION = 6,
    VT_PARENT = 8
  };
  const MyGame::Sample::Translation *translation() const {
    return GetStruct<const MyGame::Sample::Translation *>(VT_TRANSLATION);
  }
  const MyGame::Sample::Rotation *rotation() const {
    return GetStruct<const MyGame::Sample::Rotation *>(VT_ROTATION);
  }
  int32_t parent() const {
    return GetField<int32_t>(VT_PARENT, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<MyGame::Sample::Translation>(verifier, VT_TRANSLATION) &&
           VerifyField<MyGame::Sample::Rotation>(verifier, VT_ROTATION) &&
           VerifyField<int32_t>(verifier, VT_PARENT) &&
           verifier.EndTable();
  }
};

struct TransformBuilder {
  typedef Transform Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_translation(const MyGame::Sample::Translation *translation) {
    fbb_.AddStruct(Transform::VT_TRANSLATION, translation);
  }
  void add_rotation(const MyGame::Sample::Rotation *rotation) {
    fbb_.AddStruct(Transform::VT_ROTATION, rotation);
  }
  void add_parent(int32_t parent) {
    fbb_.AddElement<int32_t>(Transform::VT_PARENT, parent, 0);
  }
  explicit TransformBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Transform> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Transform>(end);
    return o;
  }
};

inline flatbuffers::Offset<Transform> CreateTransform(
    flatbuffers::FlatBufferBuilder &_fbb,
    const MyGame::Sample::Translation *translation = 0,
    const MyGame::Sample::Rotation *rotation = 0,
    int32_t parent = 0) {
  TransformBuilder builder_(_fbb);
  builder_.add_parent(parent);
  builder_.add_rotation(rotation);
  builder_.add_translation(translation);
  return builder_.Finish();
}

struct Subject FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SubjectBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_NODES = 4,
    VT_NAMES = 6,
    VT_NAME = 8
  };
  const flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Transform>> *nodes() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Transform>> *>(VT_NODES);
  }
  const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *names() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *>(VT_NAMES);
  }
  const flatbuffers::String *name() const {
    return GetPointer<const flatbuffers::String *>(VT_NAME);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_NODES) &&
           verifier.VerifyVector(nodes()) &&
           verifier.VerifyVectorOfTables(nodes()) &&
           VerifyOffset(verifier, VT_NAMES) &&
           verifier.VerifyVector(names()) &&
           verifier.VerifyVectorOfStrings(names()) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           verifier.EndTable();
  }
};

struct SubjectBuilder {
  typedef Subject Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_nodes(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Transform>>> nodes) {
    fbb_.AddOffset(Subject::VT_NODES, nodes);
  }
  void add_names(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> names) {
    fbb_.AddOffset(Subject::VT_NAMES, names);
  }
  void add_name(flatbuffers::Offset<flatbuffers::String> name) {
    fbb_.AddOffset(Subject::VT_NAME, name);
  }
  explicit SubjectBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Subject> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Subject>(end);
    return o;
  }
};

inline flatbuffers::Offset<Subject> CreateSubject(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Transform>>> nodes = 0,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>>> names = 0,
    flatbuffers::Offset<flatbuffers::String> name = 0) {
  SubjectBuilder builder_(_fbb);
  builder_.add_name(name);
  builder_.add_names(names);
  builder_.add_nodes(nodes);
  return builder_.Finish();
}

inline flatbuffers::Offset<Subject> CreateSubjectDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<MyGame::Sample::Transform>> *nodes = nullptr,
    const std::vector<flatbuffers::Offset<flatbuffers::String>> *names = nullptr,
    const char *name = nullptr) {
  auto nodes__ = nodes ? _fbb.CreateVector<flatbuffers::Offset<MyGame::Sample::Transform>>(*nodes) : 0;
  auto names__ = names ? _fbb.CreateVector<flatbuffers::Offset<flatbuffers::String>>(*names) : 0;
  auto name__ = name ? _fbb.CreateString(name) : 0;
  return MyGame::Sample::CreateSubject(
      _fbb,
      nodes__,
      names__,
      name__);
}

struct SubjectList FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SubjectListBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SUBJECTS = 4
  };
  const flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Subject>> *subjects() const {
    return GetPointer<const flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Subject>> *>(VT_SUBJECTS);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_SUBJECTS) &&
           verifier.VerifyVector(subjects()) &&
           verifier.VerifyVectorOfTables(subjects()) &&
           verifier.EndTable();
  }
};

struct SubjectListBuilder {
  typedef SubjectList Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_subjects(flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Subject>>> subjects) {
    fbb_.AddOffset(SubjectList::VT_SUBJECTS, subjects);
  }
  explicit SubjectListBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<SubjectList> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<SubjectList>(end);
    return o;
  }
};

inline flatbuffers::Offset<SubjectList> CreateSubjectList(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::Vector<flatbuffers::Offset<MyGame::Sample::Subject>>> subjects = 0) {
  SubjectListBuilder builder_(_fbb);
  builder_.add_subjects(subjects);
  return builder_.Finish();
}

inline flatbuffers::Offset<SubjectList> CreateSubjectListDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    const std::vector<flatbuffers::Offset<MyGame::Sample::Subject>> *subjects = nullptr) {
  auto subjects__ = subjects ? _fbb.CreateVector<flatbuffers::Offset<MyGame::Sample::Subject>>(*subjects) : 0;
  return MyGame::Sample::CreateSubjectList(
      _fbb,
      subjects__);
}

inline const MyGame::Sample::SubjectList *GetSubjectList(const void *buf) {
  return flatbuffers::GetRoot<MyGame::Sample::SubjectList>(buf);
}

inline const MyGame::Sample::SubjectList *GetSizePrefixedSubjectList(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<MyGame::Sample::SubjectList>(buf);
}

inline bool VerifySubjectListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<MyGame::Sample::SubjectList>(nullptr);
}

inline bool VerifySizePrefixedSubjectListBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<MyGame::Sample::SubjectList>(nullptr);
}

inline void FinishSubjectListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<MyGame::Sample::SubjectList> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedSubjectListBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<MyGame::Sample::SubjectList> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace Sample
}  // namespace MyGame

#endif  // FLATBUFFERS_GENERATED_SCHEMA_MYGAME_SAMPLE_H_
