#pragma once

#ifndef NODEHOST_LIBNODEHOST_H
#define NODEHOST_LIBNODEHOST_H

#include "./config.h"

#include <stdint.h>
#include <math.h>

namespace nodehost {
    namespace api {
        class NodeInstance;
        class NodeContext;
        class NodeScript;
        class NodeValue;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

    NODEHOST_API int32_t nodehost_getlasterror(char* buffer_utf8, int32_t size);

    // process
    NODEHOST_API int32_t nodehost_process__startup(int32_t argc, char** argv);
    NODEHOST_API int32_t nodehost_process__shutdown();

    // instance
    NODEHOST_API int32_t nodehost_instance__new(nodehost::api::NodeInstance** out_instance);
    NODEHOST_API int32_t nodehost_instance__main_context(nodehost::api::NodeInstance* instance, nodehost::api::NodeContext** out_context);
    NODEHOST_API int32_t nodehost_instance__run(nodehost::api::NodeInstance* instance);
    NODEHOST_API int32_t nodehost_instance__dispose(nodehost::api::NodeInstance* instance);

    // context
    //NODEHOST_API int32_t nodehost_context__new(nodehost::api::NodeInstance* instance, nodehost::api::NodeContext** out_context);
    NODEHOST_API int32_t nodehost_context__execute(nodehost::api::NodeContext* context, nodehost::api::NodeScript* script);
    NODEHOST_API int32_t nodehost_context__execute_string(nodehost::api::NodeContext* context, const char* script_utf8);
    //NODEHOST_API int32_t nodehost_context__dispose(nodehost::api::NodeContext* context);

    // script
    NODEHOST_API int32_t nodehost_script__new(nodehost::api::NodeContext* context, const char* script_utf8, nodehost::api::NodeScript** out_script);
    NODEHOST_API int32_t nodehost_script__dispose(nodehost::api::NodeScript* script);

    // value
    NODEHOST_API int32_t nodehost_value__type(nodehost::api::NodeValue* value, int32_t* out_value /* nodehost::api::NodeValueType */);
    NODEHOST_API int32_t nodehost_value__get_boolean_value(nodehost::api::NodeValue* value, int8_t* out_value);
    NODEHOST_API int32_t nodehost_value__get_number_value(nodehost::api::NodeValue* value, double_t* out_value);

#ifdef __cplusplus
}
#endif

#endif
