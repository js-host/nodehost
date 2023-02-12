#pragma once

#ifndef NODEHOST_API_NODECONTEXT_H
#define NODEHOST_API_NODECONTEXT_H

#include "./node_value.h"

#include "node.h"

#include <memory>
#include <string>

namespace nodehost {
    namespace api {

        class NodeInstance;
        class NodeScript;
        class NodeValue;

        class NodeContext {

        public:
            NodeContext(NodeInstance* instance);
            ~NodeContext();

        private:
            class impl;
            std::unique_ptr<impl> m_impl {};

        public:
            inline bool disposed() const {
                return m_impl == nullptr;
            }

            std::shared_ptr<NodeScript> compile(const std::string& script_utf8);

            void execute(const std::string& script_utf8);
            void execute(const NodeScript& script);
            std::shared_ptr<NodeValue> evaluate(const std::string& script_utf8);
            std::shared_ptr<NodeValue> evaluate(const NodeScript& script);
            void run();

            void dispose();

            // node api
            node::Environment* node_environment() const;

            // v8 api
            v8::Local<v8::Context> v8_context() const;

        };

    }
}

#endif
