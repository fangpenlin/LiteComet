#ifndef __LITE_COMET_COMETREADSERVICE_HEADER__
#define __LITE_COMET_COMETREADSERVICE_HEADER__

#include <set>

#include <pion/PionLogger.hpp>
#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

#include "ChannelManager.hpp"

namespace lite_comet {

/// Weak reference to writer for avoiding cycle reference
typedef boost::weak_ptr<pion::net::HTTPResponseWriter> HTTPResponseWriterWeakPtr;

/* 
    service for providing users to read from Comet

    Note:
        We use weak_ptr to pass to channel as argument of callback function,
        because somehow, if we pass the shared_ptr directly, there will
        be a reference cycle, then the TCPConnection will never be released.
        
        I am not sure how the cycle formed, but I guess it must be relative
        to those objects created by boost::bind function which holds 
        reference to the other.
**/
class CometReadService :
    public pion::net::WebService
{
public:
    typedef std::set<pion::net::HTTPResponseWriterPtr> WriterSet;
private:
    /// Channel manager
    ChannelManager& m_channel_manager;
    /// Logger
    pion::PionLogger m_logger;
    /// References to writers
    WriterSet m_writers;
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
    virtual void notifyChannel(HTTPResponseWriterWeakPtr, ChannelPtr) ;
};

}

#endif

