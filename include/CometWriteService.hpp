#ifndef __LITE_COMET_COMETWRITESERVICE_HEADER__
#define __LITE_COMET_COMETWRITESERVICE_HEADER__

#include <set>

#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "CometReadService.hpp"

namespace pion {        // begin namespace pion
namespace plugins {     // begin namespace plugins

/**
    HTTP service for providing web server to push data to waiting users
**/
class CometWriteService :
    public pion::net::WebService
{
private:
    CometReadService &m_readService;
public:
    CometWriteService(CometReadService& readService) 
        :   m_readService(readService)
    {}
    virtual ~CometWriteService() {}
    virtual void operator()(pion::net::HTTPRequestPtr& request,
                            pion::net::TCPConnectionPtr& tcp_conn);
};

}   // end namespace plugins
}   // end namespace pion

#endif

