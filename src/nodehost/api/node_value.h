#pragma once

#ifndef NODEHOST_API_NODEVALUE_H
#define NODEHOST_API_NODEVALUE_H

#include "../config.h"

#include <memory>
#include <string>

#include "../src/node.h"

namespace nodehost {
    namespace api {

        class NodeInstance;
        class NodeContext;

        enum NodeValueType
        {
            NODEVALUETYPE_UNKNOWN = 0,

            NODEVALUETYPE_NULL = 1,
            NODEVALUETYPE_UNDEFINED = 2,
            NODEVALUETYPE_BOOLEAN = 3,
            NODEVALUETYPE_NUMBER = 4,
            NODEVALUETYPE_STRING = 5,

            NODEVALUETYPE_OBJECT = 10,
            NODEVALUETYPE_ARRAY = 11,
            NODEVALUETYPE_FUNCTION = 12,
        };

        class NODEHOST_API NodeValue {

        public:
            NodeValue(NodeInstance* instance, NodeContext* context, v8::Global<v8::Value> value);
            ~NodeValue();

        private:
            class impl;
            std::unique_ptr<impl> m_impl {};

        public:
            inline bool disposed() const {
                return m_impl == nullptr;
            }

            NodeValueType get_type() const;
            bool get_boolean_value() const;
            double_t get_number_value() const;

            void dispose();

            // v8
            v8::Local<v8::Value> v8_value() const;

        };

    }
}

#endif
