#include "service.hpp"

using namespace Pistache;

int main() {  
  Address address(Ipv4::any(), Port(8081));

  Service serviceA(address);

  serviceA.initAndStart();      
}