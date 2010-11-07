#include "Config.hpp"
#include "ChannelManager.hpp"

using namespace std;
using namespace boost::posix_time;

namespace lite_comet {

ChannelPtr& ChannelManager::getChannel(const string& name) {
    ChannelMap::iterator i = m_channels.find(name);
    // The channel doesn't exists, create one
    if(i == m_channels.end()) {
        ChannelPtr channel = Channel::create();
        i = m_channels.insert(ChannelMap::value_type(name, channel)).first;
    }
    return i->second;
}

ChannelManager::ChannelMap& ChannelManager::getChannelMap() {
    return m_channels;
}

void ChannelManager::reset(const string& name) {
    m_channels.erase(name);
}

bool ChannelManager::isActive(const string& name, ptime cur) const {
    ChannelMap::const_iterator i = m_channels.find(name);
    if(i == m_channels.end()) {
        return false;
    }
    ChannelPtr channel = i->second;
    time_duration elapsed = cur - channel->getLastActive();//negative result
    if(elapsed.total_milliseconds() <= Config::instance().CHANNEL_TIMEOUT*2) {
        return true;
    }
    return false;
}

bool ChannelManager::isActive(const string& name) const {
    return isActive(name, microsec_clock::universal_time());
}

}   // end namespace lite_comet 

