#include "create_unique_constraints.hpp"

#include "logical_query_plan/abstract_lqp_node.hpp"

namespace opossum {

void CreateUniqueConstraintsRule::apply_to(const std::shared_ptr<AbstractLQPNode>& lqp) const {

  for(auto int i = 0; i < 100; ++i) {
    const auto& fds = lqp->functional_dependencies();
  }

}

}  // namespace opossum
