#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//const uint8_t PROTO_SCOPE_LEVELS = 0x01;
//const uint8_t PROTO_CMD_SHOW = 0x81;

#define PROTO_SCOPE_TOP 0x00
#define PROTO_SCOPE_LEVELS 0x01

#define PROTO_CMD_SHOW 0x01
#define PROTO_CMD_STOP_SERVER 0xfe
#define PROTO_CMD_QUIT 0xff

#define CD_NET_CODE_OK 0x00
#define CD_NET_CODE_FAIL 0x01
