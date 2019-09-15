#ifndef ALP_H
#define ALP_H


#include <CircularBuffer.h>
#include "dash7_modem.h"
#include "d7ap.h"
#include "d7ap_fs.h"
#include "lorawan_stack.h"

#define SERIAL_ALP_FRAME_SYNC_BYTE 0xC0
#define SERIAL_ALP_FRAME_VERSION   0x00
#define SERIAL_ALP_FRAME_HEADER_SIZE 3

#define ALP_PAYLOAD_MAX_SIZE D7A_PAYLOAD_MAX_SIZE + 1 // TODO configurable?

typedef enum
{
    DASH7,
    LORAWAN_OTAA,
    LORAWAN_ABP
} interface_type_t;

typedef struct {
    interface_type_t interface_type;
    union {
        d7ap_session_config_t d7ap_session_config;
        lorawan_session_config_otaa_t lorawan_session_config_otaa;
        lorawan_session_config_abp_t lorawan_session_config_abp;
    };
} session_config_t;

typedef enum alp_itf_id
{
    ALP_ITF_ID_HOST = 0x00,
    ALP_ITF_ID_SERIAL = 0x01, // not part of the spec
    ALP_ITF_ID_LORAWAN_ABP = 0x02, // not part of the spec
    ALP_ITF_ID_LORAWAN_OTAA = 0x03, // not part of the spec
    ALP_ITF_ID_D7ASP = 0xD7
} alp_itf_id_t;

typedef enum {
    ALP_OP_NOP = 0,
    ALP_OP_READ_FILE_DATA = 1,
    ALP_OP_READ_FILE_PROPERTIES = 2,
    ALP_OP_WRITE_FILE_DATA = 4,
    ALP_OP_WRITE_FILE_DATA_FLUSH = 5,
    ALP_OP_WRITE_FILE_PROPERTIES = 6,
    ALP_OP_ACTION_QUERY = 8,
    ALP_OP_BREAK_QUERY = 9,
    ALP_OP_PERMISSION_REQUEST = 10,
    ALP_OP_VERIFY_CHECKSUM = 11,
    ALP_OP_EXIST_FILE = 16,
    ALP_OP_CREATE_FILE = 17,
    ALP_OP_DELETE_FILE = 18,
    ALP_OP_RESTORE_FILE = 19,
    ALP_OP_FLUSH_FILE = 20,
    ALP_OP_OPEN_FILE = 21,
    ALP_OP_CLOSE_FILE = 22,
    ALP_OP_COPY_FILE = 23,
    ALP_OP_EXECUTE_FILE = 31,
    ALP_OP_RETURN_FILE_DATA = 32,
    ALP_OP_RETURN_FILE_PROPERTIES = 33,
    ALP_OP_RETURN_STATUS = 34,
    ALP_OP_RETURN_TAG = 35,
    ALP_OP_CHUNK = 48,
    ALP_OP_LOGIC = 49,
    ALP_OP_FORWARD = 50,
    ALP_OP_INDIRECT_FORWARD = 51,
    ALP_OP_REQUEST_TAG = 52
} alp_operation_t;

// define the (max) size for all ALP operation types
#define ALP_OP_SIZE_REQUEST_TAG (1 + 1)
#define ALP_OP_SIZE_READ_FILE_DATA (1 + 5 + 4)

typedef enum {
  ALP_STATUS_OK = 0x00,
  ALP_STATUS_PARTIALLY_COMPLETED = 0x01,
  ALP_STATUS_UNKNOWN_ERROR = 0x80,
  ALP_STATUS_UNKNOWN_OPERATION = 0xF6,
  ALP_STATUS_INSUFFICIENT_PERMISSIONS = 0xFC,
  // TODO others
  ALP_STATUS_INCOMPLETE_OPERAND = 0xF5,
  ALP_STATUS_FILE_ID_ALREADY_EXISTS = 0xFE,
  ALP_STATUS_FILE_ID_NOT_EXISTS = 0xFF,
} alp_status_codes_t;

typedef enum {
  ARITH_COMP_TYPE_INEQUALITY = 0,
  ARITH_COMP_TYPE_EQUALITY = 1,
  ARITH_COMP_TYPE_LESS_THAN = 2,
  ARITH_COMP_TYPE_LESS_THAN_OR_EQUAL_TO = 3,
  ARITH_COMP_TYPE_GREATER_THAN = 4,
  ARITH_COMP_TYPE_GREATER_THAN_OR_EQUAL_TO = 5
} alp_query_arithmetic_comparison_type_t;

/*! \brief The ALP CTRL header
 *
 * note: bit order is important here since this is send over the air. We explicitly reverse the order to ensure BE.
 * Although bit fields can cause portability problems it seems fine for now using gcc and the current platforms.
 * If this poses problems in the future we must resort to bit arithmetics and flags.
 */
typedef struct {
    union {
        uint8_t raw;
        struct {
            alp_operation_t operation : 6;
            bool b6 : 1;
            bool b7 : 1;
        };
    };
} alp_control_t;

/*! \brief The ALP CTRL header, for 'regular' operations, where b6 and b7 are overloaded with response_requested and group flags respectively
 */
typedef struct {
    union {
        uint8_t raw;
        struct {
            alp_operation_t operation : 6;
            bool response_requested : 1;
            bool group : 1;
        };
    };
} alp_control_regular_t;

/*! \brief The ALP CTRL header, for tag request operation
 */
typedef struct {
    union {
        uint8_t raw;
        struct {
            alp_operation_t operation : 6;
            bool _rfu : 1;
            bool respond_when_completed : 1;
        };
    };
} alp_control_tag_request_t;

/*! \brief The ALP CTRL header, for tag response operation
 */
typedef struct {
    union {
        uint8_t raw;
        struct {
            alp_operation_t operation : 6;
            bool error : 1;
            bool _rfu : 1;
        };
    };
} alp_control_tag_response_t;

typedef struct {
    uint8_t file_id;
    uint32_t offset;
} alp_operand_file_offset_t;

typedef struct {
    alp_operand_file_offset_t file_offset;
    uint32_t requested_data_length;
} alp_operand_file_data_request_t;

typedef struct {
    alp_operand_file_offset_t file_offset;
    uint32_t provided_data_length;
    uint8_t data[255]; // TODO fixed size?
} alp_operand_file_data_t;

typedef struct {
    uint8_t file_id;
    d7ap_fs_file_header_t file_header;
} alp_operand_file_header_t;

typedef struct {
    uint8_t data[255];
    uint8_t len;
    alp_itf_id_t type;
} alp_interface_status_t;


typedef struct {
    alp_operation_t operation;
    union {
        alp_operand_file_data_t file_data_operand;
        struct {
            bool completed;
            bool error;
            uint8_t tag_id;
        } tag_response;
        alp_interface_status_t status;
    };

} alp_action_t;

alp_operand_file_offset_t alp_parse_file_offset_operand(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer);
uint32_t alp_parse_length_operand(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer);
void alp_parse_action(CircularBuffer<uint8_t,BUFFER_SIZE>* buffer, alp_action_t* action);



#endif
