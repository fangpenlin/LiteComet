#include <sstream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>

#include "Config.hpp"
#include "CometReadService.hpp"
#include "Response.hpp"

using namespace std;
using namespace boost;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

// CometReadService member functions

/**
    @brief convert message range to string
**/
const string messagesToString(
    Channel::MessageList::const_iterator begin, 
    Channel::MessageList::const_iterator end
) {
    stringstream result;
    result << "[";
    Channel::MessageList::const_iterator i = begin; 
    for(; i != end; ++i) {
        result << "'" << (*i).getData() << "'";
        if(next(i) != end) {
            result << ",";
        }
    }
    result << "]";
    return result.str();
}

const string channelDataToString(
    const Channel::ChannelData& data
) {
    stringstream result;
    result << "{"
        << "new_offset:" << data.get<0>() << ","
        << "data:" << messagesToString(data.get<1>(), data.get<2>())
    << "}";
    return result.str();
}

void CometReadService::notifyChannel(
    const string& channel_name,
    const string& js_callback,
    long offset,
    long timeout,
    HTTPRequestPtr request,
    HTTPResponseWriterPtr writer, 
    ChannelPtr channel
) {
    PION_LOG_DEBUG(m_logger, "Notify channel \"" << channel_name << "\"");
    const Channel::ChannelData new_data = channel->getData(offset);
    // write empty back
    if(new_data.get<1>() == new_data.get<2>()) {
        Response::empty(writer, offset, js_callback);
    // write data back
    } else {
        Response::data(writer, channelDataToString(new_data), js_callback);
    }
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
    // Java script callback
    const string js_callback(request->getQuery("js_callback"));
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
    
    const Channel::ChannelData new_data = channel->getData(offset);
    if(new_data.get<0>() == Config::instance().NEEDS_RESYNC) {
        PION_LOG_DEBUG(m_logger, "Resync channel \"" << 
            channel_name << "\"");
        Response::data(writer, channelDataToString(new_data), js_callback);
        writer->send(bind(&TCPConnection::finish, tcp_conn));
    // There is data to return immedinately
    } else if(new_data.get<1>() != new_data.get<2>()) {
        PION_LOG_DEBUG(m_logger, "Return instant data from channel \"" << 
            channel_name << "\"");
        Response::data(writer, channelDataToString(new_data), js_callback);
        writer->send(bind(&TCPConnection::finish, tcp_conn));
    // We need to wait for more data here
    } else { 
        PION_LOG_DEBUG(m_logger, "A request is waitting for channel \"" << 
            channel_name << "\"");
        channel->addListener(
            bind(&CometReadService::notifyChannel, this, 
                channel_name, js_callback, offset, timeout, request, writer, 
                channel
        ));

        // TODO set timer and timeout function

        // We don't want Keep-Alive here
        tcp_conn->setLifecycle(TCPConnection::LIFECYCLE_CLOSE);
        // Set the content type as text
        writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
        // Do not send content length, because it is unknown now
        writer->getResponse().setDoNotSendContentLength();
        // Send the header
        writer->send();
    }

}


}   // end namespace lite_comet

