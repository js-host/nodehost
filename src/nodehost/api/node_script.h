#pragma once

#ifndef NODEHOST_API_NODESCRIPT_H
#define NODEHOST_API_NODESCRIPT_H

#include "../config.h"

#include <memory>
#include <string>

#include "../src/node.h"

namespace nodehost {
    namespace api {

        class NodeInstance;
        class NodeContext;

        class NODEHOST_API NodeScript {

        public:
            NodeScript(NodeInstance* instance, NodeContext* context, const std::string& script_utf8);
            ~NodeScript();

        private:
            class impl;
            std::unique_ptr<impl> m_impl {};

        public:
            inline bool disposed() const {
                return m_impl == nullptr;
            }

            void dispose();

            // v8
            v8::Local<v8::Script> v8_script() const;

        };

    }
}

#endif
