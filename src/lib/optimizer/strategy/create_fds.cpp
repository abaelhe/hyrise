#include "create_fds.hpp"

#include "logical_query_plan/abstract_lqp_node.hpp"

namespace opossum {

void CreateFDsRule::apply_to(const std::shared_ptr<AbstractLQPNode>& lqp) const {

  for(size_t i = 0; i < 100; ++i) {
    const auto& fds = lqp->functional_dependencies();
    Assert(fds.size() >= 0, "Dummy usage");
  }

}

}  // namespace opossum
