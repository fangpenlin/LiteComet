#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "CometWriteService.hpp"
#include "Response.hpp"

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
    PION_LOG_DEBUG(m_logger, "Write with query: " << request->getQueryString());

    HTTPResponseWriterPtr writer(
        HTTPResponseWriter::create(
            tcp_conn, 
            *request,
            bind(&TCPConnection::finish, tcp_conn)
        )
    );
    const string channel_name(request->getQuery("channel_name"));
    const string type(request->getQuery("type"));

    if(type == "reset") {
        ChannelManager::ChannelMap& map(m_channel_manager.getChannelMap());
        ChannelManager::ChannelMap::iterator i = map.begin();
        for(; i != map.end(); ++i) {
            m_channel_manager.reset(i->first);
            i->second->markActive();
        }
        PION_LOG_DEBUG(m_logger, "Reset all channels");
    } else if (type == "close") {
        m_channel_manager.reset(channel_name);
        PION_LOG_DEBUG(m_logger, "Close channel " << channel_name);
    } else if (type == "is_active") {
        // TODO test active and return here
    } else {
        const string data(request->getQuery("data"));
        if(data.empty()) {
            // TODO: write bad request error herex
            writer->write("Bad request");
            writer->send();
            return;
        }
        ChannelPtr channel = m_channel_manager.getChannel(channel_name);
        PION_LOG_DEBUG(m_logger, "Add " << data.size() << 
            " bytes data to channel \"" << channel_name << "\"");
        channel->addData(data);
    }
    
    // Close the connection once the request is done
    tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);
    // Set the content type as text
    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    // Send the response
    writer->writeNoCopy(Response::OK);
    writer->send();
}


}

