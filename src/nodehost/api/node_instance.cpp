#include <string>
#include <thread>

#include "../src/node.h"
#include "../deps/uv/include/uv.h"

#include "node_instance.h"

#include "../internal/utils.h"
#include "node_process.h"
#include "node_error.h"

//#define USE_EMBEDDING_API
#define CHECK_DISPOSED PIMPL_CHECK_DISPOSED("node instance");

using namespace nodehost::api;
using namespace nodehost::internal;

class NodeInstance::impl {

public:
    impl() {
        int err;

#ifdef USE_EMBEDDING_API
        std::vector<std::string> errors {};
        std::vector<std::string> args { "dummy" };
        std::vector<std::string> exec_args {};

        // this call crashes for some unknown reason; running similar steps without it seems to work though..
        auto setup = node::CommonEnvironmentSetup::Create(NodeProcess::platform(), &errors, args, exec_args, node::EnvironmentFlags::kNoFlags);
#else
        m_loop.data = nullptr;
        err = uv_loop_init(&m_loop);
        if (err != 0) {
            throw std::runtime_error(std::string("Failed to initialize loop: ") + std::string(uv_err_name(err)));
        }

        m_allocator = node::ArrayBufferAllocator::Create();

        m_isolate = node::NewIsolate(m_allocator, &m_loop, NodeProcess::platform());

        // init
        {
            v8::Locker locker(m_isolate);
            v8::Isolate::Scope isolate_scope(m_isolate);

            m_isolate_data.reset(node::CreateIsolateData(m_isolate, &m_loop, NodeProcess::platform(), m_allocator.get()));

            v8::HandleScope handle_scope(m_isolate);

            const v8::Local<v8::Context> context = node::NewContext(m_isolate);
            m_context.Reset(m_isolate, context);
            if (m_context.IsEmpty()) {
                throw std::runtime_error("Failed to initialize V8 Context");
            }

            v8::Context::Scope context_scope(context);

            m_environment.reset(node::CreateEnvironment(node_isolate_data(), context, { "dummy" }, {}, node::EnvironmentFlags::kNoFlags, {}));

            node::LoadEnvironment(m_environment.get(),
                "const publicRequire = require('module').createRequire(process.cwd() + '/');"
                "globalThis.require = publicRequire;"
            );
        }
#endif
    }
    ~impl() {
        // stop
        if (m_isolate != nullptr) {
            {
                v8::Locker locker(m_isolate);
                v8::Isolate::Scope isolate_scope(m_isolate);

                m_context.Reset();
                m_environment.reset();
                m_isolate_data.reset();
            }

            auto platform = NodeProcess::platform();

            bool platform_finished = false;
            platform->AddIsolateFinishedCallback(m_isolate, [](void* data) {
                *static_cast<bool*>(data) = true;
            }, &platform_finished);
            platform->UnregisterIsolate(m_isolate);
            m_isolate->Dispose();

            // Wait until the platform has cleaned up all relevant resources.
            while (!platform_finished)
                uv_run(&m_loop, UV_RUN_ONCE);
        }

        if (m_isolate || m_loop.data != nullptr) {
            if (uv_loop_close(&m_loop) != 0) {
                // ¯\_(ツ)_/¯
                //throw node_error("Failed to close loop");
            }
        }
      
        m_isolate = nullptr;
    }
    impl(const impl&) = delete;
    impl(const impl&&) = delete;
    impl& operator=(const impl&) = delete;
    impl& operator=(const impl&&) = delete;

private:
    uv_loop_t m_loop;
    std::shared_ptr<node::ArrayBufferAllocator> m_allocator;
    v8::Isolate* m_isolate;
    std::unique_ptr<node::IsolateData, functor<&node::FreeIsolateData>> m_isolate_data;
    
    v8::Global<v8::Context> m_context {};
    std::unique_ptr<node::Environment, functor<&node::FreeEnvironment>> m_environment {};

public:
    void run() {
        const auto isolate = this->m_isolate;

        v8::Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        v8::Local<v8::Context> context = m_context.Get(isolate);
        v8::Context::Scope context_scope(context);

        int err = node::SpinEventLoop(m_environment.get()).FromMaybe(1);
        if (err != 0) {
            throw std::runtime_error(std::string("Failed to spin event loop, exit code: ") + std::to_string(err));
        }
    }

    // v8 api
    inline v8::Isolate* v8_isolate() const {
        return m_isolate;
    }

    inline v8::Local<v8::Context> v8_main_context() const {
        return m_context.Get(m_isolate);
    }
  
    // node api
    inline node::IsolateData* node_isolate_data() const {
        return m_isolate_data.get();
    }

    inline node::Environment* node_environment() const {
        return m_environment.get();
    }

};

NodeInstance::NodeInstance()
    : m_impl{ new impl() }
{
    m_main_context = std::make_unique<NodeContext>(this);
}
NodeInstance::~NodeInstance() {
    this->dispose();
}

NodeContext* NodeInstance::main_context() const {
    CHECK_DISPOSED;

    return m_main_context.get();
}

void NodeInstance::execute(const std::string& script_utf8) {
    CHECK_DISPOSED;

    m_main_context->execute(script_utf8);
}

void NodeInstance::run() {
    CHECK_DISPOSED;
  
    m_impl->run();
}

void NodeInstance::dispose() {
    if (disposed()) {
        return;
    }

    m_main_context.reset();
    m_impl.reset();
}

// v8 api
v8::Isolate* NodeInstance::v8_isolate() const {
    CHECK_DISPOSED;

    return m_impl->v8_isolate();
}

v8::Local<v8::Context> NodeInstance::v8_main_context() const {
  CHECK_DISPOSED;

  return m_impl->v8_main_context();
}

// node api
node::IsolateData* NodeInstance::node_isolate_data() const {
    CHECK_DISPOSED;

    return m_impl->node_isolate_data();
}

node::Environment* NodeInstance::node_environment() const {
    CHECK_DISPOSED;

    return m_impl->node_environment();
}