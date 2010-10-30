#ifndef __LITE_COMET_CHANNEL__HEADER__
#define __LITE_COMET_CHANNEL__HEADER__

#include <list>

#include <boost/signals2.hpp>
#include <boost/function.hpp>

namespace lite_comet {

class Message {
private:
    long m_offset;
    std::string m_data;
public:
    Message(long offset, const std::string &data) 
        : m_offset(offset), m_data(data)
    {}

    Message(const Message& other) {
        setOffset(other.getOffset());
        setData(other.getData());
    }

    /**
        @brief get offset of this message
        @return offset
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
        @brief get data of this message
        @return data
    **/
    inline const std::string& getData() const {
        return m_data;
    }

    /**
        @brief set data of this message
        @param data: data to set
    **/
    inline void setData(const std::string& data) {
        m_data = data;
    }
  
};

/**
    Channel for registering event data
**/
class Channel:
    public boost::noncopyable
{
public:
    ///
    static const long NO_OFFSET = 0;
    ///
    static const long WAITING_DATA = -1;
    ///
    static const long NEEDS_RESYNC = -3;

    /// Container of messages
    typedef std::list<Message> MessageList;
    /// Signal for notifying listeners
    typedef boost::signals2::signal<void ()> Signal;
    /// Listener function type
    typedef boost::function<void ()> Listener;
private:
    /// Messages of this channel
    MessageList m_messages;
    /// Event for data added
    Signal m_data_added;
public:
    Channel() {}
    ~Channel() {}

    /**
        @brief Add data to channel
        @param data to add
    **/
    void addData(const std::string&) ;

    /**
        @brief Add data to channel
        @param data to add
    **/
    const std::string getData(long offset) ;

    /**
        @brief Add data to channel
        @param data to add
    **/
    long getCurrentOffset() const ;

    /**
        @brief Determine is this channel active
    **/
    bool isActive(/*current date*/) const ;

    /**
        @brief Mark this channel as active
    **/
    void markActive() ;

    /**
        @brief Add listener to data added event
    **/
    void addListener(Listener) ;

private:
    /**
        @brief Notify all listeners attached to this channel
    **/
    void notify() ;

};

}   // end namespace lite_comet

#endif

