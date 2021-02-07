#pragma once

#include "IInterface.h"
#include <memory>

namespace monopoly
{
	std::unique_ptr<IInterface> make_interface(std::string const &interfaceName);
}
