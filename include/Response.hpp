#ifndef __LITE_COMET_RESPONSE__HEADER__
#define __LITE_COMET_RESPONSE__HEADER__

#include <string>

#include <pion/net/HTTPResponseWriter.hpp>

namespace lite_comet {

/**
    Util class for sending response
**/
class Response {
public:
    static const std::string INVALID_REQUEST;
    static const std::string NOT_ACTIVE_CHANNEL;
    static const std::string OK;
public:
    /**
        @brief write ok response
    **/
    static void ok(pion::net::HTTPResponseWriterPtr, 
        const std::string&, bool=true) ;

    /**
        @brief write error response
    **/
    static void error(pion::net::HTTPResponseWriterPtr, 
        const std::string&, bool=true) ;

    /**
        @brief write data response
    **/
    static void data(pion::net::HTTPResponseWriterPtr, 
        const std::string&, const std::string&, bool=true) ;

    /**
        @brief write empty response
    **/
    static void empty(pion::net::HTTPResponseWriterPtr, 
        long, const std::string&, bool=true) ;
};

}   // end namespace lite_comet

#endif

