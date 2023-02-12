#pragma once

#ifndef NODEHOST_API_NODECONFIGURATION_H
#define NODEHOST_API_NODECONFIGURATION_H

#include "../config.h"

namespace nodehost {
    namespace api {

        struct NODEHOST_API NodeConfiguration {

            std::vector<std::string> args;

        };

    }
}

#endif
