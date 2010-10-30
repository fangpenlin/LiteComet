#ifndef __LITE_COMET_CHANNELMANNGER__HEADER__
#define __LITE_COMET_CHANNELMANNGER__HEADER__

#include <string>
#include <map>

#include <boost/utility.hpp>

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
        @brief Reset channel by name
        @param name of channel to reset
    **/
    void reset(const std::string&);
};

}   // end namespace lite_comet

#endif

