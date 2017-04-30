// Generated by the protocCSUTILfer compiler.  DO NOT EDIT!
// source: tree_model.proto

#ifndef PROTOBUF_tree_5fmodel_2eproto__INCLUDED
#define PROTOBUF_tree_5fmodel_2eproto__INCLUDED

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

namespace tree_model {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_tree_5fmodel_2eproto();
void protobuf_AssignDesc_tree_5fmodel_2eproto();
void protobuf_ShutdownFile_tree_5fmodel_2eproto();

class TreeNodeHead;
class TreeNode;
class DumpNode;

// ===================================================================

class TreeNodeHead : public ::google::protobuf::Message {
 public:
  TreeNodeHead();
  virtual ~TreeNodeHead();

  TreeNodeHead(const TreeNodeHead& from);

  inline TreeNodeHead& operator=(const TreeNodeHead& from) {
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
  static const TreeNodeHead& default_instance();

  void Swap(TreeNodeHead* other);

  // implements Message ----------------------------------------------

  TreeNodeHead* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TreeNodeHead& from);
  void MergeFrom(const TreeNodeHead& from);
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

  // optional uint64 trx_id = 1;
  inline bool has_trx_id() const;
  inline void clear_trx_id();
  static const int kTrxIdFieldNumber = 1;
  inline ::google::protobuf::uint64 trx_id() const;
  inline void set_trx_id(::google::protobuf::uint64 value);

  // optional bytes name = 11;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 11;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const void* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // optional bytes value = 12;
  inline bool has_value() const;
  inline void clear_value();
  static const int kValueFieldNumber = 12;
  inline const ::std::string& value() const;
  inline void set_value(const ::std::string& value);
  inline void set_value(const char* value);
  inline void set_value(const void* value, size_t size);
  inline ::std::string* mutable_value();
  inline ::std::string* release_value();
  inline void set_allocated_value(::std::string* value);

  // optional int64 create_time = 13;
  inline bool has_create_time() const;
  inline void clear_create_time();
  static const int kCreateTimeFieldNumber = 13;
  inline ::google::protobuf::int64 create_time() const;
  inline void set_create_time(::google::protobuf::int64 value);

  // optional int64 modify_time = 14;
  inline bool has_modify_time() const;
  inline void clear_modify_time();
  static const int kModifyTimeFieldNumber = 14;
  inline ::google::protobuf::int64 modify_time() const;
  inline void set_modify_time(::google::protobuf::int64 value);

  // optional int64 version = 15;
  inline bool has_version() const;
  inline void clear_version();
  static const int kVersionFieldNumber = 15;
  inline ::google::protobuf::int64 version() const;
  inline void set_version(::google::protobuf::int64 value);

  // optional int64 children_version = 16;
  inline bool has_children_version() const;
  inline void clear_children_version();
  static const int kChildrenVersionFieldNumber = 16;
  inline ::google::protobuf::int64 children_version() const;
  inline void set_children_version(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:tree_model.TreeNodeHead)
 private:
  inline void set_has_trx_id();
  inline void clear_has_trx_id();
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_value();
  inline void clear_has_value();
  inline void set_has_create_time();
  inline void clear_has_create_time();
  inline void set_has_modify_time();
  inline void clear_has_modify_time();
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_children_version();
  inline void clear_has_children_version();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 trx_id_;
  ::std::string* name_;
  ::std::string* value_;
  ::google::protobuf::int64 create_time_;
  ::google::protobuf::int64 modify_time_;
  ::google::protobuf::int64 version_;
  ::google::protobuf::int64 children_version_;
  friend void  protobuf_AddDesc_tree_5fmodel_2eproto();
  friend void protobuf_AssignDesc_tree_5fmodel_2eproto();
  friend void protobuf_ShutdownFile_tree_5fmodel_2eproto();

  void InitAsDefaultInstance();
  static TreeNodeHead* default_instance_;
};
// -------------------------------------------------------------------

class TreeNode : public ::google::protobuf::Message {
 public:
  TreeNode();
  virtual ~TreeNode();

  TreeNode(const TreeNode& from);

  inline TreeNode& operator=(const TreeNode& from) {
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
  static const TreeNode& default_instance();

  void Swap(TreeNode* other);

  // implements Message ----------------------------------------------

  TreeNode* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TreeNode& from);
  void MergeFrom(const TreeNode& from);
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

