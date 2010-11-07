#ifndef __LITE_COMET_COMETSTATUSSERVICE_HEADER__
#define __LITE_COMET_COMETSTATUSSERVICE_HEADER__

#include <boost/asio.hpp>
#include <pion/PionLogger.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "ChannelManager.hpp"

namespace lite_comet {

/**
    service for returning comet status
**/
class CometStatusService :
    public pion::net::WebService
{
private:
    /// Channel manager
    ChannelManager& m_channel_manager;
    /// Logger
    pion::PionLogger m_logger;
public:
    CometStatusService(ChannelManager &channel_manager) 
        : m_channel_manager(channel_manager), 
          m_logger(PION_GET_LOGGER("lite_comet.CometStatusService"))
    {}
    virtual ~CometStatusService() {}
    virtual void operator()(pion::net::HTTPRequestPtr& request,
                            pion::net::TCPConnectionPtr& tcp_conn);

};

}

#endif

