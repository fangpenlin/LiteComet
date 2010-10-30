#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <pion/net/WebServer.hpp>
#include <yaml.h>

#include "Config.hpp"
#include "ShutdownManager.hpp"
#include "Channel.hpp"
#include "CometReadService.hpp"
#include "CometWriteService.hpp"

using namespace std;
using namespace boost::asio;
using namespace pion;
using namespace pion::net;
using namespace YAML;
using namespace lite_comet;

/**
    @brief Print usage of this program
**/
void printUsage() {
    cout << "Usage: lite_comet [config.yaml]" << endl;
}

int main(int argc, const char **argv)
{
    // Port number to run server
    unsigned int port = 8080;
    // Network interface to run server
    string interface = "0.0.0.0";
    // Number of threads to run
    size_t numThreads = 1;

    // Initialize log system (use simple configuration)
    PionLogger main_log(PION_GET_LOGGER("lite_comet"));
    PionLogger pion_log(PION_GET_LOGGER("pion"));
    PION_LOG_SETLEVEL_INFO(main_log);
    PION_LOG_SETLEVEL_INFO(pion_log);
    PION_LOG_CONFIG_BASIC;

    PION_LOG_INFO(main_log, "Initialize Lite Comet server");

    string config_file_name = "config.yaml";
    // Get the configuration file name from args
    if(argc == 2) {
        config_file_name = argv[1];
    } else if(argc > 2) {
        printUsage();
        return 0;
    }

    PION_LOG_INFO(main_log, "Loading configuration from " << config_file_name);

    ifstream yaml_file(config_file_name.c_str());
    Parser parser(yaml_file);

    // Read the configuration yaml doc
    Node doc;
    parser.GetNextDocument(doc);

    // Debug logger level
    if(const Node *debug_node= doc.FindValue("debug")) {
        bool debug;
        *debug_node >> debug;
        if(debug) {
            PION_LOG_SETLEVEL_DEBUG(main_log);
            PION_LOG_SETLEVEL_DEBUG(pion_log);
        }
    }

    // Configuration for lite comet 
    if(const Node *lite_comet_node = doc.FindValue("lite_comet")) {
        PION_LOG_INFO(main_log, "Loading Lite Comet config");
        const Node *node = NULL;
        if((node = lite_comet_node->FindValue("NO_OFFSET"))) {
            *node >> Config::instance().NO_OFFSET;
        }
        if((node = lite_comet_node->FindValue("WAITING_DATA"))) {
            *node >> Config::instance().WAITING_DATA;
        }
        if((node = lite_comet_node->FindValue("NEEDS_RESYNC"))) {
            *node >> Config::instance().NEEDS_RESYNC;
        }
        if((node = lite_comet_node->FindValue("CHANNEL_TIMEOUT"))) {
            *node >> Config::instance().CHANNEL_TIMEOUT;
        }
        if((node = lite_comet_node->FindValue("CHANNEL_MAX_MSG"))) {
            *node >> Config::instance().CHANNEL_MAX_MSG;
        }
        if((node = lite_comet_node->FindValue("CLEANUP_CYCLE"))) {
            *node >> Config::instance().CLEANUP_CYCLE;
        }
    } else {
        PION_LOG_WARN(main_log, 
            "Can't find lite_comet setting in configuration file.");
    }


    // Configuration for server
    if(const Node *server_node = doc.FindValue("server")) {
        PION_LOG_INFO(main_log, "Loading server config");
        // Read port number
        const Node *node = NULL;
        if((node = server_node->FindValue("port"))) {
            *node >> port;
        }
        // Read interface
        if((node = server_node->FindValue("interface"))) {
            *node >> interface;
        }
        // Read number of threads 
        if((node = server_node->FindValue("numThreads"))) {
            *node >> numThreads;
        }
    } else {
        PION_LOG_WARN(main_log, 
            "Can't find server setting in configuration file.");
    }

    try {
        // Address of interface
        const ip::address address(ip::address::from_string(interface));
        // Endpoint to listen
        const ip::tcp::endpoint endpoint(address, port);

        // Cocurrency model
        PionOneToOneScheduler scheduler;
        scheduler.setNumThreads(numThreads);
        
        PION_LOG_INFO(main_log, "Number of threads: " << numThreads);

        ChannelManager channel_manager;

        CometReadService read_service(channel_manager);
        CometWriteService write_service(channel_manager);

        WebServer web_server(scheduler, endpoint);
        web_server.addService("/read", 
            dynamic_cast<WebService *>(&read_service));
        web_server.addService("/write", 
            dynamic_cast<WebService *>(&write_service));
        web_server.start();
        main_shutdown_manager.wait();
    } catch (std::exception& e) {
        PION_LOG_FATAL(main_log, e.what());
    }

    return 0;
}
