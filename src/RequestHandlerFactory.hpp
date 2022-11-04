#pragma once

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
    Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &req);
};

class NotFoundRequestHandler : public Poco::Net::HTTPRequestHandler
{
    void handleRequest(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &res);
};

class DataRequestHandler : public Poco::Net::HTTPRequestHandler
{
    void handleRequest(Poco::Net::HTTPServerRequest &, Poco::Net::HTTPServerResponse &res);
};
