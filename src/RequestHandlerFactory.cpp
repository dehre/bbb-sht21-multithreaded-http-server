#include "RequestHandlerFactory.hpp"
#include "SHT21.hpp"
#include <Poco/JSON/Object.h>
#include <Poco/Util/Application.h>
#include <iomanip>
#include <iostream>

using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Net;
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

void NotFoundRequestHandler::handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
{
    res.setStatus(HTTPResponse::HTTP_NOT_FOUND);
    res.setContentType("text/plain");
    res.send() << "Resource not found";
}

void DataRequestHandler::handleRequest(HTTPServerRequest &, HTTPServerResponse &res)
{
    try
    {
        Application &app = Application::instance();
        Int64 expire_cache_timeout_ms{app.config().getInt64("SHT21_EXPIRE_CACHE_TIMEOUT_MS")};
        const SHT21::Data data{SHT21::instance(expire_cache_timeout_ms).get()};

        res.setStatus(HTTPResponse::HTTP_OK);
        res.setContentType("application/json");

        Object json{};
        json.set("version", CMAKE_PROJECT_VERSION)
            .set("data", Object{}
                             .set("temperature", data.temperature)
                             .set("humidity", data.humidity)
                             .set("cached", data.cached));
        json.stringify(res.send());
    }
    catch (Poco::Exception &exc)
    {
        std::cerr << exc.displayText() << std::endl;
        res.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        res.setContentType("application/json");
        Object{}.set("error", exc.displayText()).stringify(res.send());
    }
}
