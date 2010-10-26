// ------------------------------------------------------------------
// pion-net: a C++ framework for building lightweight HTTP interfaces
// ------------------------------------------------------------------
// Copyright (C) 2007-2008 Atomic Labs, Inc.  (http://www.atomiclabs.com)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#ifndef __PION_COMETWRITESERVICE_HEADER__
#define __PION_COMETWRITESERVICE_HEADER__

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

