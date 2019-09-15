#ifndef LORAWAN_STACK_H
#define LORAWAN_STACK_H

#include "arduino.h"

typedef struct
{
  /*point to the LoRa App data buffer*/
  uint8_t* Buff;
  /*LoRa App data buffer size*/
  uint8_t BuffSize;
  /*Port on which the LoRa App is data is sent/ received*/
  uint8_t Port;

} lorawan_AppData_t;

typedef enum {
  LORAWAN_STACK_ERROR_OK,
  LORAWAN_STACK_ERROR_NOT_JOINED,
  LORAWAN_STACK_ERROR_TX_NOT_POSSIBLE,
  LORAWAN_STACK_ERROR_UNKNOWN,
  LORAWAN_STACK_ERROR_NACK,
  LORAWAN_STACK_JOIN_FAILED,
  LORAWAN_STACK_DUTY_CYCLE_DELAY,
  LORAWAN_STACK_RETRY_TRANSMISSION,
  LORAWAN_STACK_JOINED
} lorawan_stack_status_t;

typedef enum {
  ABP,
  OTAA,
} activationMethod_t;

typedef struct __attribute__((__packed__)) {
    uint8_t attempts;
    lorawan_stack_status_t error_state;
    uint16_t duty_cycle_wait_time;
} lorawan_session_result_t;


typedef struct {
    uint8_t nwkSKey[16] ;
    uint8_t appSKey[16] ;
    uint32_t devAddr;
    uint32_t network_id;
    bool request_ack;
    uint8_t application_port;
    bool adr_enabled;
    uint8_t data_rate;
} lorawan_session_config_abp_t;

typedef struct {
    uint8_t devEUI[8];
    uint8_t appEUI[8];
    uint8_t appKey[16];
    bool request_ack;
    uint8_t application_port;
    bool adr_enabled;
    uint8_t data_rate;
} lorawan_session_config_otaa_t;

#endif
