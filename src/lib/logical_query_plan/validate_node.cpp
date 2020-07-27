#include "validate_node.hpp"

#include <string>

namespace opossum {

ValidateNode::ValidateNode() : AbstractLQPNode(LQPNodeType::Validate) {}

std::string ValidateNode::description(const DescriptionMode mode) const { return "[Validate]"; }

std::shared_ptr<LQPUniqueConstraints> ValidateNode::unique_constraints() const {
  return _forward_left_unique_constraints();
}

std::vector<FunctionalDependency> ValidateNode::functional_dependencies() const {
  return _remove_invalid_fds(left_input()->functional_dependencies());
}

std::shared_ptr<AbstractLQPNode> ValidateNode::_on_shallow_copy(LQPNodeMapping& node_mapping) const {
  return ValidateNode::make();
}

bool ValidateNode::_on_shallow_equals(const AbstractLQPNode& rhs, const LQPNodeMapping& node_mapping) const {
  return true;
}

}  // namespace opossum
