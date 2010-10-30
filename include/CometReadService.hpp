#ifndef __LITE_COMET_COMETREADSERVICE_HEADER__
#define __LITE_COMET_COMETREADSERVICE_HEADER__

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
private:
    /// Channel manager
    ChannelManager& m_channel_manager;
    /// Logger
    pion::PionLogger m_logger;
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
    virtual void notifyChannel(pion::net::HTTPResponseWriterPtr, ChannelPtr) ;
};

}

#endif

