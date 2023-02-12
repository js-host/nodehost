#pragma once

#ifndef NODEHOST_API_NODEERROR_H
#define NODEHOST_API_NODEERROR_H

#include "../config.h"

#include <stdexcept>

namespace nodehost {
    namespace api {

        class NODEHOST_API node_error : public std::runtime_error {

        public:
            node_error(const std::string& what = "")
                : std::runtime_error(what)
            {}

        };

    }
}

#endif
