// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Msg.proto

#include "Msg.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_Msg_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_block_msg_t_Msg_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_Msg_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_txn_msg_t_Msg_2eproto;
class txn_msg_tDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<txn_msg_t> _instance;
} _txn_msg_t_default_instance_;
class block_msg_tDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<block_msg_t> _instance;
} _block_msg_t_default_instance_;
class bc_msg_tDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<bc_msg_t> _instance;
} _bc_msg_t_default_instance_;
static void InitDefaultsscc_info_bc_msg_t_Msg_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_bc_msg_t_default_instance_;
    new (ptr) ::bc_msg_t();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::bc_msg_t::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_bc_msg_t_Msg_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_bc_msg_t_Msg_2eproto}, {
      &scc_info_block_msg_t_Msg_2eproto.base,}};

static void InitDefaultsscc_info_block_msg_t_Msg_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_block_msg_t_default_instance_;
    new (ptr) ::block_msg_t();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::block_msg_t::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_block_msg_t_Msg_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_block_msg_t_Msg_2eproto}, {
      &scc_info_txn_msg_t_Msg_2eproto.base,}};

static void InitDefaultsscc_info_txn_msg_t_Msg_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_txn_msg_t_default_instance_;
    new (ptr) ::txn_msg_t();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::txn_msg_t::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_txn_msg_t_Msg_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_txn_msg_t_Msg_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_Msg_2eproto[3];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_Msg_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_Msg_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_Msg_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::txn_msg_t, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::txn_msg_t, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::txn_msg_t, sender_id_),
  PROTOBUF_FIELD_OFFSET(::txn_msg_t, recver_id_),
  PROTOBUF_FIELD_OFFSET(::txn_msg_t, amount_),
  0,
  1,
  2,
  PROTOBUF_FIELD_OFFSET(::block_msg_t, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::block_msg_t, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::block_msg_t, term_),
  PROTOBUF_FIELD_OFFSET(::block_msg_t, phash_),
  PROTOBUF_FIELD_OFFSET(::block_msg_t, nonce_),
  PROTOBUF_FIELD_OFFSET(::block_msg_t, txn_),
  3,
  0,
  1,
  2,
  PROTOBUF_FIELD_OFFSET(::bc_msg_t, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::bc_msg_t, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::bc_msg_t, committed_index_),
  PROTOBUF_FIELD_OFFSET(::bc_msg_t, blocks_),
  0,
  ~0u,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, sizeof(::txn_msg_t)},
  { 11, 20, sizeof(::block_msg_t)},
  { 24, 31, sizeof(::bc_msg_t)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_txn_msg_t_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_block_msg_t_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_bc_msg_t_default_instance_),
};

const char descriptor_table_protodef_Msg_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\tMsg.proto\"A\n\ttxn_msg_t\022\021\n\tsender_id\030\001 "
  "\002(\r\022\021\n\trecver_id\030\002 \002(\r\022\016\n\006amount\030\003 \002(\002\"R"
  "\n\013block_msg_t\022\014\n\004term\030\001 \002(\r\022\r\n\005phash\030\002 \002"
  "(\t\022\r\n\005nonce\030\003 \002(\t\022\027\n\003txn\030\004 \002(\0132\n.txn_msg"
  "_t\"A\n\010bc_msg_t\022\027\n\017committed_index\030\001 \002(\r\022"
  "\034\n\006blocks\030\003 \003(\0132\014.block_msg_t"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_Msg_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_Msg_2eproto_sccs[3] = {
  &scc_info_bc_msg_t_Msg_2eproto.base,
  &scc_info_block_msg_t_Msg_2eproto.base,
  &scc_info_txn_msg_t_Msg_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_Msg_2eproto_once;
static bool descriptor_table_Msg_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Msg_2eproto = {
  &descriptor_table_Msg_2eproto_initialized, descriptor_table_protodef_Msg_2eproto, "Msg.proto", 229,
  &descriptor_table_Msg_2eproto_once, descriptor_table_Msg_2eproto_sccs, descriptor_table_Msg_2eproto_deps, 3, 0,
  schemas, file_default_instances, TableStruct_Msg_2eproto::offsets,
  file_level_metadata_Msg_2eproto, 3, file_level_enum_descriptors_Msg_2eproto, file_level_service_descriptors_Msg_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_Msg_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_Msg_2eproto), true);

