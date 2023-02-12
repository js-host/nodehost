#include "./node_value.h"

#include "../internal/utils.h"
#include "./node_error.h"
#include "./node_instance.h"

#define CHECK_DISPOSED PIMPL_CHECK_DISPOSED("node value");

using namespace nodehost::api;
using namespace nodehost::internal;

class NodeValue::impl {

public:
    impl(NodeInstance* instance, NodeContext* context, v8::Global<v8::Value> value) {
        m_instance = instance;
        m_context = context;

        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        m_value.Reset(isolate, value);
    }
    ~impl() {
        if (!m_value.IsEmpty()) {
            const auto isolate = this->m_instance->v8_isolate();

            v8::Locker locker(isolate);
            v8::Isolate::Scope isolate_scope(isolate);
            v8::HandleScope handle_scope(isolate);

            m_value.Reset();
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
    v8::Global<v8::Value> m_value;

public:
    NodeValueType get_type() const {
        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        const v8::Local<v8::Value> value = this->v8_value();

        if (value->IsBoolean()) {
            return NODEVALUETYPE_BOOLEAN;
        }
        if (value->IsNumber()) {
            return NODEVALUETYPE_NUMBER;
        }

        return NODEVALUETYPE_UNKNOWN;
    }

    bool get_boolean_value() const {
        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        const v8::Local<v8::Value> value = this->v8_value();

        if (!value->IsBoolean()) {
            // TODO: specify which kind of value
            throw std::runtime_error(std::string("Attempted to retrieve non-boolean value as boolean"));
        }

        return value->ToBoolean(isolate)->Value();
    }

    double_t get_number_value() const {
        const auto isolate = this->m_instance->v8_isolate();

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        const v8::Local<v8::Value> value = this->v8_value();

        if (!value->IsNumber()) {
            // TODO: specify which kind of value
            throw std::runtime_error(std::string("Attempted to retrieve non-number value as number"));
        }

        return value.As<v8::Number>()->Value();
    }

    // v8 api
    v8::Local<v8::Value> v8_value() const {
        const auto isolate = v8::Isolate::GetCurrent();
        if (m_instance->v8_isolate() != isolate) {
            throw node_error("Trying to retrieve v8 value in incorrect isolate.");
        }

        return m_value.Get(isolate);
    }

};

NodeValue::NodeValue(NodeInstance* instance, NodeContext* context, v8::Global<v8::Value> value)
    : m_impl(new impl(instance, context, std::move(value)))
{
}
NodeValue::~NodeValue() {
    this->dispose();
}

NodeValueType NodeValue::get_type() const {
    CHECK_DISPOSED;

    return m_impl->get_type();
}

bool NodeValue::get_boolean_value() const {
    CHECK_DISPOSED;

    return m_impl->get_boolean_value();
}

double_t NodeValue::get_number_value() const {
    CHECK_DISPOSED;

    return m_impl->get_number_value();
}


void NodeValue::dispose() {
    if (disposed()) {
        return;
    }

    m_impl.reset();
}

// v8 api
v8::Local<v8::Value> NodeValue::v8_value() const {
    CHECK_DISPOSED;

    return m_impl->v8_value();
}

