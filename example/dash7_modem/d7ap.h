#ifndef D7AP_H
#define D7AP_H

#include "arduino.h"

#define D7AP_MAX_CLIENT_COUNT 8

#define ID_TYPE_NBID_ID_LENGTH 1
#define ID_TYPE_NOID_ID_LENGTH 0
#define ID_TYPE_UID_ID_LENGTH   8
#define ID_TYPE_VID_LENGTH      2

#define ID_TYPE_IS_BROADCAST(id_type) (id_type == ID_TYPE_NBID || id_type == ID_TYPE_NOID)

#define D7A_PAYLOAD_MAX_SIZE 239 // TODO confirm this value when FEC and security are disabled


typedef enum {
    ID_TYPE_NBID = 0,
    ID_TYPE_NOID = 1,
    ID_TYPE_UID = 2,
    ID_TYPE_VID = 3
} d7ap_addressee_id_type_t;

typedef enum
{
    AES_NONE = 0, /* No security */
    AES_CTR = 0x01, /* data confidentiality */
    AES_CBC_MAC_128 = 0x02, /* data authenticity */
    AES_CBC_MAC_64 = 0x03, /* data authenticity */
    AES_CBC_MAC_32 = 0x04, /* data authenticity */
    AES_CCM_128 = 0x05, /* data confidentiality and authenticity*/
    AES_CCM_64 = 0x06, /* data confidentiality and authenticity*/
    AES_CCM_32 = 0x07, /* data confidentiality and authenticity*/
} nls_method_t;

typedef struct {
    union {
        uint8_t raw;
        struct {
            nls_method_t nls_method : 4;
            d7ap_addressee_id_type_t id_type : 2;
            uint8_t _rfu : 2;
        };
    };
} d7ap_addressee_ctrl_t;

typedef struct __attribute__((__packed__)) {
    d7ap_addressee_ctrl_t ctrl;
    union {
        uint8_t access_class;
        struct {
            uint8_t access_mask : 4;
            uint8_t access_specifier : 4;
        };
    };
    uint8_t id[8]; // TODO assuming 8 byte id for now
} d7ap_addressee_t;

typedef struct {
    union {
        uint8_t raw;
        struct {
            uint8_t _rfu : 4;
            bool ucast : 1;
            bool retry : 1;
            bool missed : 1;
            bool nls : 1;
        };
    };
} d7ap_sp_state_t;

typedef struct
{
    uint8_t channel_header;         /**< PHY layer channel header */
    uint16_t center_freq_index;     /**< The center frequency index of the channel id */
} d7ap_channel_t;

typedef struct __attribute__((__packed__)) {
    d7ap_channel_t channel;
    uint8_t rx_level;
    uint8_t link_budget;
    uint8_t link_quality;
    uint8_t target_rx_level;
    d7ap_sp_state_t status;
    uint8_t fifo_token;
    uint8_t seqnr;
    uint8_t response_to;
    bool response_expected;
    d7ap_addressee_t addressee;
} d7ap_session_result_t;

typedef enum  {
    SESSION_RESP_MODE_NO = 0,
    SESSION_RESP_MODE_ALL = 1,
    SESSION_RESP_MODE_ANY = 2,
    SESSION_RESP_MODE_NO_RPT = 4,
    SESSION_RESP_MODE_ON_ERR = 5,
    SESSION_RESP_MODE_PREFERRED = 6,
} d7ap_session_resp_mode_t;

typedef enum {
    SESSION_RETRY_MODE_NO = 0
} d7ap_session_retry_mode_t;

typedef struct {
    union {
        uint8_t raw;
        struct {
            d7ap_session_resp_mode_t qos_resp_mode : 3;
            d7ap_session_retry_mode_t qos_retry_mode: 3;
            bool qos_record : 1;
            bool qos_stop_on_error : 1;
        };
    };
} d7ap_session_qos_t;

typedef struct __attribute__((__packed__)) {
    d7ap_session_qos_t qos;
    uint8_t dormant_timeout;
    d7ap_addressee_t addressee;
} d7ap_session_config_t;


static inline uint8_t d7ap_addressee_id_length(d7ap_addressee_id_type_t id_type) {
  switch(id_type)
  {
      case ID_TYPE_NOID:
        return ID_TYPE_NOID_ID_LENGTH;
      case ID_TYPE_NBID:
        return ID_TYPE_NBID_ID_LENGTH;
      case ID_TYPE_UID:
        return ID_TYPE_UID_ID_LENGTH;
      case ID_TYPE_VID:
        return ID_TYPE_VID_LENGTH;
      default:
        //assert(false);
        break;
  }

  return ID_TYPE_NOID_ID_LENGTH;
}

#endif
