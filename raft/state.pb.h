// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: state.proto

#ifndef PROTOBUF_state_2eproto__INCLUDED
#define PROTOBUF_state_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
namespace jraft {
class LogEntry;
class LogEntryDefaultTypeInternal;
extern LogEntryDefaultTypeInternal _LogEntry_default_instance_;
class PersistState;
class PersistStateDefaultTypeInternal;
extern PersistStateDefaultTypeInternal _PersistState_default_instance_;
}  // namespace jraft

namespace jraft {

namespace protobuf_state_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_state_2eproto

enum EntryType {
  ENTRY_TYPE_NOOP = 0,
  ENTRY_TYPE_DATA = 1,
  ENTRY_TYPE_CONFIGURATION = 3
};
bool EntryType_IsValid(int value);
const EntryType EntryType_MIN = ENTRY_TYPE_NOOP;
const EntryType EntryType_MAX = ENTRY_TYPE_CONFIGURATION;
const int EntryType_ARRAYSIZE = EntryType_MAX + 1;

const ::google::protobuf::EnumDescriptor* EntryType_descriptor();
inline const ::std::string& EntryType_Name(EntryType value) {
  return ::google::protobuf::internal::NameOfEnum(
    EntryType_descriptor(), value);
}
inline bool EntryType_Parse(
    const ::std::string& name, EntryType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<EntryType>(
    EntryType_descriptor(), name, value);
}
// ===================================================================

class LogEntry : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:jraft.LogEntry) */ {
 public:
  LogEntry();
  virtual ~LogEntry();

  LogEntry(const LogEntry& from);

