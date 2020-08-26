#include "create_fds.hpp"

#include "logical_query_plan/abstract_lqp_node.hpp"

namespace opossum {

void CreateFDsRule::apply_to(const std::shared_ptr<AbstractLQPNode>& lqp) const {

  for(size_t i = 0; i < 1000; ++i) {
    Assert(!lqp->right_input(), "Did not expect right input node");
    const auto& fds = lqp->left_input()->functional_dependencies();
    Assert(fds.size() > 0 || fds.size() == 0, "Dummy usage");
  }

}

}  // namespace opossum