  // optional .tree_model.TreeNodeHead head = 1;
  inline bool has_head() const;
  inline void clear_head();
  static const int kHeadFieldNumber = 1;
  inline const ::tree_model::TreeNodeHead& head() const;
  inline ::tree_model::TreeNodeHead* mutable_head();
  inline ::tree_model::TreeNodeHead* release_head();
  inline void set_allocated_head(::tree_model::TreeNodeHead* head);

  // repeated .tree_model.TreeNode children_nodes = 2;
  inline int children_nodes_size() const;
  inline void clear_children_nodes();
  static const int kChildrenNodesFieldNumber = 2;
  inline const ::tree_model::TreeNode& children_nodes(int index) const;
  inline ::tree_model::TreeNode* mutable_children_nodes(int index);
  inline ::tree_model::TreeNode* add_children_nodes();
  inline const ::google::protobuf::RepeatedPtrField< ::tree_model::TreeNode >&
      children_nodes() const;
  inline ::google::protobuf::RepeatedPtrField< ::tree_model::TreeNode >*
      mutable_children_nodes();

  // @@protoc_insertion_point(class_scope:tree_model.TreeNode)
 private:
  inline void set_has_head();
  inline void clear_has_head();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::tree_model::TreeNodeHead* head_;
  ::google::protobuf::RepeatedPtrField< ::tree_model::TreeNode > children_nodes_;
  friend void  protobuf_AddDesc_tree_5fmodel_2eproto();
  friend void protobuf_AssignDesc_tree_5fmodel_2eproto();
  friend void protobuf_ShutdownFile_tree_5fmodel_2eproto();

  void InitAsDefaultInstance();
  static TreeNode* default_instance_;
};
// -------------------------------------------------------------------

class DumpNode : public ::google::protobuf::Message {
 public:
  DumpNode();
  virtual ~DumpNode();

  DumpNode(const DumpNode& from);

  inline DumpNode& operator=(const DumpNode& from) {
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
  static const DumpNode& default_instance();

  void Swap(DumpNode* other);

  // implements Message ----------------------------------------------

  DumpNode* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DumpNode& from);
  void MergeFrom(const DumpNode& from);
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

  // optional uint64 trx_id = 1;
  inline bool has_trx_id() const;
  inline void clear_trx_id();
  static const int kTrxIdFieldNumber = 1;
  inline ::google::protobuf::uint64 trx_id() const;
  inline void set_trx_id(::google::protobuf::uint64 value);

  // optional bytes content = 2;
  inline bool has_content() const;
  inline void clear_content();
  static const int kContentFieldNumber = 2;
  inline const ::std::string& content() const;
  inline void set_content(const ::std::string& value);
  inline void set_content(const char* value);
  inline void set_content(const void* value, size_t size);
  inline ::std::string* mutable_content();
  inline ::std::string* release_content();
  inline void set_allocated_content(::std::string* content);

  // @@protoc_insertion_point(class_scope:tree_model.DumpNode)
 private:
  inline void set_has_trx_id();
  inline void clear_has_trx_id();
  inline void set_has_content();
  inline void clear_has_content();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::uint64 trx_id_;
  ::std::string* content_;
  friend void  protobuf_AddDesc_tree_5fmodel_2eproto();
  friend void protobuf_AssignDesc_tree_5fmodel_2eproto();
  friend void protobuf_ShutdownFile_tree_5fmodel_2eproto();

  void InitAsDefaultInstance();
  static DumpNode* default_instance_;
};
// ===================================================================


// ===================================================================

// TreeNodeHead

// optional uint64 trx_id = 1;
inline bool TreeNodeHead::has_trx_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TreeNodeHead::set_has_trx_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TreeNodeHead::clear_has_trx_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TreeNodeHead::clear_trx_id() {
  trx_id_ = GOOGLE_ULONGLONG(0);
  clear_has_trx_id();
}
inline ::google::protobuf::uint64 TreeNodeHead::trx_id() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.trx_id)
  return trx_id_;
}
inline void TreeNodeHead::set_trx_id(::google::protobuf::uint64 value) {
  set_has_trx_id();
  trx_id_ = value;
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.trx_id)
}

// optional bytes name = 11;
inline bool TreeNodeHead::has_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void TreeNodeHead::set_has_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void TreeNodeHead::clear_has_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void TreeNodeHead::clear_name() {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& TreeNodeHead::name() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.name)
  return *name_;
}
inline void TreeNodeHead::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.name)
}
inline void TreeNodeHead::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(value);
  // @@protoc_insertion_point(field_set_char:tree_model.TreeNodeHead.name)
}
inline void TreeNodeHead::set_name(const void* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:tree_model.TreeNodeHead.name)
}
inline ::std::string* TreeNodeHead::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    name_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:tree_model.TreeNodeHead.name)
  return name_;
}
inline ::std::string* TreeNodeHead::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void TreeNodeHead::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:tree_model.TreeNodeHead.name)
}

