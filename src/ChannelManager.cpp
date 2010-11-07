#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "Config.hpp"
#include "ChannelManager.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
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
    return i->second->isActive(cur);
}

bool ChannelManager::isActive(const string& name) const {
    return isActive(name, microsec_clock::universal_time());
}

void ChannelManager::startCleanup() {
    PION_LOG_INFO(m_logger, "Start cleanup cycle with period " << m_period);
    m_timer.expires_from_now(m_period);
    m_timer.async_wait(bind(&ChannelManager::purgeExpired, this, 
        placeholders::error));
}

void ChannelManager::stopCleanup() {
    m_timer.cancel();
}

void ChannelManager::purgeExpired(const boost::system::error_code& error) {
    if(!error) {
        PION_LOG_DEBUG(m_logger, "Purge expired");
        ptime cur(microsec_clock::universal_time());
        ChannelMap::iterator i = m_channels.begin();
        for(; i != m_channels.end(); ++i) {
            ChannelPtr channel = i->second;
            if(channel->isActive(cur)) {
                m_channels.erase(i);
            } else {
                channel->notify();
            }
        }
        m_timer.expires_from_now(m_period);
        m_timer.async_wait(bind(&ChannelManager::purgeExpired, this, 
            placeholders::error));
    } else {
        PION_LOG_ERROR(m_logger, "Timer error " << error);
    }
}

}   // end namespace lite_comet 

