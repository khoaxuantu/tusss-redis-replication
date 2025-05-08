#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "stdio.h"

const size_t K_MAX_MSG = 4096;
const size_t MSG_HEADER_SIZE = 4; // bytes
const size_t MSG_BODY_START_INDEX = 4;
const size_t MSG_SIZE = MSG_HEADER_SIZE + K_MAX_MSG;

#endif
