#ifndef __LITE_COMET_COMETREADSERVICE_HEADER__
#define __LITE_COMET_COMETREADSERVICE_HEADER__

#include <set>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <pion/PionLogger.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "ChannelManager.hpp"

namespace lite_comet {

/**
    service for providing users to read from Comet
**/
class CometReadService :
    public pion::net::WebService
{
public:
    typedef boost::asio::deadline_timer Timer;
    typedef boost::shared_ptr<Timer> TimerPtr;
    typedef std::set<TimerPtr> TimerSet;
private:
    /// Channel manager
    ChannelManager& m_channel_manager;
    /// Logger
    pion::PionLogger m_logger;
    /// Timer set
    TimerSet m_timers;
public:
    CometReadService(ChannelManager &channel_manager) 
        : m_channel_manager(channel_manager), 
          m_logger(PION_GET_LOGGER("lite_comet.CometReadService"))
    {}
    virtual ~CometReadService() {}
    virtual void operator()(pion::net::HTTPRequestPtr& request,
                            pion::net::TCPConnectionPtr& tcp_conn);
private:
    /**
        @brief Notify all waiting connections
    **/
    virtual void notifyChannel(
        pion::net::HTTPResponseWriterPtr, 
        ChannelPtr, 
        TimerPtr,
        const std::string&, 
        const std::string&, 
        long
    ) ;

    /**
        @brief Notify channel timeout
    **/
    virtual void notifyTimeout(
        pion::net::HTTPResponseWriterPtr, 
        ChannelPtr, 
        TimerPtr,
        Channel::ListenerID,
        const boost::system::error_code& , 
        const std::string&, 
        const std::string&, 
        long    
    ) ;


};

}

#endif