// optional bytes value = 12;
inline bool TreeNodeHead::has_value() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void TreeNodeHead::set_has_value() {
  _has_bits_[0] |= 0x00000004u;
}
inline void TreeNodeHead::clear_has_value() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void TreeNodeHead::clear_value() {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_->clear();
  }
  clear_has_value();
}
inline const ::std::string& TreeNodeHead::value() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.value)
  return *value_;
}
inline void TreeNodeHead::set_value(const ::std::string& value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.value)
}
inline void TreeNodeHead::set_value(const char* value) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(value);
  // @@protoc_insertion_point(field_set_char:tree_model.TreeNodeHead.value)
}
inline void TreeNodeHead::set_value(const void* value, size_t size) {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  value_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:tree_model.TreeNodeHead.value)
}
inline ::std::string* TreeNodeHead::mutable_value() {
  set_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    value_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:tree_model.TreeNodeHead.value)
  return value_;
}
inline ::std::string* TreeNodeHead::release_value() {
  clear_has_value();
  if (value_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = value_;
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void TreeNodeHead::set_allocated_value(::std::string* value) {
  if (value_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete value_;
  }
  if (value) {
    set_has_value();
    value_ = value;
  } else {
    clear_has_value();
    value_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:tree_model.TreeNodeHead.value)
}

// optional int64 create_time = 13;
inline bool TreeNodeHead::has_create_time() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void TreeNodeHead::set_has_create_time() {
  _has_bits_[0] |= 0x00000008u;
}
inline void TreeNodeHead::clear_has_create_time() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void TreeNodeHead::clear_create_time() {
  create_time_ = GOOGLE_LONGLONG(0);
  clear_has_create_time();
}
inline ::google::protobuf::int64 TreeNodeHead::create_time() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.create_time)
  return create_time_;
}
inline void TreeNodeHead::set_create_time(::google::protobuf::int64 value) {
  set_has_create_time();
  create_time_ = value;
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.create_time)
}

// optional int64 modify_time = 14;
inline bool TreeNodeHead::has_modify_time() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void TreeNodeHead::set_has_modify_time() {
  _has_bits_[0] |= 0x00000010u;
}
inline void TreeNodeHead::clear_has_modify_time() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void TreeNodeHead::clear_modify_time() {
  modify_time_ = GOOGLE_LONGLONG(0);
  clear_has_modify_time();
}
inline ::google::protobuf::int64 TreeNodeHead::modify_time() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.modify_time)
  return modify_time_;
}
inline void TreeNodeHead::set_modify_time(::google::protobuf::int64 value) {
  set_has_modify_time();
  modify_time_ = value;
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.modify_time)
}

// optional int64 version = 15;
inline bool TreeNodeHead::has_version() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void TreeNodeHead::set_has_version() {
  _has_bits_[0] |= 0x00000020u;
}
inline void TreeNodeHead::clear_has_version() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void TreeNodeHead::clear_version() {
  version_ = GOOGLE_LONGLONG(0);
  clear_has_version();
}
inline ::google::protobuf::int64 TreeNodeHead::version() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.version)
  return version_;
}
inline void TreeNodeHead::set_version(::google::protobuf::int64 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.version)
}

// optional int64 children_version = 16;
inline bool TreeNodeHead::has_children_version() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void TreeNodeHead::set_has_children_version() {
  _has_bits_[0] |= 0x00000040u;
}
inline void TreeNodeHead::clear_has_children_version() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void TreeNodeHead::clear_children_version() {
  children_version_ = GOOGLE_LONGLONG(0);
  clear_has_children_version();
}
inline ::google::protobuf::int64 TreeNodeHead::children_version() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNodeHead.children_version)
  return children_version_;
}
inline void TreeNodeHead::set_children_version(::google::protobuf::int64 value) {
  set_has_children_version();
  children_version_ = value;
  // @@protoc_insertion_point(field_set:tree_model.TreeNodeHead.children_version)
}

// -------------------------------------------------------------------

// TreeNode

