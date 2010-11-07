#include <fstream>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Config.hpp"
#include "CometStatusService.hpp"

using namespace std;
using namespace boost::posix_time;
using namespace pion;
using namespace pion::net;

namespace lite_comet {

/**
    Reference to
    http://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
**/
void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat", ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

void CometStatusService::operator()(
    HTTPRequestPtr& request, 
    TCPConnectionPtr& tcp_conn
) {
    HTTPResponseWriterPtr writer(
        HTTPResponseWriter::create(
            tcp_conn, 
            *request,
            bind(&TCPConnection::finish, tcp_conn)
        )
    );

    size_t memory_usage = 0;
    size_t total_channels = 0;
    size_t active_channels = 0;
    size_t active_listeners = 0;
    size_t active_timeouts = 0;
    size_t data_count = 0;

    double vm;
    double rss;
    process_mem_usage(vm, rss);
    memory_usage = rss;

    ptime cur(microsec_clock::universal_time());
    
    ChannelManager::ChannelMap& map = m_channel_manager.getChannelMap();
    ChannelManager::ChannelMap::iterator i = map.begin();
    for(; i != map.end(); ++i) {
        ChannelPtr channel = i->second;
        ++total_channels;
        if(channel->isActive(cur)) {
            ++active_channels;
        }
        
        active_listeners += channel->getListenerCount();
        data_count += channel->getDataCount();
    }

    stringstream stream;
    stream << "{";
    stream << "\"memory_stats\":" << memory_usage << ",";
    stream << "\"total_channels\":" << total_channels << ",";
    stream << "\"active_channels\":" << active_channels << ",";
    stream << "\"active_listeners\":" << active_listeners << ",";
    stream << "\"active_timeouts\":" << active_timeouts << ",";
    stream << "\"data_count\":" << data_count;
    stream << "}";

    writer->getResponse().setContentType(HTTPTypes::CONTENT_TYPE_TEXT);
    writer->write(stream.str());
    writer->send();
}


}   // end namespace lite_comet