// ===================================================================

void txn_msg_t::InitAsDefaultInstance() {
}
class txn_msg_t::_Internal {
 public:
  using HasBits = decltype(std::declval<txn_msg_t>()._has_bits_);
  static void set_has_sender_id(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_recver_id(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_amount(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

txn_msg_t::txn_msg_t()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:txn_msg_t)
}
txn_msg_t::txn_msg_t(const txn_msg_t& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&sender_id_, &from.sender_id_,
    static_cast<size_t>(reinterpret_cast<char*>(&amount_) -
    reinterpret_cast<char*>(&sender_id_)) + sizeof(amount_));
  // @@protoc_insertion_point(copy_constructor:txn_msg_t)
}

void txn_msg_t::SharedCtor() {
  ::memset(&sender_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&amount_) -
      reinterpret_cast<char*>(&sender_id_)) + sizeof(amount_));
}

txn_msg_t::~txn_msg_t() {
  // @@protoc_insertion_point(destructor:txn_msg_t)
  SharedDtor();
}

void txn_msg_t::SharedDtor() {
}

void txn_msg_t::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const txn_msg_t& txn_msg_t::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_txn_msg_t_Msg_2eproto.base);
  return *internal_default_instance();
}


void txn_msg_t::Clear() {
// @@protoc_insertion_point(message_clear_start:txn_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    ::memset(&sender_id_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&amount_) -
        reinterpret_cast<char*>(&sender_id_)) + sizeof(amount_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* txn_msg_t::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // required uint32 sender_id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_sender_id(&has_bits);
          sender_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required uint32 recver_id = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_recver_id(&has_bits);
          recver_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required float amount = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 29)) {
          _Internal::set_has_amount(&has_bits);
          amount_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* txn_msg_t::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:txn_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required uint32 sender_id = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_sender_id(), target);
  }

  // required uint32 recver_id = 2;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_recver_id(), target);
  }

  // required float amount = 3;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteFloatToArray(3, this->_internal_amount(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:txn_msg_t)
  return target;
}

size_t txn_msg_t::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:txn_msg_t)
  size_t total_size = 0;

  if (_internal_has_sender_id()) {
    // required uint32 sender_id = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_sender_id());
  }

  if (_internal_has_recver_id()) {
    // required uint32 recver_id = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_recver_id());
  }

  if (_internal_has_amount()) {
    // required float amount = 3;
    total_size += 1 + 4;
  }

  return total_size;
}
size_t txn_msg_t::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:txn_msg_t)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x00000007) ^ 0x00000007) == 0) {  // All required fields are present.
    // required uint32 sender_id = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_sender_id());

    // required uint32 recver_id = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_recver_id());

    // required float amount = 3;
    total_size += 1 + 4;

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void txn_msg_t::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:txn_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  const txn_msg_t* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<txn_msg_t>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:txn_msg_t)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:txn_msg_t)
    MergeFrom(*source);
  }
}

void txn_msg_t::MergeFrom(const txn_msg_t& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:txn_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      sender_id_ = from.sender_id_;
    }
    if (cached_has_bits & 0x00000002u) {
      recver_id_ = from.recver_id_;
    }
    if (cached_has_bits & 0x00000004u) {
      amount_ = from.amount_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void txn_msg_t::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:txn_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void txn_msg_t::CopyFrom(const txn_msg_t& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:txn_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool txn_msg_t::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;
  return true;
}

void txn_msg_t::InternalSwap(txn_msg_t* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(sender_id_, other->sender_id_);
  swap(recver_id_, other->recver_id_);
  swap(amount_, other->amount_);
}

::PROTOBUF_NAMESPACE_ID::Metadata txn_msg_t::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void block_msg_t::InitAsDefaultInstance() {
  ::_block_msg_t_default_instance_._instance.get_mutable()->txn_ = const_cast< ::txn_msg_t*>(
      ::txn_msg_t::internal_default_instance());
}
class block_msg_t::_Internal {
 public:
  using HasBits = decltype(std::declval<block_msg_t>()._has_bits_);
  static void set_has_term(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
  static void set_has_phash(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_nonce(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static const ::txn_msg_t& txn(const block_msg_t* msg);
  static void set_has_txn(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
};

const ::txn_msg_t&
block_msg_t::_Internal::txn(const block_msg_t* msg) {
  return *msg->txn_;
}
block_msg_t::block_msg_t()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:block_msg_t)
}
block_msg_t::block_msg_t(const block_msg_t& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  phash_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_phash()) {
    phash_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.phash_);
  }
  nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_nonce()) {
    nonce_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.nonce_);
  }
  if (from._internal_has_txn()) {
    txn_ = new ::txn_msg_t(*from.txn_);
  } else {
    txn_ = nullptr;
  }
  term_ = from.term_;
  // @@protoc_insertion_point(copy_constructor:block_msg_t)
}

