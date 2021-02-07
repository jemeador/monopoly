#include "Interface.h"
#include "TestInterface.h"
using namespace monopoly;

using namespace std;

std::unique_ptr<IInterface> monopoly::make_interface(std::string const &interfaceName)
{
	return make_unique<TestInterface>();
}