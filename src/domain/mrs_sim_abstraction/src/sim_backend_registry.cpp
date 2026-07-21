#include <stdexcept>

#include "mrs_sim_abstraction/sim_backend_registry.hpp"

namespace mrs
{

void SimBackendRegistry::register_backend(
  const std::string & backend_name, std::shared_ptr<ISimBackend> backend)
{
  (void)backend_name;
  (void)backend;
  throw std::logic_error(
    "not implemented: SimBackendRegistry::register_backend — Phase 5 (coordination-builder) 대상");
}

bool SimBackendRegistry::select_backend(const std::string & backend_name)
{
  (void)backend_name;
  throw std::logic_error(
    "not implemented: SimBackendRegistry::select_backend — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
