#ifndef __LITE_COMET_CHANNELDATA__HEADER__
#define __LITE_COMET_CHANNELDATA__HEADER__

#include <list>

#include "Message.hpp"

namespace lite_comet {

class ChannelData {
public:
    /// List container for message
    typedef std::list<Message> MessageList;
private:
    long m_offset;
    MessageList m_messages;
public:
    ChannelData(long offset) 
        : m_offset(offset)
    {}

    /**
        @brief get new offset
        @return new offset offset
    **/
    inline long getOffset() const {
        return m_offset;
    }

    /**
        @brief set offset of this message
        @param offset: offset to set
    **/
    inline void setOffset(long offset) {
        m_offset = offset;
    }

    /**
        @brief add message 
        @param message to add
    **/
    inline void addMessage(const Message& message) {
        m_messages.push_back(message);
    }
  
};
}   // end namespace lite_comet

#endif

