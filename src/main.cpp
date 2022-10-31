#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <string_view>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

constexpr std::string_view g_version{"1.0"};
constexpr double g_temperature{19.12};
constexpr double g_humidity{50.95};

class SensorRequestHandler : public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
    {
        res.setContentType("application/json");
        // TODO LORIS: maybe use https://docs.pocoproject.org/current/Poco.JSON.Stringifier.html ?
        res.send() << "{\"version\":\"" << g_version << "\",\"data\":{\"temperature\":" << g_temperature
                   << ",\"humidity\":" << g_humidity << "}}";
    }
};

class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &req)
    {
        Application &app = Application::instance();
        app.logger().information("Request from %s for %s", req.clientAddress().toString(), req.getURI());

        // TODO LORIS: handle favicon, and any other request not for /
        return new SensorRequestHandler;
    }
};

class WebServerApp : public ServerApplication
{
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string> &)
    {
        UInt16 port = static_cast<UInt16>(config().getUInt("port", 8080));

        HTTPServer srv(new RequestHandlerFactory, port);
        srv.start();
        logger().information("HTTP Server started on port %hu.", port);
        waitForTerminationRequest();
        logger().information("Stopping HTTP Server...");
        srv.stop();

        return Application::EXIT_OK;
    }
};

POCO_SERVER_MAIN(WebServerApp)
