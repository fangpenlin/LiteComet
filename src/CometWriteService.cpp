#include <vector>

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <pion/net/HTTPTypes.hpp>
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
    const string channel_names(
        HTTPTypes::url_decode(request->getQuery("channels")));
    const string type(HTTPTypes::url_decode(request->getQuery("type")));

    typedef vector<string> ChannelList;
    ChannelList channels;
    split(channels, channel_names, is_any_of(","));

    if(type == "reset") {
        ChannelList::const_iterator i = channels.begin();
        for(; i != channels.end(); ++i) {
            m_channel_manager.reset(*i);
            m_channel_manager.getChannel(*i);
        }
        PION_LOG_DEBUG(m_logger, "Reset channels " << channel_names);
    } else if (type == "close") {
        ChannelList::const_iterator i = channels.begin();
        for(; i != channels.end(); ++i) {
            m_channel_manager.reset(*i);
        }
        PION_LOG_DEBUG(m_logger, "Close channel " << channel_names);
    } else if (type == "is_active") {
        stringstream s;
        s << "{";
        ChannelList::const_iterator i = channels.begin();
        for(; i != channels.end(); ++i) {
            if(m_channel_manager.isActive(*i)) {
                s << "\"" << *i << "\": 1";
            }
        }
        // TODO handle comma here
        s << "}";
        Response::ok(writer, s.str());
        writer->send();
        return;
    } else {
        const string data(HTTPTypes::url_decode(request->getQuery("data")));
        if(data.empty()) {
            // TODO: write bad request error herex
            writer->write("Bad request");
            writer->send();
            return;
        }
        ChannelList::const_iterator i = channels.begin();
        for(; i != channels.end(); ++i) {
            ChannelPtr channel = m_channel_manager.getChannel(*i);
            PION_LOG_DEBUG(m_logger, "Add " << data.size() << 
                " bytes data to channel \"" << *i << "\"");
            channel->addData(data);
        }
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

