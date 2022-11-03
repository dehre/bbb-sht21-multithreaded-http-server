#include "RequestHandlerFactory.hpp"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

class App : public ServerApplication
{
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string> &)
    {
        UInt16 port = static_cast<UInt16>(config().getUInt("PORT"));
        HTTPServer srv(new RequestHandlerFactory, port);
        srv.start();
        logger().information("HTTP Server started on port %hu.", port);
        waitForTerminationRequest();
        logger().information("Stopping HTTP Server...");
        srv.stop();
        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(App)
