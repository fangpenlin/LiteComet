#ifndef __LITE_COMET_CONFIG__HEADER__
#define __LITE_COMET_CONFIG__HEADER__

namespace lite_comet {

/**
    Configuration of lite comet
**/
class Config
{
public:
    long NO_OFFSET;
    long WAITING_DATA;
    long NEEDS_RESYNC;
    long CHANNEL_TIMEOUT;
    long CHANNEL_MAX_MSG;
    long CLEANUP_CYCLE;

    static Config& instance() ;
private:
    Config() 
        : NO_OFFSET(0),
          WAITING_DATA(-1),
          NEEDS_RESYNC(-3),
          CHANNEL_TIMEOUT(10000),
          CHANNEL_MAX_MSG(20),
          CLEANUP_CYCLE(5000)
    {}
};

}   // end namespace lite_comet

#endif