  inline LogEntry& operator=(const LogEntry& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  LogEntry(LogEntry&& from) noexcept
    : LogEntry() {
    *this = ::std::move(from);
  }

  inline LogEntry& operator=(LogEntry&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const LogEntry& default_instance();

  static inline const LogEntry* internal_default_instance() {
    return reinterpret_cast<const LogEntry*>(
               &_LogEntry_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(LogEntry* other);
  friend void swap(LogEntry& a, LogEntry& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline LogEntry* New() const PROTOBUF_FINAL { return New(NULL); }

  LogEntry* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const LogEntry& from);
  void MergeFrom(const LogEntry& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(LogEntry* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string command = 4;
  bool has_command() const;
  void clear_command();
  static const int kCommandFieldNumber = 4;
  const ::std::string& command() const;
  void set_command(const ::std::string& value);
  #if LANG_CXX11
  void set_command(::std::string&& value);
  #endif
  void set_command(const char* value);
  void set_command(const char* value, size_t size);
  ::std::string* mutable_command();
  ::std::string* release_command();
  void set_allocated_command(::std::string* command);

  // required int64 term = 2;
  bool has_term() const;
  void clear_term();
  static const int kTermFieldNumber = 2;
  ::google::protobuf::int64 term() const;
  void set_term(::google::protobuf::int64 value);

  // required int64 index = 3;
  bool has_index() const;
  void clear_index();
  static const int kIndexFieldNumber = 3;
  ::google::protobuf::int64 index() const;
  void set_index(::google::protobuf::int64 value);

  // required .jraft.EntryType type = 1;
  bool has_type() const;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::jraft::EntryType type() const;
  void set_type(::jraft::EntryType value);

  // @@protoc_insertion_point(class_scope:jraft.LogEntry)
 private:
  void set_has_type();
  void clear_has_type();
  void set_has_term();
  void clear_has_term();
  void set_has_index();
  void clear_has_index();
  void set_has_command();
  void clear_has_command();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr command_;
  ::google::protobuf::int64 term_;
  ::google::protobuf::int64 index_;
  int type_;
  friend struct protobuf_state_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class PersistState : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:jraft.PersistState) */ {
 public:
  PersistState();
  virtual ~PersistState();

  PersistState(const PersistState& from);

  inline PersistState& operator=(const PersistState& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PersistState(PersistState&& from) noexcept
    : PersistState() {
    *this = ::std::move(from);
  }

  inline PersistState& operator=(PersistState&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PersistState& default_instance();

  static inline const PersistState* internal_default_instance() {
    return reinterpret_cast<const PersistState*>(
               &_PersistState_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(PersistState* other);
  friend void swap(PersistState& a, PersistState& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PersistState* New() const PROTOBUF_FINAL { return New(NULL); }

  PersistState* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const PersistState& from);
  void MergeFrom(const PersistState& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(PersistState* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .jraft.LogEntry logs = 3;
  int logs_size() const;
  void clear_logs();
  static const int kLogsFieldNumber = 3;
  const ::jraft::LogEntry& logs(int index) const;
  ::jraft::LogEntry* mutable_logs(int index);
  ::jraft::LogEntry* add_logs();
  ::google::protobuf::RepeatedPtrField< ::jraft::LogEntry >*
      mutable_logs();
  const ::google::protobuf::RepeatedPtrField< ::jraft::LogEntry >&
      logs() const;

  // required int64 currentTerm = 1;
  bool has_currentterm() const;
  void clear_currentterm();
  static const int kCurrentTermFieldNumber = 1;
  ::google::protobuf::int64 currentterm() const;
  void set_currentterm(::google::protobuf::int64 value);

  // required int64 votedFor = 2;
  bool has_votedfor() const;
  void clear_votedfor();
  static const int kVotedForFieldNumber = 2;
  ::google::protobuf::int64 votedfor() const;
  void set_votedfor(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:jraft.PersistState)
 private:
  void set_has_currentterm();
  void clear_has_currentterm();
  void set_has_votedfor();
  void clear_has_votedfor();

  // helper for ByteSizeLong()
  size_t RequiredFieldsByteSizeFallback() const;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::jraft::LogEntry > logs_;
  ::google::protobuf::int64 currentterm_;
  ::google::protobuf::int64 votedfor_;
  friend struct protobuf_state_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// LogEntry

// required .jraft.EntryType type = 1;
inline bool LogEntry::has_type() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void LogEntry::set_has_type() {
  _has_bits_[0] |= 0x00000008u;
}
inline void LogEntry::clear_has_type() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void LogEntry::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::jraft::EntryType LogEntry::type() const {
  // @@protoc_insertion_point(field_get:jraft.LogEntry.type)
  return static_cast< ::jraft::EntryType >(type_);
}
inline void LogEntry::set_type(::jraft::EntryType value) {
  assert(::jraft::EntryType_IsValid(value));
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:jraft.LogEntry.type)
}

// required int64 term = 2;
inline bool LogEntry::has_term() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void LogEntry::set_has_term() {
  _has_bits_[0] |= 0x00000002u;
}
inline void LogEntry::clear_has_term() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void LogEntry::clear_term() {
  term_ = GOOGLE_LONGLONG(0);
  clear_has_term();
}
inline ::google::protobuf::int64 LogEntry::term() const {
  // @@protoc_insertion_point(field_get:jraft.LogEntry.term)
  return term_;
}
inline void LogEntry::set_term(::google::protobuf::int64 value) {
  set_has_term();
  term_ = value;
  // @@protoc_insertion_point(field_set:jraft.LogEntry.term)
}

// required int64 index = 3;
inline bool LogEntry::has_index() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void LogEntry::set_has_index() {
  _has_bits_[0] |= 0x00000004u;
}
inline void LogEntry::clear_has_index() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void LogEntry::clear_index() {
  index_ = GOOGLE_LONGLONG(0);
  clear_has_index();
}
inline ::google::protobuf::int64 LogEntry::index() const {
  // @@protoc_insertion_point(field_get:jraft.LogEntry.index)
  return index_;
}
inline void LogEntry::set_index(::google::protobuf::int64 value) {
  set_has_index();
  index_ = value;
  // @@protoc_insertion_point(field_set:jraft.LogEntry.index)
}

// required string command = 4;
inline bool LogEntry::has_command() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void LogEntry::set_has_command() {
  _has_bits_[0] |= 0x00000001u;
}
inline void LogEntry::clear_has_command() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void LogEntry::clear_command() {
  command_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_command();
}
inline const ::std::string& LogEntry::command() const {
  // @@protoc_insertion_point(field_get:jraft.LogEntry.command)
  return command_.GetNoArena();
}
inline void LogEntry::set_command(const ::std::string& value) {
  set_has_command();
  command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:jraft.LogEntry.command)
}
#if LANG_CXX11
inline void LogEntry::set_command(::std::string&& value) {
  set_has_command();
  command_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:jraft.LogEntry.command)
}
#endif
inline void LogEntry::set_command(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_command();
  command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:jraft.LogEntry.command)
}
inline void LogEntry::set_command(const char* value, size_t size) {
  set_has_command();
  command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:jraft.LogEntry.command)
}
inline ::std::string* LogEntry::mutable_command() {
  set_has_command();
  // @@protoc_insertion_point(field_mutable:jraft.LogEntry.command)
  return command_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* LogEntry::release_command() {
  // @@protoc_insertion_point(field_release:jraft.LogEntry.command)
  clear_has_command();
  return command_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void LogEntry::set_allocated_command(::std::string* command) {
  if (command != NULL) {
    set_has_command();
  } else {
    clear_has_command();
  }
  command_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), command);
  // @@protoc_insertion_point(field_set_allocated:jraft.LogEntry.command)
}

// -------------------------------------------------------------------

// PersistState

// required int64 currentTerm = 1;
inline bool PersistState::has_currentterm() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PersistState::set_has_currentterm() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PersistState::clear_has_currentterm() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PersistState::clear_currentterm() {
  currentterm_ = GOOGLE_LONGLONG(0);
  clear_has_currentterm();
}
inline ::google::protobuf::int64 PersistState::currentterm() const {
  // @@protoc_insertion_point(field_get:jraft.PersistState.currentTerm)
  return currentterm_;
}
inline void PersistState::set_currentterm(::google::protobuf::int64 value) {
  set_has_currentterm();
  currentterm_ = value;
  // @@protoc_insertion_point(field_set:jraft.PersistState.currentTerm)
}

// required int64 votedFor = 2;
inline bool PersistState::has_votedfor() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PersistState::set_has_votedfor() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PersistState::clear_has_votedfor() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PersistState::clear_votedfor() {
  votedfor_ = GOOGLE_LONGLONG(0);
  clear_has_votedfor();
}
inline ::google::protobuf::int64 PersistState::votedfor() const {
  // @@protoc_insertion_point(field_get:jraft.PersistState.votedFor)
  return votedfor_;
}
inline void PersistState::set_votedfor(::google::protobuf::int64 value) {
  set_has_votedfor();
  votedfor_ = value;
  // @@protoc_insertion_point(field_set:jraft.PersistState.votedFor)
}

// repeated .jraft.LogEntry logs = 3;
inline int PersistState::logs_size() const {
  return logs_.size();
}
inline void PersistState::clear_logs() {
  logs_.Clear();
}
inline const ::jraft::LogEntry& PersistState::logs(int index) const {
  // @@protoc_insertion_point(field_get:jraft.PersistState.logs)
  return logs_.Get(index);
}
inline ::jraft::LogEntry* PersistState::mutable_logs(int index) {
  // @@protoc_insertion_point(field_mutable:jraft.PersistState.logs)
  return logs_.Mutable(index);
}
inline ::jraft::LogEntry* PersistState::add_logs() {
  // @@protoc_insertion_point(field_add:jraft.PersistState.logs)
  return logs_.Add();
}
inline ::google::protobuf::RepeatedPtrField< ::jraft::LogEntry >*
PersistState::mutable_logs() {
  // @@protoc_insertion_point(field_mutable_list:jraft.PersistState.logs)
  return &logs_;
}
inline const ::google::protobuf::RepeatedPtrField< ::jraft::LogEntry >&
PersistState::logs() const {
  // @@protoc_insertion_point(field_list:jraft.PersistState.logs)
  return logs_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace jraft

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::jraft::EntryType> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::jraft::EntryType>() {
  return ::jraft::EntryType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_state_2eproto__INCLUDED