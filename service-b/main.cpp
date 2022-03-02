#include "service.hpp"

using namespace Pistache;

int main() {  
  Address address(Ipv4::any(), Port(8082));

  Service serviceB(address);

  serviceB.initAndStart();
}