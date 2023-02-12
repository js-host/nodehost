#include "libnodehost.h"

#include "./internal/utils.h"
#include "./api/node_error.h"
#include "./api/node_process.h"
#include "./api/node_script.h"

#include <sstream>
#include <optional>
#include <chrono>

#include "node.h"

#define NODEHOST_SUCCESS 0
#define NODEHOST_ERROR 1

using namespace nodehost;
using namespace nodehost::internal;

std::optional<std::string> s_lasterror {};
void nodehost_setlasterror(const std::string& error) {
    s_lasterror = error;
}
void nodehost_setlasterror(const std::vector<std::string>& errors) {
    std::stringstream ss;
    for (const std::string& error : errors) {
        ss << error << std::endl;
    }
    nodehost_setlasterror(ss.str());
}

std::vector<std::shared_ptr<nodehost::api::NodeInstance>> s_instances {};
std::vector<std::shared_ptr<nodehost::api::NodeContext>> s_contexts {};
std::vector<std::shared_ptr<nodehost::api::NodeScript>> s_scripts {};

#ifdef __cplusplus
extern "C" {
#endif

    // utils
    void nodehost_setlasterror(const char* error) {
        if (error) {
            s_lasterror = std::string(error);
        }
        else {
            s_lasterror.reset();
        }
    }
    int nodehost_getlasterror(char* buffer_utf8, int32_t size) {
        auto hasError = s_lasterror.has_value();

        if (size <= 0 || !hasError) {
            return hasError ? s_lasterror.value().size() : 0;
        }

        const auto& error = s_lasterror.value();
        return error.copy(buffer_utf8, size);
    }

    // process
    int32_t nodehost_process__startup(int32_t argc, char** argv) {
        try {
            nodehost::api::NodeConfiguration configuration;
            configuration.args = std::vector<std::string>(argv, argv + argc);

            nodehost::api::NodeProcess::startup(configuration);

            return 0;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return 1;
        }
    }

    int32_t nodehost_process__shutdown() {
        try {
            nodehost::api::NodeProcess::shutdown();

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    // instance
    int32_t nodehost_instance__new(nodehost::api::NodeInstance** out_instance) {
        try {
            auto ptr = nodehost::api::NodeProcess::create_instance();

            s_instances.push_back(ptr);

            *out_instance = ptr.get();

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    int32_t nodehost_instance__main_context(nodehost::api::NodeInstance* instance, nodehost::api::NodeContext** out_context) {
        try {
            *out_context = instance->main_context();
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    int32_t nodehost_instance__run(nodehost::api::NodeInstance* instance) {
        try {
            instance->run();

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    int32_t nodehost_instance__dispose(nodehost::api::NodeInstance* instance) {
        try {
            instance->dispose();

            s_instances.erase(
                std::remove_if(s_instances.begin(), s_instances.end(), [instance](std::shared_ptr<nodehost::api::NodeInstance>& ptr) { return ptr.get() == instance; }),
                s_instances.end()
            );
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    // context
    // int32_t nodehost_context__new(nodehost::api::NodeInstance* instance, nodehost::api::NodeContext** out_context) {
    //     try {
    //         auto context = instance->create_context();
    //
    //         s_contexts.push_back(context);
    //
    //         *out_context = context.get();
    //         return NODEHOST_SUCCESS;
    //     }
    //     catch (const nodehost::api::node_error& err) {
    //         nodehost_setlasterror(err.what());
    //         return NODEHOST_ERROR;
    //     }
    // }

    int32_t nodehost_context__execute(nodehost::api::NodeContext* context, nodehost::api::NodeScript* script) {
        try {
            context->execute(*script);
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    int32_t nodehost_context__execute_string(nodehost::api::NodeContext* context, const char* script_utf8) {
        try {
            context->execute(script_utf8);
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    // int32_t nodehost_context__dispose(nodehost::api::NodeContext* context) {
    //     try {
    //         context->dispose();
    //
    //         s_contexts.erase(
    //             std::remove_if(s_contexts.begin(), s_contexts.end(), [context](std::shared_ptr<nodehost::api::NodeContext>& ptr) { return ptr.get() == context; }),
    //             s_contexts.end()
    //         );
    //         return NODEHOST_SUCCESS;
    //     }
    //     catch (const nodehost::api::node_error& err) {
    //         nodehost_setlasterror(err.what());
    //         return NODEHOST_ERROR;
    //     }
    // }

    // script
    int32_t nodehost_script__new(nodehost::api::NodeContext* context, const char* script_utf8, nodehost::api::NodeScript** out_script) {
        try {
            auto script = context->compile(script_utf8);

            s_scripts.push_back(script);

            *out_script = script.get();
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    int32_t nodehost_script__dispose(nodehost::api::NodeScript* script) {
        try {
            script->dispose();

            s_scripts.erase(
                std::remove_if(s_scripts.begin(), s_scripts.end(), [script](std::shared_ptr<nodehost::api::NodeScript>& ptr) { return ptr.get() == script; }),
                s_scripts.end()
            );
            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

    // value
    NODEHOST_API int32_t nodehost_value__type(nodehost::api::NodeValue* value, int32_t* out_value /* nodehost::api::NodeValueType */) {
        try {

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }
    NODEHOST_API int32_t nodehost_value__get_boolean_value(nodehost::api::NodeValue* value, int8_t* out_value) {
        try {

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }
    NODEHOST_API int32_t nodehost_value__get_number_value(nodehost::api::NodeValue* value, double_t* out_value) {
        try {

            return NODEHOST_SUCCESS;
        }
        catch (const nodehost::api::node_error& err) {
            nodehost_setlasterror(err.what());
            return NODEHOST_ERROR;
        }
    }

#ifdef __cplusplus
}
#endif
