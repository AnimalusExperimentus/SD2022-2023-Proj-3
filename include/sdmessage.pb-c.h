/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: sdmessage.proto */

#ifndef PROTOBUF_C_sdmessage_2eproto__INCLUDED
#define PROTOBUF_C_sdmessage_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1003000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1003003 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _MessageT MessageT;
typedef struct _MessageT__Value MessageT__Value;


/* --- enums --- */

typedef enum _MessageT__Opcode {
  MESSAGE_T__OPCODE__OP_BAD = 0,
  MESSAGE_T__OPCODE__OP_SIZE = 10,
  MESSAGE_T__OPCODE__OP_HEIGHT = 20,
  MESSAGE_T__OPCODE__OP_DEL = 30,
  MESSAGE_T__OPCODE__OP_GET = 40,
  MESSAGE_T__OPCODE__OP_PUT = 50,
  MESSAGE_T__OPCODE__OP_GETKEYS = 60,
  MESSAGE_T__OPCODE__OP_GETVALUES = 70,
  MESSAGE_T__OPCODE__OP_VERIFY = 80,
  MESSAGE_T__OPCODE__OP_ERROR = 99
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(MESSAGE_T__OPCODE)
} MessageT__Opcode;
typedef enum _MessageT__CType {
  MESSAGE_T__C_TYPE__CT_BAD = 0,
  MESSAGE_T__C_TYPE__CT_KEY = 10,
  MESSAGE_T__C_TYPE__CT_VALUE = 20,
  MESSAGE_T__C_TYPE__CT_ENTRY = 30,
  MESSAGE_T__C_TYPE__CT_KEYS = 40,
  MESSAGE_T__C_TYPE__CT_VALUES = 50,
  MESSAGE_T__C_TYPE__CT_RESULT = 60,
  MESSAGE_T__C_TYPE__CT_NONE = 70
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(MESSAGE_T__C_TYPE)
} MessageT__CType;

/* --- messages --- */

struct  _MessageT__Value
{
  ProtobufCMessage base;
  ProtobufCBinaryData data;
};
#define MESSAGE_T__VALUE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&message_t__value__descriptor) \
    , {0,NULL} }


struct  _MessageT
{
  ProtobufCMessage base;
  MessageT__Opcode opcode;
  MessageT__CType c_type;
  char *key;
  int32_t size;
  ProtobufCBinaryData data;
  size_t n_keys;
  char **keys;
  size_t n_vals;
  MessageT__Value **vals;
  int32_t op_n;
};
#define MESSAGE_T__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&message_t__descriptor) \
    , MESSAGE_T__OPCODE__OP_BAD, MESSAGE_T__C_TYPE__CT_BAD, (char *)protobuf_c_empty_string, 0, {0,NULL}, 0,NULL, 0,NULL, 0 }


/* MessageT__Value methods */
void   message_t__value__init
                     (MessageT__Value         *message);
/* MessageT methods */
void   message_t__init
                     (MessageT         *message);
size_t message_t__get_packed_size
                     (const MessageT   *message);
size_t message_t__pack
                     (const MessageT   *message,
                      uint8_t             *out);
size_t message_t__pack_to_buffer
                     (const MessageT   *message,
                      ProtobufCBuffer     *buffer);
MessageT *
       message_t__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   message_t__free_unpacked
                     (MessageT *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*MessageT__Value_Closure)
                 (const MessageT__Value *message,
                  void *closure_data);
typedef void (*MessageT_Closure)
                 (const MessageT *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor message_t__descriptor;
extern const ProtobufCMessageDescriptor message_t__value__descriptor;
extern const ProtobufCEnumDescriptor    message_t__opcode__descriptor;
extern const ProtobufCEnumDescriptor    message_t__c_type__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_sdmessage_2eproto__INCLUDED */
