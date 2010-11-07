#ifndef __LITE_COMET_CHANNEL__HEADER__
#define __LITE_COMET_CHANNEL__HEADER__

#include <list>

#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "Message.hpp"

namespace lite_comet {

/**
    Channel for registering event data
**/
class Channel:
    public boost::noncopyable,
    public boost::enable_shared_from_this<Channel>
{
public:
    /// Container of messages
    typedef std::list<Message> MessageList;
    /// Listener function type
    typedef boost::function<void ()> Listener;
    /// Container for listener
    typedef std::list<Listener> ListenerList;
    /// Listener ID
    typedef ListenerList::iterator ListenerID;
    /// Type for returning from getData
    typedef boost::tuple<long, 
        MessageList::const_iterator, 
        MessageList::const_iterator
    > ChannelData;

private:
    /// Messages of this channel
    MessageList m_messages;
    /// List of listener to notify
    ListenerList m_listeners;
    /// Last active time
    boost::posix_time::ptime m_last_active;
protected:
    // stop client from create Channel directly, use create() instead
    Channel() 
        : m_last_active(boost::posix_time::microsec_clock::universal_time())
    {}
public:
    static inline boost::shared_ptr<Channel> create() {
        return boost::shared_ptr<Channel>(new Channel());
    } 

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
    const ChannelData getData(long offset) ;

    /**
        @brief Add data to channel
        @param data to add
    **/
    long getCurrentOffset() const ;

    /**
        @brief Determine is this channel active
    **/
    inline boost::posix_time::ptime getLastActive() const {
        return m_last_active;
    }

    /**
        @brief Check is this channel active
        @return Active or not
    **/
    bool isActive(boost::posix_time::ptime) const ;

    /**
        @brief Mark this channel as active
    **/
    void markActive() ;

    /**
        @brief Add listener to data added event
        @return ID to the listener
    **/
    ListenerID addListener(Listener) ;

    /**
        @brief Remove a listener
        @param ID of listener to remove
    **/
    void removeListener(ListenerID) ;

    /**
        @brief Notify all listeners attached to this channel
    **/
    void notify() ;

};

/// Shared pointer of channel
typedef boost::shared_ptr<Channel> ChannelPtr;

}   // end namespace lite_comet

#endif

