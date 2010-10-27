#ifndef __LITE_COMET_CHANNEL__HEADER__
#define __LITE_COMET_CHANNEL__HEADER__

#include <set>

#include <pion/net/WebService.hpp>
#include <pion/net/HTTPResponseWriter.hpp>

namespace lite_comet {

/**
    Channel for registering event data
**/
class Channel:
    public boost::noncopyable
{
public:
    ///
    static const long NO_OFFSET = 0;
    ///
    static const long WAITING_DATA = -1;
    ///
    static const long NEEDS_RESYNC = -3;
public:
    Channel() {}
    virtual ~Channel() {}

    /**
        @brief Add data to channel
        @param data to add
    **/
    virtual void addData(const std::string&) ;

    /**
        @brief Add data to channel
        @param data to add
    **/
    virtual const std::string getData() const ;

    /**
        @brief Add data to channel
        @param data to add
    **/
    virtual long getCurrentOffset() const ;

    /**
        @brief Determine is this channel active
    **/
    virtual bool isActive(/*current date*/) const ;

    /**
        @brief Mark this channel as active
    **/
    virtual void markActive() ;
};

}   // end namespace lite_comet

#endif

