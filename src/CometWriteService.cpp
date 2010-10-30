#include <boost/bind.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "CometWriteService.hpp"

using namespace std;
using namespace boost;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

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
        PION_LOG_DEBUG(m_logger, "Add data to channel test");
        ChannelPtr channel = m_channel_manager.getChannel("test");
        channel->addData(data);
    } else {
        // TODO: write bad request error herex
        writer->write("Bad request");
        writer->send();
    }
    
    // Close the connection once the request is done
    tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);
    // Set the content type as text
    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    // Send the response
    writer->writeNoCopy(OK_RESPONSE);
    writer->send();
}


}