void block_msg_t::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_block_msg_t_Msg_2eproto.base);
  phash_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  nonce_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&txn_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&term_) -
      reinterpret_cast<char*>(&txn_)) + sizeof(term_));
}

block_msg_t::~block_msg_t() {
  // @@protoc_insertion_point(destructor:block_msg_t)
  SharedDtor();
}

void block_msg_t::SharedDtor() {
  phash_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  nonce_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete txn_;
}

void block_msg_t::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const block_msg_t& block_msg_t::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_block_msg_t_Msg_2eproto.base);
  return *internal_default_instance();
}


void block_msg_t::Clear() {
// @@protoc_insertion_point(message_clear_start:block_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000007u) {
    if (cached_has_bits & 0x00000001u) {
      phash_.ClearNonDefaultToEmptyNoArena();
    }
    if (cached_has_bits & 0x00000002u) {
      nonce_.ClearNonDefaultToEmptyNoArena();
    }
    if (cached_has_bits & 0x00000004u) {
      GOOGLE_DCHECK(txn_ != nullptr);
      txn_->Clear();
    }
  }
  term_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* block_msg_t::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // required uint32 term = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_term(&has_bits);
          term_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required string phash = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_phash();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "block_msg_t.phash");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required string nonce = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_nonce();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          #ifndef NDEBUG
          ::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "block_msg_t.nonce");
          #endif  // !NDEBUG
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // required .txn_msg_t txn = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr = ctx->ParseMessage(_internal_mutable_txn(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* block_msg_t::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:block_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required uint32 term = 1;
  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_term(), target);
  }

  // required string phash = 2;
  if (cached_has_bits & 0x00000001u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_phash().data(), static_cast<int>(this->_internal_phash().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "block_msg_t.phash");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_phash(), target);
  }

  // required string nonce = 3;
  if (cached_has_bits & 0x00000002u) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::VerifyUTF8StringNamedField(
      this->_internal_nonce().data(), static_cast<int>(this->_internal_nonce().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::SERIALIZE,
      "block_msg_t.nonce");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_nonce(), target);
  }

  // required .txn_msg_t txn = 4;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        4, _Internal::txn(this), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:block_msg_t)
  return target;
}

size_t block_msg_t::RequiredFieldsByteSizeFallback() const {
// @@protoc_insertion_point(required_fields_byte_size_fallback_start:block_msg_t)
  size_t total_size = 0;

  if (_internal_has_phash()) {
    // required string phash = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_phash());
  }

  if (_internal_has_nonce()) {
    // required string nonce = 3;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_nonce());
  }

  if (_internal_has_txn()) {
    // required .txn_msg_t txn = 4;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *txn_);
  }

  if (_internal_has_term()) {
    // required uint32 term = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_term());
  }

  return total_size;
}
size_t block_msg_t::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:block_msg_t)
  size_t total_size = 0;

  if (((_has_bits_[0] & 0x0000000f) ^ 0x0000000f) == 0) {  // All required fields are present.
    // required string phash = 2;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_phash());

    // required string nonce = 3;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_nonce());

    // required .txn_msg_t txn = 4;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *txn_);

    // required uint32 term = 1;
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_term());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void block_msg_t::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:block_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  const block_msg_t* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<block_msg_t>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:block_msg_t)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:block_msg_t)
    MergeFrom(*source);
  }
}

