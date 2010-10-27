#ifndef __LITE_COMET_COMETREADSERVICE_HEADER__
#define __LITE_COMET_COMETREADSERVICE_HEADER__

#include <set>

#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

namespace pion {        // begin namespace pion
namespace plugins {     // begin namespace plugins

/**
    HTTP service for providing users to read from Comet
**/
class CometReadService :
    public pion::net::WebService
{
public:
    // Container type for writers 
    typedef std::set<pion::net::HTTPResponseWriterPtr> WriterList;
private:
    /// All waiting writer of connections
    WriterList m_writers;
public:
    CometReadService(void) {}
    virtual ~CometReadService() {}
    virtual void operator()(pion::net::HTTPRequestPtr& request,
                            pion::net::TCPConnectionPtr& tcp_conn);
    /**
        @brief Notify all waiting connections
    **/
    virtual void notifyAll(const std::string) ;
};

}   // end namespace plugins
}   // end namespace pion

#endif

