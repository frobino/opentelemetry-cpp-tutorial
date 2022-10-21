#ifndef DOCKER_COMPOSE
#define DOCKER_COMPOSE 1
#endif

#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <pistache/net.h>

#include "../common/tracer.hpp"

#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/experimental_semantic_conventions.h"

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/common/url_parser.h"

using namespace Pistache;
using namespace Rest;

using namespace opentelemetry::trace;
namespace http_client = opentelemetry::ext::http::client;

class Service
{
private:
  Router router;
  std::shared_ptr<Http::Endpoint> httpEndpoint;

  Address address;
  Http::Client httpClient;

  const std::string serviceName = "service-a";

  void setupRoutes()
  {
    Routes::Get(router, "/ping", Routes::bind(&Service::ping, this));
  }

  void initEndpoint()
  {
    auto opts = Http::Endpoint::options().threads(4);
    httpEndpoint->init(opts);
  }

  void initHttpClient()
  {
    auto opts = Http::Client::options().threads(1).maxConnectionsPerHost(8);
    httpClient.init(opts);
  }

  void sendPingToAnotherService(std::string hostName, std::string svcName)
  {
    auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan("sending ping to"));
    // auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan(serviceName + ": sending ping to: " + svcName));

    // inject current context into http header
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    HttpTextMapCarrier<opentelemetry::ext::http::client::Headers> carrier;
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    prop->Inject(carrier, current_ctx);
    // TODO: translate carrier.headers_ into Pistache headers?
    // c.post(page).body(body).header<Http::Header::ContentType>(header).send();
    // opentelemetry::ext::http::client::Headers h2 = carrier.headers_;
    // auto resp = httpClient.get(hostName).header<opentelemetry::ext::http::client::Headers>(h2).send();

    // ORIGINAL BELOW:
    auto resp = httpClient.get(hostName).send();

    std::cout << "Response from "
              << ": " << svcName << std::endl;
    resp.then(
        [&](Http::Response response)
        {
          // std::cout << "Response from " << ": " << svcName << std::endl;
          std::cout << "Code = " << response.code() << std::endl;
          auto body = response.body();
          if (!body.empty())
            std::cout << "Body = " << body << std::endl;
        },
        [&](std::exception_ptr exc)
        {
          std::cout << "Error..." << std::endl;
        });
  }

  void sendPingToAnotherServiceProp2(std::string hostName, std::string svcName)
  {

    auto http_client = http_client::HttpClientFactory::CreateSync();
    // start scoped active span
    StartSpanOptions options;
    options.kind = SpanKind::kClient;
    opentelemetry::ext::http::common::UrlParser url_parser(hostName);

    auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan("sending ping to" + svcName, options));

    // get global propagator
    HttpTextMapCarrier<opentelemetry::ext::http::client::Headers> carrier;
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    // inject current context into http header
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    prop->Inject(carrier, current_ctx);

    // send http request
    http_client::Result result = http_client->Get(hostName, carrier.headers_);
    if (result)
    {
      std::cout << "Response from "
                << ": " << svcName << std::endl;
      auto status_code = result.GetResponse().GetStatusCode();
      std::cout << "Code = " << status_code << std::endl;
      auto resp_body = result.GetResponse().GetBody();
      std::cout << "Body = " << resp_body.data() << std::endl;
    }
  }

public:
  explicit Service(Address address)
  {
    this->address = address;
    httpEndpoint = std::make_shared<Http::Endpoint>(address);
  }

  void initAndStart()
  {
    initEndpoint();
    setupRoutes();
    initHttpClient();

    // true for docker-compose, false for local
    setUpTracer(DOCKER_COMPOSE, serviceName);

    std::cout << "Listening at: " << address.host() << ":" << address.port().toString() << std::endl;

    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
  }

  void ping(const Rest::Request &request, Http::ResponseWriter writer)
  {
    std::cout << "\n---=== " << serviceName << "===---\n";

    auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan("ping"));

    if (DOCKER_COMPOSE){
      // sendPingToAnotherService("http://0.0.0.0:8082/ping", "service-b");
      sendPingToAnotherServiceProp2("http://172.16.238.11:8082/ping", "service-b");
    } else {
      // sendPingToAnotherService("http://0.0.0.0:8082/ping", "service-b");
      sendPingToAnotherServiceProp2("http://0.0.0.0:8082/ping", "service-b");
    }

    writer.send(Http::Code::Ok, "Hello from " + serviceName);
  }
};
