#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

using namespace Poco::Net;

class RequestHandlerFactory : public HTTPRequestHandlerFactory
{
    HTTPRequestHandler *createRequestHandler(const HTTPServerRequest &req);
};

class DataRequestHandler : public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest &, HTTPServerResponse &res);
};

class NotFoundRequestHandler : public HTTPRequestHandler
{
    void handleRequest(HTTPServerRequest &, HTTPServerResponse &res);
};
