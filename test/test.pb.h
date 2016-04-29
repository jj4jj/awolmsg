// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef PROTOBUF_test_2eproto__INCLUDED
#define PROTOBUF_test_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_test_2eproto();
void protobuf_AssignDesc_test_2eproto();
void protobuf_ShutdownFile_test_2eproto();

class MailAttachment;
class MailOption;
class MailMsg;

// ===================================================================

class MailAttachment : public ::google::protobuf::Message {
 public:
  MailAttachment();
  virtual ~MailAttachment();

  MailAttachment(const MailAttachment& from);

  inline MailAttachment& operator=(const MailAttachment& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MailAttachment& default_instance();

  void Swap(MailAttachment* other);

  // implements Message ----------------------------------------------

  MailAttachment* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MailAttachment& from);
  void MergeFrom(const MailAttachment& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::google::protobuf::int32 type() const;
  inline void set_type(::google::protobuf::int32 value);

  // optional bytes data = 2;
  inline bool has_data() const;
  inline void clear_data();
  static const int kDataFieldNumber = 2;
  inline const ::std::string& data() const;
  inline void set_data(const ::std::string& value);
  inline void set_data(const char* value);
  inline void set_data(const void* value, size_t size);
  inline ::std::string* mutable_data();
  inline ::std::string* release_data();
  inline void set_allocated_data(::std::string* data);

  // @@protoc_insertion_point(class_scope:MailAttachment)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_data();
  inline void clear_has_data();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* data_;
  ::google::protobuf::int32 type_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static MailAttachment* default_instance_;
};
// -------------------------------------------------------------------

class MailOption : public ::google::protobuf::Message {
 public:
  MailOption();
  virtual ~MailOption();

  MailOption(const MailOption& from);

  inline MailOption& operator=(const MailOption& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MailOption& default_instance();

  void Swap(MailOption* other);

  // implements Message ----------------------------------------------

  MailOption* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MailOption& from);
  void MergeFrom(const MailOption& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint32 expire_timestamp = 1;
  inline bool has_expire_timestamp() const;
  inline void clear_expire_timestamp();
  static const int kExpireTimestampFieldNumber = 1;
  inline ::google::protobuf::uint32 expire_timestamp() const;
  inline void set_expire_timestamp(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:MailOption)
 private:
  inline void set_has_expire_timestamp();
  inline void clear_has_expire_timestamp();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint32 expire_timestamp_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static MailOption* default_instance_;
};
// -------------------------------------------------------------------

class MailMsg : public ::google::protobuf::Message {
 public:
  MailMsg();
  virtual ~MailMsg();

  MailMsg(const MailMsg& from);