void block_msg_t::MergeFrom(const block_msg_t& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:block_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    if (cached_has_bits & 0x00000001u) {
      _has_bits_[0] |= 0x00000001u;
      phash_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.phash_);
    }
    if (cached_has_bits & 0x00000002u) {
      _has_bits_[0] |= 0x00000002u;
      nonce_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.nonce_);
    }
    if (cached_has_bits & 0x00000004u) {
      _internal_mutable_txn()->::txn_msg_t::MergeFrom(from._internal_txn());
    }
    if (cached_has_bits & 0x00000008u) {
      term_ = from.term_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void block_msg_t::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:block_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void block_msg_t::CopyFrom(const block_msg_t& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:block_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool block_msg_t::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000000f) != 0x0000000f) return false;
  if (_internal_has_txn()) {
    if (!txn_->IsInitialized()) return false;
  }
  return true;
}

void block_msg_t::InternalSwap(block_msg_t* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  phash_.Swap(&other->phash_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  nonce_.Swap(&other->nonce_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(txn_, other->txn_);
  swap(term_, other->term_);
}

::PROTOBUF_NAMESPACE_ID::Metadata block_msg_t::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void bc_msg_t::InitAsDefaultInstance() {
}
class bc_msg_t::_Internal {
 public:
  using HasBits = decltype(std::declval<bc_msg_t>()._has_bits_);
  static void set_has_committed_index(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

bc_msg_t::bc_msg_t()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:bc_msg_t)
}
bc_msg_t::bc_msg_t(const bc_msg_t& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_),
      blocks_(from.blocks_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  committed_index_ = from.committed_index_;
  // @@protoc_insertion_point(copy_constructor:bc_msg_t)
}

void bc_msg_t::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_bc_msg_t_Msg_2eproto.base);
  committed_index_ = 0u;
}

bc_msg_t::~bc_msg_t() {
  // @@protoc_insertion_point(destructor:bc_msg_t)
  SharedDtor();
}

void bc_msg_t::SharedDtor() {
}

void bc_msg_t::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const bc_msg_t& bc_msg_t::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_bc_msg_t_Msg_2eproto.base);
  return *internal_default_instance();
}


void bc_msg_t::Clear() {
// @@protoc_insertion_point(message_clear_start:bc_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  blocks_.Clear();
  committed_index_ = 0u;
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* bc_msg_t::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // required uint32 committed_index = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_committed_index(&has_bits);
          committed_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // repeated .block_msg_t blocks = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_blocks(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<26>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* bc_msg_t::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:bc_msg_t)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // required uint32 committed_index = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_committed_index(), target);
  }

  // repeated .block_msg_t blocks = 3;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_blocks_size()); i < n; i++) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(3, this->_internal_blocks(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:bc_msg_t)
  return target;
}

size_t bc_msg_t::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:bc_msg_t)
  size_t total_size = 0;

  // required uint32 committed_index = 1;
  if (_internal_has_committed_index()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_committed_index());
  }
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .block_msg_t blocks = 3;
  total_size += 1UL * this->_internal_blocks_size();
  for (const auto& msg : this->blocks_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void bc_msg_t::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:bc_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  const bc_msg_t* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<bc_msg_t>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:bc_msg_t)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:bc_msg_t)
    MergeFrom(*source);
  }
}

void bc_msg_t::MergeFrom(const bc_msg_t& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:bc_msg_t)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  blocks_.MergeFrom(from.blocks_);
  if (from._internal_has_committed_index()) {
    _internal_set_committed_index(from._internal_committed_index());
  }
}

void bc_msg_t::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:bc_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void bc_msg_t::CopyFrom(const bc_msg_t& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:bc_msg_t)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool bc_msg_t::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  if (!::PROTOBUF_NAMESPACE_ID::internal::AllAreInitialized(blocks_)) return false;
  return true;
}

void bc_msg_t::InternalSwap(bc_msg_t* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  blocks_.InternalSwap(&other->blocks_);
  swap(committed_index_, other->committed_index_);
}

::PROTOBUF_NAMESPACE_ID::Metadata bc_msg_t::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::txn_msg_t* Arena::CreateMaybeMessage< ::txn_msg_t >(Arena* arena) {
  return Arena::CreateInternal< ::txn_msg_t >(arena);
}
template<> PROTOBUF_NOINLINE ::block_msg_t* Arena::CreateMaybeMessage< ::block_msg_t >(Arena* arena) {
  return Arena::CreateInternal< ::block_msg_t >(arena);
}
template<> PROTOBUF_NOINLINE ::bc_msg_t* Arena::CreateMaybeMessage< ::bc_msg_t >(Arena* arena) {
  return Arena::CreateInternal< ::bc_msg_t >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
