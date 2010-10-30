#ifndef __LITE_COMET_CHANNEL__HEADER__
#define __LITE_COMET_CHANNEL__HEADER__

#include <list>

#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/signals2.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>

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
    /// Signal for notifying listeners
    typedef boost::signals2::signal<void ()> Signal;
    /// Listener function type
    typedef boost::function<void ()> Listener;
    /// Type for returning from getData
    typedef boost::tuple<long, 
        MessageList::const_iterator, 
        MessageList::const_iterator
    > ChannelData;
private:
    /// Messages of this channel
    MessageList m_messages;
    /// Event for data added
    Signal m_data_added;
protected:
    // stop client from create Channel directly, use create() instead
    Channel() {}
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

/// Shared pointer of channel
typedef boost::shared_ptr<Channel> ChannelPtr;

}   // end namespace lite_comet

#endif