  inline MailMsg& operator=(const MailMsg& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MailMsg& default_instance();

  void Swap(MailMsg* other);

  // implements Message ----------------------------------------------

  MailMsg* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MailMsg& from);
  void MergeFrom(const MailMsg& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required uint64 to = 1;
  inline bool has_to() const;
  inline void clear_to();
  static const int kToFieldNumber = 1;
  inline ::google::protobuf::uint64 to() const;
  inline void set_to(::google::protobuf::uint64 value);

  // optional uint64 from = 2;
  inline bool has_from() const;
  inline void clear_from();
  static const int kFromFieldNumber = 2;
  inline ::google::protobuf::uint64 from() const;
  inline void set_from(::google::protobuf::uint64 value);

  // optional string sender = 3;
  inline bool has_sender() const;
  inline void clear_sender();
  static const int kSenderFieldNumber = 3;
  inline const ::std::string& sender() const;
  inline void set_sender(const ::std::string& value);
  inline void set_sender(const char* value);
  inline void set_sender(const char* value, size_t size);
  inline ::std::string* mutable_sender();
  inline ::std::string* release_sender();
  inline void set_allocated_sender(::std::string* sender);

  // optional string subject = 4;
  inline bool has_subject() const;
  inline void clear_subject();
  static const int kSubjectFieldNumber = 4;
  inline const ::std::string& subject() const;
  inline void set_subject(const ::std::string& value);
  inline void set_subject(const char* value);
  inline void set_subject(const char* value, size_t size);
  inline ::std::string* mutable_subject();
  inline ::std::string* release_subject();
  inline void set_allocated_subject(::std::string* subject);

  // optional string content = 5;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 5;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const char* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // repeated .MailAttachment attachements = 6;
  inline int attachements_size() const;
  inline void clear_attachements();
  static const int kAttachementsFieldNumber = 6;
  inline const ::MailAttachment& attachements(int index) const;
  inline ::MailAttachment* mutable_attachements(int index);
  inline ::MailAttachment* add_attachements();
  inline const ::google::protobuf::RepeatedPtrField< ::MailAttachment >&
      attachements() const;
  inline ::google::protobuf::RepeatedPtrField< ::MailAttachment >*
      mutable_attachements();

  // optional .MailOption option = 7;
  inline bool has_option() const;
  inline void clear_option();
  static const int kOptionFieldNumber = 7;
  inline const ::MailOption& option() const;
  inline ::MailOption* mutable_option();
  inline ::MailOption* release_option();
  inline void set_allocated_option(::MailOption* option);

  // @@protoc_insertion_point(class_scope:MailMsg)
 private:
  inline void set_has_to();
  inline void clear_has_to();
  inline void set_has_from();
  inline void clear_has_from();
  inline void set_has_sender();
  inline void clear_has_sender();
  inline void set_has_subject();
  inline void clear_has_subject();
  inline void set_has_content();
  inline void clear_has_content();
  inline void set_has_option();
  inline void clear_has_option();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 to_;
  ::google::protobuf::uint64 from_;
  ::std::string* sender_;
  ::std::string* subject_;
  ::std::string* content_;
  ::google::protobuf::RepeatedPtrField< ::MailAttachment > attachements_;
  ::MailOption* option_;
  friend void  protobuf_AddDesc_test_2eproto();
  friend void protobuf_AssignDesc_test_2eproto();
  friend void protobuf_ShutdownFile_test_2eproto();

  void InitAsDefaultInstance();
  static MailMsg* default_instance_;
};
// ===================================================================


// ===================================================================

// MailAttachment

// optional int32 type = 1;
inline bool MailAttachment::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MailAttachment::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MailAttachment::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MailAttachment::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::google::protobuf::int32 MailAttachment::type() const {
  // @@protoc_insertion_point(field_get:MailAttachment.type)
  return type_;
}
inline void MailAttachment::set_type(::google::protobuf::int32 value) {
  set_has_type();
  type_ = value;
  // @@protoc_insertion_point(field_set:MailAttachment.type)
}

// optional bytes data = 2;
inline bool MailAttachment::has_data() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MailAttachment::set_has_data() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MailAttachment::clear_has_data() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MailAttachment::clear_data() {
  if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    data_->clear();
  }
  clear_has_data();
}
inline const ::std::string& MailAttachment::data() const {
  // @@protoc_insertion_point(field_get:MailAttachment.data)
  return *data_;
}
inline void MailAttachment::set_data(const ::std::string& value) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    data_ = new ::std::string;
  }
  data_->assign(value);
  // @@protoc_insertion_point(field_set:MailAttachment.data)
}
inline void MailAttachment::set_data(const char* value) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    data_ = new ::std::string;
  }
  data_->assign(value);
  // @@protoc_insertion_point(field_set_char:MailAttachment.data)
}
inline void MailAttachment::set_data(const void* value, size_t size) {
  set_has_data();
  if (data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    data_ = new ::std::string;
  }
  data_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MailAttachment.data)
}
inline ::std::string* MailAttachment::mutable_data() {
  set_has_data();
  if (data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    data_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MailAttachment.data)
  return data_;
}
inline ::std::string* MailAttachment::release_data() {
  clear_has_data();
  if (data_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = data_;
    data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MailAttachment::set_allocated_data(::std::string* data) {
  if (data_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete data_;
  }
  if (data) {
    set_has_data();
    data_ = data;
  } else {
    clear_has_data();
    data_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MailAttachment.data)
}

// -------------------------------------------------------------------

// MailOption

// optional uint32 expire_timestamp = 1;
inline bool MailOption::has_expire_timestamp() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MailOption::set_has_expire_timestamp() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MailOption::clear_has_expire_timestamp() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MailOption::clear_expire_timestamp() {
  expire_timestamp_ = 0u;
  clear_has_expire_timestamp();
}
inline ::google::protobuf::uint32 MailOption::expire_timestamp() const {
  // @@protoc_insertion_point(field_get:MailOption.expire_timestamp)
  return expire_timestamp_;
}
inline void MailOption::set_expire_timestamp(::google::protobuf::uint32 value) {
  set_has_expire_timestamp();
  expire_timestamp_ = value;
  // @@protoc_insertion_point(field_set:MailOption.expire_timestamp)
}

// -------------------------------------------------------------------

// MailMsg

// required uint64 to = 1;
inline bool MailMsg::has_to() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void MailMsg::set_has_to() {
  _has_bits_[0] |= 0x00000001u;
}
inline void MailMsg::clear_has_to() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void MailMsg::clear_to() {
  to_ = GOOGLE_ULONGLONG(0);
  clear_has_to();
}
inline ::google::protobuf::uint64 MailMsg::to() const {
  // @@protoc_insertion_point(field_get:MailMsg.to)
  return to_;
}
inline void MailMsg::set_to(::google::protobuf::uint64 value) {
  set_has_to();
  to_ = value;
  // @@protoc_insertion_point(field_set:MailMsg.to)
}

// optional uint64 from = 2;
inline bool MailMsg::has_from() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void MailMsg::set_has_from() {
  _has_bits_[0] |= 0x00000002u;
}
inline void MailMsg::clear_has_from() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void MailMsg::clear_from() {
  from_ = GOOGLE_ULONGLONG(0);
  clear_has_from();
}
inline ::google::protobuf::uint64 MailMsg::from() const {
  // @@protoc_insertion_point(field_get:MailMsg.from)
  return from_;
}
inline void MailMsg::set_from(::google::protobuf::uint64 value) {
  set_has_from();
  from_ = value;
  // @@protoc_insertion_point(field_set:MailMsg.from)
}

// optional string sender = 3;
inline bool MailMsg::has_sender() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void MailMsg::set_has_sender() {
  _has_bits_[0] |= 0x00000004u;
}
inline void MailMsg::clear_has_sender() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void MailMsg::clear_sender() {
  if (sender_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sender_->clear();
  }
  clear_has_sender();
}
inline const ::std::string& MailMsg::sender() const {
  // @@protoc_insertion_point(field_get:MailMsg.sender)
  return *sender_;
}
inline void MailMsg::set_sender(const ::std::string& value) {
  set_has_sender();
  if (sender_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sender_ = new ::std::string;
  }
  sender_->assign(value);
  // @@protoc_insertion_point(field_set:MailMsg.sender)
}
inline void MailMsg::set_sender(const char* value) {
  set_has_sender();
  if (sender_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sender_ = new ::std::string;
  }
  sender_->assign(value);
  // @@protoc_insertion_point(field_set_char:MailMsg.sender)
}
inline void MailMsg::set_sender(const char* value, size_t size) {
  set_has_sender();
  if (sender_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sender_ = new ::std::string;
  }
  sender_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MailMsg.sender)
}
inline ::std::string* MailMsg::mutable_sender() {
  set_has_sender();
  if (sender_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    sender_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MailMsg.sender)
  return sender_;
}
inline ::std::string* MailMsg::release_sender() {
  clear_has_sender();
  if (sender_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = sender_;
    sender_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MailMsg::set_allocated_sender(::std::string* sender) {
  if (sender_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete sender_;
  }
  if (sender) {
    set_has_sender();
    sender_ = sender;
  } else {
    clear_has_sender();
    sender_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MailMsg.sender)
}

// optional string subject = 4;
inline bool MailMsg::has_subject() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void MailMsg::set_has_subject() {
  _has_bits_[0] |= 0x00000008u;
}
inline void MailMsg::clear_has_subject() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void MailMsg::clear_subject() {
  if (subject_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    subject_->clear();
  }
  clear_has_subject();
}
inline const ::std::string& MailMsg::subject() const {
  // @@protoc_insertion_point(field_get:MailMsg.subject)
  return *subject_;
}
inline void MailMsg::set_subject(const ::std::string& value) {
  set_has_subject();
  if (subject_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    subject_ = new ::std::string;
  }
  subject_->assign(value);
  // @@protoc_insertion_point(field_set:MailMsg.subject)
}
inline void MailMsg::set_subject(const char* value) {
  set_has_subject();
  if (subject_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    subject_ = new ::std::string;
  }
  subject_->assign(value);
  // @@protoc_insertion_point(field_set_char:MailMsg.subject)
}
inline void MailMsg::set_subject(const char* value, size_t size) {
  set_has_subject();
  if (subject_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    subject_ = new ::std::string;
  }
  subject_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MailMsg.subject)
}
inline ::std::string* MailMsg::mutable_subject() {
  set_has_subject();
  if (subject_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    subject_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MailMsg.subject)
  return subject_;
}
inline ::std::string* MailMsg::release_subject() {
  clear_has_subject();
  if (subject_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = subject_;
    subject_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MailMsg::set_allocated_subject(::std::string* subject) {
  if (subject_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete subject_;
  }
  if (subject) {
    set_has_subject();
    subject_ = subject;
  } else {
    clear_has_subject();
    subject_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MailMsg.subject)
}

// optional string content = 5;
inline bool MailMsg::has_content() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void MailMsg::set_has_content() {
  _has_bits_[0] |= 0x00000010u;
}
inline void MailMsg::clear_has_content() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void MailMsg::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& MailMsg::content() const {
  // @@protoc_insertion_point(field_get:MailMsg.content)
  return *content_;
}
inline void MailMsg::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:MailMsg.content)
}
inline void MailMsg::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:MailMsg.content)
}
inline void MailMsg::set_content(const char* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:MailMsg.content)
}
inline ::std::string* MailMsg::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:MailMsg.content)
  return content_;
}
inline ::std::string* MailMsg::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void MailMsg::set_allocated_content(::std::string* content) {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete content_;
  }
  if (content) {
    set_has_content();
    content_ = content;
  } else {
    clear_has_content();
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:MailMsg.content)
}

