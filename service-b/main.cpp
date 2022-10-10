/*
  #include "service.hpp"

  using namespace Pistache;

  int main() {
  Address address(Ipv4::any(), Port(8082));

  Service serviceB(address);

  serviceB.initAndStart();
  }
*/

#include "../common/tracer.hpp"
#include "opentelemetry/ext/http/server/http_server.h"
#include <chrono>
#include <string>

namespace
{

  class HttpServer : public HTTP_SERVER_NS::HttpRequestCallback
  {

  protected:
    HTTP_SERVER_NS::HttpServer server_;
    std::string server_url_;
    uint16_t port_;
    std::atomic<bool> is_running_{false};

  public:
    HttpServer(std::string server_name = "test_server", uint16_t port = 8800) : port_(port)
    {
      server_.setServerName(server_name);
      server_.setKeepalive(false);
    }

    void AddHandler(std::string path, HTTP_SERVER_NS::HttpRequestCallback *request_handler)
    {
      server_.addHandler(path, *request_handler);
    }

    void Start()
    {
      if (!is_running_.exchange(true))
      {
        server_.addListeningPort(port_);
        server_.start();
      }
    }

    void Stop()
    {
      if (is_running_.exchange(false))
      {
        server_.stop();
      }
    }

    ~HttpServer() { Stop(); }
  };

} // namespace

namespace
{

  using namespace opentelemetry::trace;
  namespace context = opentelemetry::context;

  uint16_t server_port = 8082;
  constexpr const char *server_name = "localhost";
  const std::string serviceName = "service-b";

  class RequestHandler : public HTTP_SERVER_NS::HttpRequestCallback
  {
  public:
    virtual int onHttpRequest(HTTP_SERVER_NS::HttpRequest const &request,
                              HTTP_SERVER_NS::HttpResponse &response) override
    {
      StartSpanOptions options;
      options.kind = SpanKind::kServer; // server
      std::string span_name = request.uri;

      // extract context from http header
      const HttpTextMapCarrier<std::map<std::string, std::string>> carrier(
          (std::map<std::string, std::string> &)request.headers);
      auto prop = context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
      auto current_ctx = context::RuntimeContext::GetCurrent();
      auto new_context = prop->Extract(carrier, current_ctx);
      options.parent = GetSpan(new_context)->GetContext();

      if (request.uri == "/ping")
      {
        std::cout << "\n---=== " << serviceName << "===---\n";

        // start span with parent context extracted from http header
        auto non_scoped_span = getTracer(serviceName)->StartSpan(serviceName + ": received ping", options);
        /*
         * NOTE: it looks like we CANNOT use scoped spans when calling private
         * methods (the context was not successfully propagated).
         * Falling back on classic non-scoped-span (i.e. specify manually start and end).
         */
        // auto scoped_span = opentelemetry::trace::Scope(getTracer(serviceName)->StartSpan(serviceName + ": received ping", options));

        // some fake calculations to be retuned in response.body
        int result = this->calculateRespA();
        result += this->calculateRespB();
        result += this->calculateRespC();
        response.body = "Hello from " + serviceName + ", result: " + std::to_string(result);
        non_scoped_span->End();
        return 200;
      }
      return 404;
    }

  private:
    int doSomething(int times)
    {
      for (int i = 0; i < times; i++)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
      return times;
    }
    int calculateRespA()
    {
      return this->doSomething(10);
    }
    int calculateRespB()
    {
      return this->doSomething(100);
    }
    int calculateRespC()
    {
      return this->doSomething(5);
    }
  };
} // namespace

int main(int argc, char *argv[])
{
  // The port the validation service listens to can be specified via the command line.
  if (argc > 1)
  {
    server_port = atoi(argv[1]);
  }

  // initAndStart: initEndPoint, setupRoutes
  HttpServer http_server(server_name, server_port);
  RequestHandler req_handler;
  http_server.AddHandler("/ping", &req_handler);

  setUpTracer(false, serviceName);

  std::cout << "Listening at: " << server_name << ":" << server_port << std::endl;

  auto root_span = getTracer(serviceName)->StartSpan(__func__);
  Scope scope(root_span);
  http_server.Start();

  std::cout << "Server is running..Press ctrl-c to exit...\n";
  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(100));
  }
  http_server.Stop();
  root_span->End();
  return 0;
}
