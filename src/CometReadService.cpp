#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "CometReadService.hpp"

using namespace boost;
using namespace pion;
using namespace pion::net;

namespace pion {        // begin namespace pion
namespace plugins {     // begin namespace plugins

// CometReadService member functions

void CometReadService::notifyAll(const std::string data) {
    WriterList::iterator i = m_writers.begin();
    for(; i != m_writers.end(); ++i) {
        (*i)->write(data);
        (*i)->send(bind(&TCPConnection::finish, (*i)->getTCPConnection()));
    }
    m_writers.clear();
}

/// handles requests for EchoService
void CometReadService::operator()(
    HTTPRequestPtr& request, 
    TCPConnectionPtr& tcp_conn
) {
    HTTPResponseWriterPtr writer(
        HTTPResponseWriter::create(
            tcp_conn, 
            *request
        )
    );
    // We don't want Keep-Alive here
    tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);
    // Set the content type as text
    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    // Do not send content length, because it is unknown now
    writer->getResponse().setDoNotSendContentLength();
    // Send the header
    writer->send();
    
    m_writers.insert(writer);
}


}   // end namespace plugins
}   // end namespace pion

