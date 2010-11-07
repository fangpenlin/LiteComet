#ifndef __LITE_COMET_UTILS__HEADER__
#define __LITE_COMET_UTILS__HEADER__

#include <ostream>

#include <json/json.h>

namespace lite_comet {

std::ostream& operator << (std::ostream& out, const Json::Value&) ;

}   // end namespace lite_comet

#endif

