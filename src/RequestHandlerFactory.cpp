#include "RequestHandlerFactory.hpp"
#include "SHT21.hpp"
#include <Poco/Util/Application.h>
#include <iomanip>

using namespace Poco;
using namespace Poco::Util;

HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const HTTPServerRequest &req)
{
    Application &app = Application::instance();
    app.logger().information("Request from %s for %s", req.clientAddress().toString(), req.getURI());

    const auto &uri{req.getURI()};
    if (uri != "/")
    {
        return new NotFoundRequestHandler;
    }
    return new DataRequestHandler;
}

void DataRequestHandler::handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
{
    // TODO LORIS: handle errors by sending appropriate message to client
    // TODO LORIS: also check which kind of error messages are logged (eg for wrong env var)
    Application &app = Application::instance();
    UInt64 expire_cache_timeout_ms{app.config().getUInt64("SHT21_EXPIRE_CACHE_TIMEOUT_MS")};
    const SHT21::Data data{SHT21::instance(expire_cache_timeout_ms).get()};
    res.setContentType("application/json");
    // TODO LORIS: maybe use https://docs.pocoproject.org/current/Poco.JSON.Stringifier.html ?
    res.send() << "{\"version\":\"" << std::fixed << std::setprecision(1) << CMAKE_PROJECT_VERSION
               << "\",\"data\":{\"temperature\":" << data.temperature << ",\"humidity\":" << data.humidity
               << ",\"cached\":" << std::boolalpha << data.cached << "}}";
}

void NotFoundRequestHandler::handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
{
    res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    res.send();
}