// repeated .MailAttachment attachements = 6;
inline int MailMsg::attachements_size() const {
  return attachements_.size();
}
inline void MailMsg::clear_attachements() {
  attachements_.Clear();
}
inline const ::MailAttachment& MailMsg::attachements(int index) const {
  // @@protoc_insertion_point(field_get:MailMsg.attachements)
  return attachements_.Get(index);
}
inline ::MailAttachment* MailMsg::mutable_attachements(int index) {
  // @@protoc_insertion_point(field_mutable:MailMsg.attachements)
  return attachements_.Mutable(index);
}
inline ::MailAttachment* MailMsg::add_attachements() {
  // @@protoc_insertion_point(field_add:MailMsg.attachements)
  return attachements_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::MailAttachment >&
MailMsg::attachements() const {
  // @@protoc_insertion_point(field_list:MailMsg.attachements)
  return attachements_;
}
inline ::google::protobuf::RepeatedPtrField< ::MailAttachment >*
MailMsg::mutable_attachements() {
  // @@protoc_insertion_point(field_mutable_list:MailMsg.attachements)
  return &attachements_;
}

// optional .MailOption option = 7;
inline bool MailMsg::has_option() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void MailMsg::set_has_option() {
  _has_bits_[0] |= 0x00000040u;
}
inline void MailMsg::clear_has_option() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void MailMsg::clear_option() {
  if (option_ != NULL) option_->::MailOption::Clear();
  clear_has_option();
}
inline const ::MailOption& MailMsg::option() const {
  // @@protoc_insertion_point(field_get:MailMsg.option)
  return option_ != NULL ? *option_ : *default_instance_->option_;
}
inline ::MailOption* MailMsg::mutable_option() {
  set_has_option();
  if (option_ == NULL) option_ = new ::MailOption;
  // @@protoc_insertion_point(field_mutable:MailMsg.option)
  return option_;
}
inline ::MailOption* MailMsg::release_option() {
  clear_has_option();
  ::MailOption* temp = option_;
  option_ = NULL;
  return temp;
}
inline void MailMsg::set_allocated_option(::MailOption* option) {
  delete option_;
  option_ = option;
  if (option) {
    set_has_option();
  } else {
    clear_has_option();
  }
  // @@protoc_insertion_point(field_set_allocated:MailMsg.option)
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_test_2eproto__INCLUDED
