#ifndef __LITE_COMET_COMETWRITESERVICE_HEADER__
#define __LITE_COMET_COMETWRITESERVICE_HEADER__

#include <set>

#include <pion/PionLogger.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "ChannelManager.hpp"

namespace lite_comet {

/**
    HTTP service for providing web server to push data to waiting users
**/
class CometWriteService :
    public pion::net::WebService
{
private:
    /// Channel manager
    ChannelManager& m_channel_manager;
    /// Logger
    pion::PionLogger m_logger;
public:
    CometWriteService(ChannelManager& channel_manager) 
        : m_channel_manager(channel_manager),
          m_logger(PION_GET_LOGGER("lite_comet.CometWriteService"))
    {}
    virtual ~CometWriteService() {}
    virtual void operator()(pion::net::HTTPRequestPtr& request,
                            pion::net::TCPConnectionPtr& tcp_conn);
};

}

#endif

