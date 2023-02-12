#pragma once

#ifndef NODEHOST_API_NODEPROCESS_H
#define NODEHOST_API_NODEPROCESS_H

#include "../config.h"

#include <vector>
#include <string>

#include "./node_configuration.h"
#include "./node_instance.h"

namespace nodehost {
    namespace api {

        enum class NodeProcessState {
            // Startup wasn't called yet.
            idle,
            // Node is running.
            running,
            // Node was shutdown.
            shutdown,
        };

        class NODEHOST_API NodeProcess {
            NodeProcess() = delete;
            ~NodeProcess() = delete;

        public:
            static void startup(const NodeConfiguration& configuration);
            static const NodeConfiguration* configuration();
            static node::MultiIsolatePlatform* platform();
            static NodeProcessState state();

            static std::shared_ptr<NodeInstance> create_instance();

            static void shutdown();

        };

    }
}

#endif
