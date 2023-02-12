#include <mutex>
#include <sstream>

#include "../deps/uv/include/uv.h"
#include "../src/node.h"
#include "../src/node_api.h"

#include "node_error.h"
#include "node_process.h"
#include "../internal/utils.h"

using namespace nodehost::api;
using namespace nodehost::internal;

std::mutex s_mutex;
std::atomic<NodeProcessState> s_state{ NodeProcessState::idle };
NodeConfiguration s_configuration;
std::unique_ptr<node::MultiIsolatePlatform> s_platform{ nullptr };

std::vector<std::weak_ptr<NodeInstance>> s_instances;

void NodeProcess::startup(const NodeConfiguration& configuration) {
    std::lock_guard guard(s_mutex);

    if (s_state.load() != NodeProcessState::idle) {
        // TODO: maybe log warning?
        return;
    }

    s_configuration = configuration;

    // http://docs.libuv.org/en/v1.x/misc.html#c.uv_setup_args
    //argv = uv_setup_args(argc, arv);

    std::vector<std::string> args = s_configuration.args;
    auto result = node::InitializeOncePerProcess(args, { node::ProcessInitializationFlags::kNoInitializeV8, node::ProcessFlags::kNoInitializeNodeV8Platform });

    s_platform = node::MultiIsolatePlatform::Create(2);
    v8::V8::InitializePlatform(s_platform.get());
    v8::V8::Initialize();

    s_state.store(NodeProcessState::running);
}

const NodeConfiguration* NodeProcess::configuration() {
    return &s_configuration;
}

NodeProcessState NodeProcess::state() {
    return s_state.load();
}

node::MultiIsolatePlatform* NodeProcess::platform() {
    return s_platform.get();
}

std::shared_ptr<NodeInstance> NodeProcess::create_instance() {
    std::lock_guard guard(s_mutex);

    gc_weak_ptr_vector(s_instances);

    auto instance = std::make_shared<NodeInstance>();

    s_instances.push_back(std::weak_ptr<NodeInstance>{ instance });

    return instance;
}

void NodeProcess::shutdown() {
    std::lock_guard guard(s_mutex);

    if (s_state.load() != NodeProcessState::running) {
        // TODO: maybe log warning?
        return;
    }

    gc_weak_ptr_vector(s_instances);

    if (s_instances.size() > 0) {
        throw node_error(string_format("Cannot shutdown node when %d instances are still running.", s_instances.size()));
    }

    v8::V8::Dispose();
    v8::V8::DisposePlatform();

    s_platform.reset(nullptr);

    s_state.store(NodeProcessState::shutdown);
}
