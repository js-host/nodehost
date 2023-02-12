#pragma once

#ifndef NODEHOST_API_NODEINSTANCE_H
#define NODEHOST_API_NODEINSTANCE_H

#include "node_context.h"

#include "v8.h"

#include <memory>

namespace nodehost {
    namespace api {

        class NodeInstance {

        public:
            NodeInstance();
            ~NodeInstance();

        private:
            class impl;
            std::unique_ptr<impl> m_impl {};
            std::unique_ptr<NodeContext> m_main_context {};

        public:
            NodeContext* main_context() const;

            inline bool disposed() const {
                return m_impl == nullptr;
            }

            void execute(const std::string& script_utf8);
            void run();
            void dispose();

            // v8 api
            v8::Isolate* v8_isolate() const;
            v8::Local<v8::Context> v8_main_context() const;

            // node api
            node::IsolateData* node_isolate_data() const;
            node::Environment* node_environment() const;

        };

    }
}

#endif
