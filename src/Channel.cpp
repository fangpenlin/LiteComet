#include <sstream>

#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "Channel.hpp"

using namespace std;

namespace lite_comet {

static string getResponse(long new_offset, const string& data = string("[]")) {
    stringstream result;
    result << "{" 
        << "new_offset:" << new_offset<< ","
        << "data:" << data
    << "}";
    return result.str();
}

void Channel::addData(const std::string& data) {
    long current_offset = getCurrentOffset();
    // Reset the current offset if it is too big
    if(current_offset > 65536) {
        current_offset = 0;
        m_messages.clear();
    } else if (current_offset == WAITING_DATA) {
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

const std::string Channel::getData(long offset) {
    markActive();

    // We don't have data
    if(m_messages.empty()) {
        return getResponse(WAITING_DATA);
    }

    bool include_preceding = false;
    // If we are waiting for data we return all data
    if(offset == WAITING_DATA) {
        include_preceding = true;
    }
    // Try to find the current offset
    // if it's not found the data needs to be resynced
    if(offset > 0) {
        bool current_offset_found = false;
        MessageList::const_iterator i = m_messages.begin();
        for(; i != m_messages.end(); ++i) {
            const Message& msg = *i;
            if(msg.getOffset() == offset) {
                current_offset_found = true;
            }
        }

        if(!current_offset_found) {
            return getResponse(NEEDS_RESYNC);
        }
    }

    long new_offset = WAITING_DATA;
    stringstream data;
    data << "[";
    MessageList::const_iterator i = m_messages.begin();
    for(; i != m_messages.end(); ++i) {
        const Message& msg = *i;
        new_offset = msg.getOffset();
        if(include_preceding) {
            data << msg.getData();
            if(boost::next(i) != m_messages.end()) {
                data << ",";
            }
        }
    }
    data << "]";
    return getResponse(new_offset, data.str());
}

long Channel::getCurrentOffset() const {
    if(m_messages.empty()) {
        return WAITING_DATA;
    }
    return m_messages.back().getOffset();
}

bool Channel::isActive(/*current date*/) const {
    return false;
}

void Channel::markActive() {

}

void Channel::addListener(Listener func) {
    m_data_added.connect(func);
}


void Channel::notify() {
    m_data_added();
}

}   // end namespace lite_comet 

