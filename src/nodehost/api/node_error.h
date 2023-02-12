#pragma once

#ifndef NODEHOST_API_NODEERROR_H
#define NODEHOST_API_NODEERROR_H

#include <stdexcept>

namespace nodehost {
    namespace api {

        class node_error : public std::runtime_error {

        public:
            node_error(const std::string& what = "")
                : std::runtime_error(what)
            {}

        };

    }
}

#endif
