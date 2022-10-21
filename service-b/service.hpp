#ifndef DOCKER_COMPOSE
#define DOCKER_COMPOSE 1
#endif

#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <pistache/net.h>

#include "../common/tracer.hpp"

using namespace Pistache;
using namespace Rest;

using namespace opentelemetry::trace;
namespace http_client = opentelemetry::ext::http::client;
namespace context = opentelemetry::context;

class Service
{
private:
  Router router;
  std::shared_ptr<Http::Endpoint> httpEndpoint;

  Address address;

  const std::string serviceName = "service-b";

  void setupRoutes()
  {
    Routes::Get(router, "/ping", Routes::bind(&Service::ping, this));
  }

  void initEndpoint()
  {
    auto opts = Http::Endpoint::options().threads(4);
    httpEndpoint->init(opts);
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

    // true for docker-compose, false for local
    setUpTracer(DOCKER_COMPOSE, serviceName);

    std::cout << "Listening at: " << address.host() << ":" << address.port().toString() << std::endl;

    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
  }

  void ping(const Rest::Request &request, Http::ResponseWriter writer)
  {
    std::cout << "\n---=== " << serviceName << "===---\n";

    StartSpanOptions options;
    options.kind = SpanKind::kServer;
    request.headers();

    // extract context from http header
    HttpTextMapCarrier<http_client::Headers> carrier;
    auto prop = context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    auto current_ctx = context::RuntimeContext::GetCurrent();
    auto new_context = prop->Extract(carrier, current_ctx);
    options.parent = GetSpan(new_context)->GetContext();

    // trace(serviceName, serviceName + ": received ping");
    auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan(serviceName + ": received ping", options));

    writer.send(Http::Code::Ok, "Hello from " + serviceName);
  }
};
