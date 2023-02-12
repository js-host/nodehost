#include "../src/node.h"
#include "../deps/uv/include/uv.h"

#include "node_context.h"

#include "../internal/utils.h"
#include "./node_error.h"
#include "./node_instance.h"
#include "./node_script.h"

#define CHECK_DISPOSED PIMPL_CHECK_DISPOSED("node context");

using namespace nodehost::api;
using namespace nodehost::internal;

class NodeContext::impl {

public:
    impl(NodeInstance* instance) {
        m_instance = instance;
    }
    ~impl() {
        if (m_instance) {
            m_instance = nullptr;
        }
    }
    impl(const impl&) = delete;
    impl(const impl&&) = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(const impl&&) = delete;

private:
    NodeInstance* m_instance {};

public:
    std::shared_ptr<NodeScript> compile(NodeContext* context, const std::string& script_utf8) {
        return std::make_shared<NodeScript>(m_instance, context, script_utf8);
    }

    void execute(NodeContext* node_context, const NodeScript& script) {
        auto result = this->evaluate(node_context, script);
    }

    void execute(NodeContext* node_context, const std::string& script_utf8) {
        auto result = this->evaluate(node_context, script_utf8);
    }

    std::shared_ptr<NodeValue> evaluate(NodeContext* node_context, const std::string& script_utf8) {
        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = m_instance->v8_main_context();
        v8::Context::Scope context_scope(context);

        v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, script_utf8.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
        v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        return std::make_shared<NodeValue>(this->m_instance, node_context, v8::Global<v8::Value>(isolate, result));
    }
    std::shared_ptr<NodeValue> evaluate(NodeContext* node_context, const NodeScript& script) {
        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::Context> context = m_instance->v8_main_context();
        v8::Context::Scope context_scope(context);

        v8::Local<v8::Value> result = script.v8_script()->Run(context).ToLocalChecked();
      
      return std::make_shared<NodeValue>(this->m_instance, node_context, v8::Global<v8::Value>(isolate, result));
    }

    // node api
    inline node::Environment* node_environment() const {
        return this->m_instance->node_environment();
    }

    // v8 api
    inline v8::Local<v8::Context> v8_context() const {
        return this->m_instance->v8_main_context();
    }

};

NodeContext::NodeContext(NodeInstance* instance)
    : m_impl(new impl(instance))
{
}
NodeContext::~NodeContext() {
    this->dispose();
}

std::shared_ptr<NodeScript> NodeContext::compile(const std::string& script_utf8) {
    CHECK_DISPOSED;

    return m_impl->compile(this, script_utf8);
}

void NodeContext::execute(const std::string& script_utf8) {
    CHECK_DISPOSED;

    m_impl->execute(this, script_utf8);
}

void NodeContext::execute(const NodeScript& script) {
    CHECK_DISPOSED;

    m_impl->execute(this, script);
}

std::shared_ptr<NodeValue> NodeContext::evaluate(const std::string& script_utf8) {
    CHECK_DISPOSED;

    return m_impl->evaluate(this, script_utf8);
}

std::shared_ptr<NodeValue> NodeContext::evaluate(const NodeScript& script) {
    CHECK_DISPOSED;

    return m_impl->evaluate(this, script);
}

void NodeContext::dispose() {
    if (disposed()) {
        return;
    }

    m_impl.reset();
}

// node api
node::Environment* NodeContext::node_environment() const {
    CHECK_DISPOSED;

    return this->m_impl->node_environment();
}

// v8 api
v8::Local<v8::Context> NodeContext::v8_context() const {
    CHECK_DISPOSED;

    return this->m_impl->v8_context();
}
