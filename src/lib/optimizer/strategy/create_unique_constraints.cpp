#include "create_unique_constraints.hpp"

#include "logical_query_plan/abstract_lqp_node.hpp"

namespace opossum {

void CreateUniqueConstraintsRule::apply_to(const std::shared_ptr<AbstractLQPNode>& lqp) const {

  for(size_t i = 0; i < 100; ++i) {
    const auto& unique_constraints = lqp->unique_constraints();
    Assert(unique_constraints->size() > 0 || unique_constraints->size() == 0, "Dummy usage");
  }

}

}  // namespace opossum
