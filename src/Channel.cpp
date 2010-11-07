#include <sstream>

#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "Config.hpp"
#include "Channel.hpp"

using namespace std;
using namespace boost;

namespace lite_comet {

void Channel::addData(const std::string& data) {
    long current_offset = getCurrentOffset();
    // Reset the current offset if it is too big
    if(current_offset > 65536) {
        current_offset = 0;
        m_messages.clear();
    } else if (current_offset == Config::instance().WAITING_DATA) {
        current_offset = 0;
    }
    ++current_offset;
    m_messages.push_back(Message(current_offset, data));
    if(m_messages.size() > 100) {
        m_messages.pop_front();
    }
    notify();
    markActive();
}

const Channel::ChannelData Channel::getData(long offset) {
    markActive();

    // We don't have data
    if(m_messages.empty()) {
        return make_tuple(Config::instance().WAITING_DATA, 
            m_messages.end(), m_messages.end());
    }

    bool include_preceding = false;
    // If client is waiting for data then we need to return all data
    if(offset == Config::instance().WAITING_DATA) {
        include_preceding = true;
    }
    // Try to find the current offset
    // if it's not found the data needs to be resynced
    MessageList::const_iterator begin_iter = m_messages.end();
    if(offset > 0) {
        bool current_offset_found = false;
        MessageList::const_iterator i = m_messages.begin();
        for(; i != m_messages.end(); ++i) {
            const Message& msg = *i;
            if(msg.getOffset() == offset) {
                current_offset_found = true;
                begin_iter = i;
                break;
            }
        }

        if(!current_offset_found) {
            return make_tuple(Config::instance().NEEDS_RESYNC, 
                m_messages.end(), m_messages.end());
        }
    }
    // Include preceding messages
    if(include_preceding) {
        begin_iter = m_messages.begin();
    }
    MessageList::const_iterator end_iter = m_messages.end();
    return make_tuple(getCurrentOffset(), begin_iter, end_iter);
}

long Channel::getCurrentOffset() const {
    if(m_messages.empty()) {
        return Config::instance().WAITING_DATA;
    }
    return m_messages.back().getOffset();
}

bool Channel::isActive(/*current date*/) const {
    return false;
}

void Channel::markActive() {

}

Channel::ListenerID Channel::addListener(Listener func) {
    return m_listeners.insert(m_listeners.end(), func);
}

void Channel::removeListener(ListenerID id) {
    m_listeners.erase(id);
}

void Channel::notify() {
    ListenerList::iterator i = m_listeners.begin();
    for(; i != m_listeners.end(); ++i) {
        (*i)();
    }
    m_listeners.clear();
}

}   // end namespace lite_comet 

