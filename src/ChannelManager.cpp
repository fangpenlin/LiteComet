#include "ChannelManager.hpp"

using namespace std;

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

}   // end namespace lite_comet 

