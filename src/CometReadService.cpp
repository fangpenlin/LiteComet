#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "Config.hpp"
#include "CometReadService.hpp"

using namespace std;
using namespace boost;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

// CometReadService member functions

void CometReadService::notifyChannel(
    HTTPResponseWriterPtr weak_writer, 
    ChannelPtr channel
) {
    PION_LOG_DEBUG(m_logger, "Notify channel test");
    HTTPResponseWriterPtr writer(weak_writer);
    writer->write(channel->getData(0));
    writer->send(bind(&TCPConnection::finish, writer->getTCPConnection()));
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
    // Comet channel name
    const string channel_name(request->getQuery("channel_name"));
    // Offset of data
    long offset = Config::instance().NO_OFFSET;
    if(request->hasQuery("offset")) {
        offset = lexical_cast<long>(request->getQuery("offset"));
    }
    // Timeout of waiting time
    long timeout = Config::instance().CHANNEL_TIMEOUT;
    if(request->hasQuery("timeout")) {
        timeout = lexical_cast<long>(request->getQuery("offset"));
    }
    // set timeout if it is zero
    if(!timeout) {
        timeout = Config::instance().CHANNEL_TIMEOUT-5000;
    }
    // limit the maxmium timeout to twice of timeout in config
    if(timeout > Config::instance().CHANNEL_TIMEOUT*2) {
        timeout = Config::instance().CHANNEL_TIMEOUT*2;
    }

    ChannelPtr channel = m_channel_manager.getChannel(channel_name);
    long current_offset = channel->getCurrentOffset();

    // Set proper offset
    if(offset == Config::instance().NO_OFFSET && 
        current_offset == Config::instance().WAITING_DATA
    ) {
        offset = Config::instance().WAITING_DATA;
    } else if(offset == Config::instance().NO_OFFSET) {
        offset = channel->getCurrentOffset();
    }

   // We don't want Keep-Alive here
    tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);
    // Set the content type as text
    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    // Do not send content length, because it is unknown now
    writer->getResponse().setDoNotSendContentLength();
    // Send the header
    writer->send();

    PION_LOG_DEBUG(m_logger, "New request is waitting for channel \"" << channel_name << "\"");
    channel->addListener(
        bind(&CometReadService::notifyChannel, this, 
             writer, channel
    ));
}


}   // end namespace lite_comet

