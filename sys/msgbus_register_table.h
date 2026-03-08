#ifndef __MSGBUSS_REGISTER_TABLE_H__
#define __MSGBUSS_REGISTER_TABLE_H__
#include "msgbus.h"

typedef struct _msgbus_register_conf {
    msgbus_type_t type;
    const char *name;
}msgbus_register_conf;


static msgbus_register_conf mbr_conf_tbl[] = {
    {MSGBUS_TYPE_NORMAL,   "misc"},
    {MSGBUS_TYPE_NORMAL,   "normal0"},
    {MSGBUS_TYPE_NORMAL,   "normal1"},
    {MSGBUS_TYPE_NORMAL,   "normal2"},
    {MSGBUS_TYPE_NORMAL,   "normal3"},
    {MSGBUS_TYPE_PRIORITY, "priority0"},
    {MSGBUS_TYPE_PRIORITY, "priority1"},
    {MSGBUS_TYPE_PRIORITY, "priority2"},
    {MSGBUS_TYPE_PRIORITY, "priority3"},
    {MSGBUS_TYPE_URGENT,   "urgent0"},
    {MSGBUS_TYPE_URGENT,   "urgent1"},
    {MSGBUS_TYPE_URGENT,   "urgent2"},
    {MSGBUS_TYPE_URGENT,   "urgent3"},
    {MSGBUS_TYPE_URGENT,   "urgent4"},
};

#endif