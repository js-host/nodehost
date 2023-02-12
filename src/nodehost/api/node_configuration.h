#pragma once

#ifndef NODEHOST_API_NODECONFIGURATION_H
#define NODEHOST_API_NODECONFIGURATION_H

#include <vector>
#include <string>

namespace nodehost {
    namespace api {

        struct NodeConfiguration {

            std::vector<std::string> args;

        };

    }
}

#endif
