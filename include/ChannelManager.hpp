#ifndef __LITE_COMET_CHANNELMANNGER__HEADER__
#define __LITE_COMET_CHANNELMANNGER__HEADER__

#include <string>
#include <map>

#include <boost/utility.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
public:
    ChannelManager() {}
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

};

}   // end namespace lite_comet

#endif

