#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "Channel.hpp"

using namespace std;

namespace lite_comet {

void Channel::addData(const std::string&) {

}

const std::string Channel::getData() const {
    return "";
}

long Channel::getCurrentOffset() const {
    return 0;
}

bool Channel::isActive(/*current date*/) const {
    return false;
}

void Channel::markActive() {

}


}   // end namespace lite_comet 