// optional .tree_model.TreeNodeHead head = 1;
inline bool TreeNode::has_head() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TreeNode::set_has_head() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TreeNode::clear_has_head() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TreeNode::clear_head() {
  if (head_ != NULL) head_->::tree_model::TreeNodeHead::Clear();
  clear_has_head();
}
inline const ::tree_model::TreeNodeHead& TreeNode::head() const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNode.head)
  return head_ != NULL ? *head_ : *default_instance_->head_;
}
inline ::tree_model::TreeNodeHead* TreeNode::mutable_head() {
  set_has_head();
  if (head_ == NULL) head_ = new ::tree_model::TreeNodeHead;
  // @@protoc_insertion_point(field_mutable:tree_model.TreeNode.head)
  return head_;
}
inline ::tree_model::TreeNodeHead* TreeNode::release_head() {
  clear_has_head();
  ::tree_model::TreeNodeHead* temp = head_;
  head_ = NULL;
  return temp;
}
inline void TreeNode::set_allocated_head(::tree_model::TreeNodeHead* head) {
  delete head_;
  head_ = head;
  if (head) {
    set_has_head();
  } else {
    clear_has_head();
  }
  // @@protoc_insertion_point(field_set_allocated:tree_model.TreeNode.head)
}

// repeated .tree_model.TreeNode children_nodes = 2;
inline int TreeNode::children_nodes_size() const {
  return children_nodes_.size();
}
inline void TreeNode::clear_children_nodes() {
  children_nodes_.Clear();
}
inline const ::tree_model::TreeNode& TreeNode::children_nodes(int index) const {
  // @@protoc_insertion_point(field_get:tree_model.TreeNode.children_nodes)
  return children_nodes_.Get(index);
}
inline ::tree_model::TreeNode* TreeNode::mutable_children_nodes(int index) {
  // @@protoc_insertion_point(field_mutable:tree_model.TreeNode.children_nodes)
  return children_nodes_.Mutable(index);
}
inline ::tree_model::TreeNode* TreeNode::add_children_nodes() {
  // @@protoc_insertion_point(field_add:tree_model.TreeNode.children_nodes)
  return children_nodes_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tree_model::TreeNode >&
TreeNode::children_nodes() const {
  // @@protoc_insertion_point(field_list:tree_model.TreeNode.children_nodes)
  return children_nodes_;
}
inline ::google::protobuf::RepeatedPtrField< ::tree_model::TreeNode >*
TreeNode::mutable_children_nodes() {
  // @@protoc_insertion_point(field_mutable_list:tree_model.TreeNode.children_nodes)
  return &children_nodes_;
}

// -------------------------------------------------------------------

// DumpNode

// optional uint64 trx_id = 1;
inline bool DumpNode::has_trx_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void DumpNode::set_has_trx_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void DumpNode::clear_has_trx_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void DumpNode::clear_trx_id() {
  trx_id_ = GOOGLE_ULONGLONG(0);
  clear_has_trx_id();
}
inline ::google::protobuf::uint64 DumpNode::trx_id() const {
  // @@protoc_insertion_point(field_get:tree_model.DumpNode.trx_id)
  return trx_id_;
}
inline void DumpNode::set_trx_id(::google::protobuf::uint64 value) {
  set_has_trx_id();
  trx_id_ = value;
  // @@protoc_insertion_point(field_set:tree_model.DumpNode.trx_id)
}

// optional bytes content = 2;
inline bool DumpNode::has_content() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void DumpNode::set_has_content() {
  _has_bits_[0] |= 0x00000002u;
}
inline void DumpNode::clear_has_content() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void DumpNode::clear_content() {
  if (content_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_->clear();
  }
  clear_has_content();
}
inline const ::std::string& DumpNode::content() const {
  // @@protoc_insertion_point(field_get:tree_model.DumpNode.content)
  return *content_;
}
inline void DumpNode::set_content(const ::std::string& value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set:tree_model.DumpNode.content)
}
inline void DumpNode::set_content(const char* value) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(value);
  // @@protoc_insertion_point(field_set_char:tree_model.DumpNode.content)
}
inline void DumpNode::set_content(const void* value, size_t size) {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  content_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:tree_model.DumpNode.content)
}
inline ::std::string* DumpNode::mutable_content() {
  set_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    content_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:tree_model.DumpNode.content)
  return content_;
}
inline ::std::string* DumpNode::release_content() {
  clear_has_content();
  if (content_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = content_;
    content_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void DumpNode::set_allocated_content(::std::string* content) {
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
  // @@protoc_insertion_point(field_set_allocated:tree_model.DumpNode.content)
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace tree_model

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_tree_5fmodel_2eproto__INCLUDED