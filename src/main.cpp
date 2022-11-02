#include "SHT21.hpp"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <iomanip>
#include <iostream>
#include <string_view>

using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

// TODO LORIS: env variables
constexpr unsigned int g_port{8080};
constexpr std::string_view g_version{"1.0"};

// TODO LORIS: move to router.hpp
class SensorRequestHandler : public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
    {
        // TODO LORIS: handle errors by sending appropriate message to client
        const auto data{SHT21::instance().get()};
        res.setContentType("application/json");
        // TODO LORIS: maybe use https://docs.pocoproject.org/current/Poco.JSON.Stringifier.html ?
        res.send() << "{\"version\":\"" << g_version << "\",\"data\":{\"temperature\":" << data.temperature
                   << ",\"humidity\":" << data.humidity << ",\"cached\":" << std::boolalpha << data.cached << "}}";
    }
};

// TODO LORIS: move to router.hpp
class NotFoundRequestHandler : public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
    {
        res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        res.send();
    }
};

// TODO LORIS: move to router.hpp
class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &req)
    {
        Application &app = Application::instance();
        app.logger().information("Request from %s for %s", req.clientAddress().toString(), req.getURI());

        const auto &uri{req.getURI()};
        if (uri != "/")
        {
            return new NotFoundRequestHandler;
        }
        return new SensorRequestHandler;
    }
};

class App : public ServerApplication
{
    void initialize(Application &self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    int main(const std::vector<std::string> &)
    {
        // TODO LORIS: do I need to use config()?
        UInt16 port = static_cast<UInt16>(config().getUInt("port", g_port));
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
