#include "./node_script.h"

#include "../internal/utils.h"
#include "./node_error.h"
#include "./node_instance.h"

#define CHECK_DISPOSED PIMPL_CHECK_DISPOSED("node script");

using namespace nodehost::api;
using namespace nodehost::internal;

class NodeScript::impl {

public:
    impl(NodeInstance* instance, NodeContext* context, const std::string& script_utf8) {
        m_instance = instance;
        m_context = context;

        auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, script_utf8.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
        m_script.Reset(isolate, v8::Script::Compile(context->v8_context(), source).ToLocalChecked());
    }
    ~impl() {
        if (!m_script.IsEmpty()) {
            auto isolate = this->m_instance->v8_isolate();

            v8::Locker locker(isolate);
            v8::Isolate::Scope isolate_scope(isolate);
            v8::HandleScope handle_scope(isolate);

            m_script.Reset();
        }
        if (m_context) {
            m_context = nullptr;
        }
        if (m_instance) {
            m_instance = nullptr;
        }
    }
    impl(const impl&) = delete;
    impl(const impl&&) = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(const impl&&) = delete;

private:
    NodeInstance* m_instance;
    NodeContext* m_context;
    v8::Global<v8::Script> m_script;

public:
    // v8 api
    v8::Local<v8::Script> v8_script() const {
        auto isolate = v8::Isolate::GetCurrent();
        if (m_instance->v8_isolate() != isolate) {
            throw node_error("Trying to retrieve v8 script in incorrect isolate.");
        }

        return m_script.Get(isolate);
    }

};

NodeScript::NodeScript(NodeInstance* instance, NodeContext* context, const std::string& script_utf8)
    : m_impl(new impl(instance, context, script_utf8))
{
}
NodeScript::~NodeScript() {
    this->dispose();
}

void NodeScript::dispose() {
    if (disposed()) {
        return;
    }

    m_impl.reset();
}

// v8 api
v8::Local<v8::Script> NodeScript::v8_script() const {
    CHECK_DISPOSED;

    return m_impl->v8_script();
}
