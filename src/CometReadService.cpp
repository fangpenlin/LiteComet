#include <sstream>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <pion/net/HTTPTypes.hpp>
#include <pion/net/HTTPResponseWriter.hpp>
#include <pion/net/PionUser.hpp>
#include <json/json.h>

#include "Config.hpp"
#include "Utils.hpp"
#include "CometReadService.hpp"
#include "Response.hpp"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

// CometReadService member functions

Json::Value& operator << (Json::Value& value, const Channel::ChannelData& channel_data) {
    value["new_offset"] = static_cast<Json::Int>(channel_data.get<0>());
    Json::Value data;
    Channel::MessageList::const_iterator i = channel_data.get<1>();
    for(; i != channel_data.get<2>(); ++i) {
        data.append((*i).getData());
    }
    value["data"] = data;
    return value;
}

void CometReadService::notifyChannel(
    HTTPResponseWriterPtr writer, 
    ChannelPtr channel,
    CometReadService::TimerPtr timer,
    const string& channel_name,
    const string& js_callback,
    long offset
) {
    PION_LOG_DEBUG(m_logger, "Notify channel \"" << channel_name << "\"");
    // Cahcnel timeout
    m_timers.erase(timer);
    timer->cancel();
    // Get data
    const Channel::ChannelData new_data = channel->getData(offset);
    // write empty back
    if(new_data.get<1>() == new_data.get<2>()) {
        Response::empty(writer, offset, js_callback);
    // write data back
    } else {
        Json::Value root;
        root << new_data;
        Json::FastWriter json_writer;
        Response::data(writer, json_writer.write(root), js_callback);
    }
    writer->send(bind(&TCPConnection::finish, writer->getTCPConnection()));
}

void CometReadService::notifyTimeout(
    HTTPResponseWriterPtr writer, 
    ChannelPtr channel,
    CometReadService::TimerPtr timer,
    Channel::ListenerID listener_id,
    const boost::system::error_code& error,
    const string& channel_name,
    const string& js_callback,
    long offset
) {
    if(!error) {
        PION_LOG_DEBUG(m_logger, "Notify timeout \"" << channel_name << "\"");
        channel->removeListener(listener_id);
        m_timers.erase(timer);
        Response::empty(writer, offset, js_callback);
        writer->send(bind(&TCPConnection::finish, writer->getTCPConnection()));
    } else {
        PION_LOG_ERROR(m_logger, "Timer error: " << error);
    }
}

/// handles requests for EchoService
void CometReadService::operator()(
    HTTPRequestPtr& request, 
    TCPConnectionPtr& tcp_conn
) {
    PION_LOG_DEBUG(m_logger, "Read with query: " << request->getQueryString());

    HTTPResponseWriterPtr writer(
        HTTPResponseWriter::create(
            tcp_conn, 
            *request
        )
    );
    // Comet channel name
    const string channel_name(
        HTTPTypes::url_decode(request->getQuery("channel")));
    // Java script callback
    const string js_callback(
        HTTPTypes::url_decode(request->getQuery("js_callback")));
    // Offset of data
    long offset = Config::instance().NO_OFFSET;
    if(request->hasQuery("offset")) {
        offset = lexical_cast<long>(
            HTTPTypes::url_decode(request->getQuery("offset")));
    }
    // Timeout of waiting time
    long timeout = Config::instance().CHANNEL_TIMEOUT;
    if(request->hasQuery("timeout")) {
        timeout = lexical_cast<long>(
            HTTPTypes::url_decode(request->getQuery("timeout")));
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

        Json::Value root;
        root << new_data;
        Json::FastWriter json_writer;
        Response::data(writer, json_writer.write(root), js_callback);
        writer->send(bind(&TCPConnection::finish, tcp_conn));
    // There is data to return immedinately
    } else if(new_data.get<1>() != new_data.get<2>()) {
        PION_LOG_DEBUG(m_logger, "Return instant data from channel \"" << 
            channel_name << "\"");

        Json::Value root;
        root << new_data;
        Json::FastWriter json_writer;
        Response::data(writer, json_writer.write(root), js_callback);
        writer->send(bind(&TCPConnection::finish, tcp_conn));
    // We need to wait for more data here
    } else { 
        PION_LOG_DEBUG(m_logger, "A request is waitting for channel \"" << 
            channel_name << "\"");

        // set up timeout timer
        shared_ptr<deadline_timer> timer(
            new deadline_timer(tcp_conn->getIOService()));
        timer->expires_from_now(posix_time::milliseconds(timeout));
        m_timers.insert(timer);
       
        // wait for data
        Channel::ListenerID listener_id = channel->addListener(
            bind(&CometReadService::notifyChannel, this, 
                writer, channel, timer, channel_name, js_callback, offset
        ));

        timer->async_wait(
            bind(&CometReadService::notifyTimeout, this, 
                writer, channel, timer, listener_id, placeholders::error,
                channel_name, js_callback, offset
        ));

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

