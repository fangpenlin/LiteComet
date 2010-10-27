#include <sstream>
#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <pion/net/WebServer.hpp>
#include <yaml.h>

#include "ShutdownManager.hpp"
#include "Channel.hpp"
#include "CometReadService.hpp"
#include "CometWriteService.hpp"

using namespace std;
using namespace boost::asio;
using namespace pion;
using namespace pion::net;
using namespace YAML;

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
    PionLogger main_log(PION_GET_LOGGER("PionHelloServer"));
    PionLogger pion_log(PION_GET_LOGGER("pion"));
    PION_LOG_SETLEVEL_INFO(main_log);
    PION_LOG_SETLEVEL_INFO(pion_log);
    PION_LOG_CONFIG_BASIC;

    PION_LOG_INFO(main_log, "Initialize Lite Comet server");

    string configFileName = "config.yaml";
    // Get the configuration file name from args
    if(argc == 2) {
        configFileName = argv[1];
    } else if(argc > 2) {
        printUsage();
        return 0;
    }

    stringstream msg;
    msg << "Loading configuration from " << configFileName;
    PION_LOG_INFO(main_log, msg.str());

    ifstream yamlFile(configFileName.c_str());
    Parser parser(yamlFile);

    // Read the configuration yaml doc
    Node doc;
    parser.GetNextDocument(doc);

    // Configuration for server
    if(const Node *pServerNode = doc.FindValue("server")) {
        PION_LOG_INFO(main_log, "Loading server config");
        // Read port number
        if(const Node *pPortNode = pServerNode->FindValue("port")) {
            *pPortNode >> port;
        }
        // Read interface
        if(const Node *pInterfaceNode = pServerNode->FindValue("interface")) {
            *pInterfaceNode >> interface;
        }
        // Read number of threads 
        if(const Node *pNumThreads = pServerNode->FindValue("numThreads")) {
            *pNumThreads >> numThreads;
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
        
        stringstream msg;
        msg << "Number of threads: " << numThreads;
        PION_LOG_INFO(main_log, msg.str());

        plugins::CometReadService readService;
        plugins::CometWriteService writeService(readService);

        WebServer webServer(scheduler, endpoint);
        webServer.addService("/read", 
            dynamic_cast<WebService *>(&readService));
        webServer.addService("/write", 
            dynamic_cast<WebService *>(&writeService));
        webServer.start();
        main_shutdown_manager.wait();
    } catch (std::exception& e) {
        PION_LOG_FATAL(main_log, e.what());
    }

    return 0;
}
