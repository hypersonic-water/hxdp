#ifndef MESSAGE_H
#define MESSAGE_H

#include "dump.h"

void display_error(const char* msg);
void display_status(const char* msg, Cache* cache);
void display_config_messages(Args* args);

#endif //MESSAGE_H
