// ------------------------------------------------------------------
// pion-net: a C++ framework for building lightweight HTTP interfaces
// ------------------------------------------------------------------
// Copyright (C) 2007-2008 Atomic Labs, Inc.  (http://www.atomiclabs.com)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "CometWriteService.hpp"

using namespace std;
using namespace boost;
using namespace pion;
using namespace pion::net;

namespace pion {        // begin namespace pion
namespace plugins {     // begin namespace plugins

// CometWriteService member functions

/// handles requests for EchoService
void CometWriteService::operator()(
    HTTPRequestPtr& request, 
    TCPConnectionPtr& tcp_conn
) {
    static const string OK_RESPONSE("OK");

    HTTPResponseWriterPtr writer(
        HTTPResponseWriter::create(
            tcp_conn, 
            *request,
            bind(&TCPConnection::finish, tcp_conn)
        )
    );
    if(request->getContent()) {
        const std::string data = request->getContent();
        m_readService.notifyAll(data);
    } else {
        // TODO: write bad request error herex
        writer->write("Bad request");
        writer->send();
    }
    
    // Set the content type as text
    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    // Send the response
    writer->writeNoCopy(OK_RESPONSE);
    writer->send();
}


}   // end namespace plugins
}   // end namespace pion

