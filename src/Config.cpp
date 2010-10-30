#include "Config.hpp"

namespace lite_comet {

Config& Config::instance() {
    static Config config;
    return config;
}

}   // end namespace lite_comet 

