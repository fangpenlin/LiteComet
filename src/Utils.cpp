#include "Utils.hpp"

using namespace std;

namespace lite_comet {

ostream& operator << (ostream& out, const Json::Value& value) {
    Json::FastWriter writer;
    out << writer.write(value);
    return out;
}

}   // end namespace lite_comet 

