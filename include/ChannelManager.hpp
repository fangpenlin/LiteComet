#ifndef __LITE_COMET_CHANNELMANNGER__HEADER__
#define __LITE_COMET_CHANNELMANNGER__HEADER__

#include <string>
#include <map>

#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <pion/PionLogger.hpp>

#include "Channel.hpp"

namespace lite_comet {

/**
    Channel manager for managing channels
**/
class ChannelManager:
    public boost::noncopyable
{
public:
    /// Mapping container from string to ChannelPtr
    typedef std::map<std::string, ChannelPtr> ChannelMap;
private:
    ChannelMap m_channels;
    pion::PionLogger m_logger;
    boost::asio::io_service& m_io_service;
    boost::asio::deadline_timer m_timer;
    boost::posix_time::time_duration m_period;
public:
    ChannelManager(
        boost::asio::io_service& io_service, 
        boost::posix_time::time_duration period
    )
        : m_logger(PION_GET_LOGGER("lite_comet.ChannelManager")),
          m_io_service(io_service),
          m_timer(io_service),
          m_period(period)
    {}
    ~ChannelManager() {}

    /**
        @brief Get the channel by name, if it doens't exist, then create it
               and return
        @param name of channel to get
        @return channel
    **/
    ChannelPtr& getChannel(const std::string&) ;

    /**
        @brief Get map to all channels
        @return channel map
    **/
    ChannelMap& getChannelMap() ;
    
    /**
        @brief Reset channel by name
        @param name of channel to reset
    **/
    void reset(const std::string&);

    /**
        @brief Determine is the channel active
        @param name of channel to check
        @return true if the channel is active, otherwise false
    **/
    bool isActive(const std::string&, boost::posix_time::ptime) const;

    bool isActive(const std::string&) const;

    /**
        @brief Start cleanup cycle
    **/
    void startCleanup() ;

    /**
        @brief Stop cleanup cycle
    **/
    void stopCleanup() ;

 private:
    void purgeExpired(const boost::system::error_code&) ;
};

}   // end namespace lite_comet

#endif

