#include "functional_dependency.hpp"

#include "boost/functional/hash.hpp"

namespace opossum {

FunctionalDependency::FunctionalDependency(ExpressionUnorderedSet init_determinants,
                                           ExpressionUnorderedSet init_dependents)
    : determinants(std::move(init_determinants)), dependents(std::move(init_dependents)) {
  DebugAssert(!determinants.empty() && !dependents.empty(), "FunctionalDependency cannot be empty");
}

bool FunctionalDependency::operator==(const FunctionalDependency& other) const {
  // Cannot use unordered_set::operator== because it ignores the custom equality function.
  // https://stackoverflow.com/questions/36167764/can-not-compare-stdunorded-set-with-custom-keyequal

  // Quick check for cardinality
  if (determinants.size() != other.determinants.size() || dependents.size() != other.dependents.size()) return false;

  // Compare determinants
  for (const auto& expression : other.determinants) {
    if (!determinants.contains(expression)) return false;
  }
  // Compare dependants
  for (const auto& expression : other.dependents) {
    if (!dependents.contains(expression)) return false;
  }

  return true;
}

bool FunctionalDependency::operator!=(const FunctionalDependency& other) const { return !(other == *this); }

std::ostream& operator<<(std::ostream& stream, const FunctionalDependency& expression) {
  stream << "{";
  auto determinants_vector =
      std::vector<std::shared_ptr<AbstractExpression>>{expression.determinants.begin(), expression.determinants.end()};
  stream << determinants_vector.at(0)->as_column_name();
  for (auto determinant_idx = size_t{1}; determinant_idx < determinants_vector.size(); ++determinant_idx) {
    stream << ", " << determinants_vector[determinant_idx]->as_column_name();
  }

  stream << "} => {";
  auto dependents_vector =
      std::vector<std::shared_ptr<AbstractExpression>>{expression.dependents.begin(), expression.dependents.end()};
  stream << dependents_vector.at(0)->as_column_name();
  for (auto dependent_idx = size_t{1}; dependent_idx < dependents_vector.size(); ++dependent_idx) {
    stream << ", " << dependents_vector[dependent_idx]->as_column_name();
  }
  stream << "}";

  return stream;
}

std::unordered_set<FunctionalDependency> inflate_fds(const std::vector<FunctionalDependency>& fds) {
  if (fds.empty()) return {};

  auto inflated_fds = std::unordered_set<FunctionalDependency>();
  inflated_fds.reserve(fds.size());

  for (auto& fd : fds) {
    if (fd.dependents.size() == 1) {
      inflated_fds.insert(fd);
    } else {
      for (const auto& dependent : fd.dependents) {
        inflated_fds.emplace(fd.determinants, ExpressionUnorderedSet{dependent});
      }
    }
  }

  return inflated_fds;
}

std::vector<FunctionalDependency> deflate_fds(const std::vector<FunctionalDependency>& fds) {
  if (fds.empty()) return {};

  auto deflated_fds = std::vector<FunctionalDependency>();
  deflated_fds.reserve(fds.size());

  for (const auto& fd_to_add : fds) {
    auto existing_fd = std::find_if(deflated_fds.begin(), deflated_fds.end(), [&fd_to_add](auto& fd) {
      // Cannot use unordered_set::operator== because it ignores the custom equality function.
      // https://stackoverflow.com/questions/36167764/can-not-compare-stdunorded-set-with-custom-keyequal

      // Quick check for cardinality
      if (fd.determinants.size() != fd_to_add.determinants.size()) return false;

      // Compare determinants
      for (const auto& expression : fd_to_add.determinants) {
        if (!fd.determinants.contains(expression)) return false;
      }

      return true;
    });
    if (existing_fd == deflated_fds.end()) {
      deflated_fds.push_back(fd_to_add);
    } else {
      // An FD with the same determinant expressions already exists. Therefore, we only have to add to the dependent
      // expressions set
      existing_fd->dependents.insert(fd_to_add.dependents.begin(), fd_to_add.dependents.end());
    }
  }

  return deflated_fds;
}

std::vector<FunctionalDependency> merge_fds(const std::vector<FunctionalDependency>& fds_a,
                                            const std::vector<FunctionalDependency>& fds_b) {
  if constexpr (HYRISE_DEBUG) {
    auto fds_a_set = std::unordered_set<FunctionalDependency>(fds_a.begin(), fds_a.end());
    auto fds_b_set = std::unordered_set<FunctionalDependency>(fds_b.begin(), fds_b.end());
    Assert(fds_a.size() == fds_a_set.size() && fds_b.size() == fds_b_set.size(),
           "Did not expect input vector to contain multiple FDs with the same determinant expressions");
  }
  if (fds_a.empty() && fds_b.empty()) return {};
  if (fds_a.empty()) return fds_b;
  if (fds_b.empty()) return fds_a;

  auto fds_merged = std::vector<FunctionalDependency>();
  fds_merged.reserve(fds_a.size() + fds_b.size());
  fds_merged.insert(fds_merged.end(), fds_a.begin(), fds_a.end());
  fds_merged.insert(fds_merged.end(), fds_b.begin(), fds_b.end());

  // To get rid of potential duplicates, we call deflate before returning.
  return deflate_fds(fds_merged);
}

std::vector<FunctionalDependency> intersect_fds(const std::vector<FunctionalDependency>& fds_a,
                                                const std::vector<FunctionalDependency>& fds_b) {
  if (fds_a.empty() || fds_b.empty()) return {};

  const auto& inflated_fds_a = inflate_fds(fds_a);
  const auto& inflated_fds_b = inflate_fds(fds_b);

  auto intersected_fds = std::vector<FunctionalDependency>();
  intersected_fds.reserve(fds_a.size());

  for (const auto& fd : inflated_fds_a) {
    if (inflated_fds_b.contains(fd)) {
      intersected_fds.push_back(fd);
    }
  }

  return deflate_fds(intersected_fds);
}

}  // namespace opossum

namespace std {

size_t hash<opossum::FunctionalDependency>::operator()(const opossum::FunctionalDependency& fd) const {
  return boost::hash_range(fd.determinants.cbegin(), fd.determinants.cend());
}

}  // namespace std
