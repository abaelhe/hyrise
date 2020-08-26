#pragma once

#include "abstract_rule.hpp"

namespace opossum {

class AbstractLQPNode;

class CreateFDsRule : public AbstractRule {
 public:
  void apply_to(const std::shared_ptr<AbstractLQPNode>& lqp) const override;
};

}  // namespace opossum
