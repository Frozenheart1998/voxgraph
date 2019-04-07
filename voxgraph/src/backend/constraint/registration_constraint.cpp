//
// Created by victor on 17.01.19.
//

#include "voxgraph/backend/constraint/registration_constraint.h"

namespace voxgraph {
void RegistrationConstraint::addToProblem(const NodeCollection &node_collection,
                                          ceres::Problem *problem) {
  CHECK_NOTNULL(problem);

  ceres::LossFunction *loss_function = nullptr;

  // Get pointers to both submap nodes
  SubmapNode::Ptr first_submap_node_ptr =
      node_collection.getNodePtrBySubmapId(config_.first_submap_id);
  SubmapNode::Ptr second_submap_node_ptr =
      node_collection.getNodePtrBySubmapId(config_.second_submap_id);
  CHECK_NOTNULL(first_submap_node_ptr);
  CHECK_NOTNULL(second_submap_node_ptr);

  // Add the submap parameters to the problem
  first_submap_node_ptr->addToProblem(problem);
  second_submap_node_ptr->addToProblem(problem);

  // TODO(victorr): Load cost options from ROS params instead of using default
  SubmapRegisterer::Options::CostFunction cost_options;

  // Create submap alignment cost function
  ceres::CostFunction *cost_function;
  if (cost_options.cost_function_type ==
      SubmapRegisterer::Options::CostFunction::Type::kNumeric) {
    cost_function = nullptr;
    LOG(FATAL) << "Numeric cost not yet implemented";
  } else {
    cost_function = new RegistrationCostFunctionXYZYaw(
        first_submap_ptr_, second_submap_ptr_, cost_options);
  }

  // Add the constraint to the optimization and keep track of it
  residual_block_id_ = problem->AddResidualBlock(
      cost_function, loss_function, first_submap_node_ptr->getPosePtr()->data(),
      second_submap_node_ptr->getPosePtr()->data());
}
}  // namespace voxgraph