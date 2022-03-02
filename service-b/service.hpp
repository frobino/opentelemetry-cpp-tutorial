#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/client.h>
#include <pistache/net.h>

#include "../common/tracer.hpp"

using namespace Pistache;
using namespace Rest;

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
        // setUpTracer(true);
        setUpTracer(false);

        std::cout << "Listening at: " << address.host() << 
          ":" << address.port().toString() << std::endl;

        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();        
    }    

    void ping(const Rest::Request& request, Http::ResponseWriter writer)
    {
      std::cout << "\n---=== " << serviceName << "===---\n";

      trace(serviceName, serviceName + ": received ping");                 
      
      writer.send(Http::Code::Ok, "Hello from " + serviceName);
    }    
};
