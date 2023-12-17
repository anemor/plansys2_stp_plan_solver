// Copyright 2019 Intelligent Robotics Lab
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>

#include "plansys2_msgs/msg/plan_item.hpp"
#include "plansys2_stp_plan_solver/stp_plan_solver.hpp"

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(plansys2::STPPlanSolver, plansys2::PlanSolverBase);

namespace plansys2
{

STPPlanSolver::STPPlanSolver()
{
}

void
STPPlanSolver::configure(rclcpp_lifecycle::LifecycleNode::SharedPtr & node, const std::string & id)
{
  char * planner_path = getenv("STP_HOME");

  if (planner_path == NULL) {
    RCLCPP_FATAL(node->get_logger(), "'STP_HOME' end not defined for %s", id.c_str());
    exit(-1);
  }

  stp_path_ = std::string(planner_path);

  RCLCPP_INFO(node->get_logger(), "Planner path set to: %s", stp_path_.c_str());
}

std::optional<plansys2_msgs::msg::Plan>
STPPlanSolver::getPlan(
  const std::string & domain, const std::string & problem,
  const std::string & node_namespace)
{
  std::string path = "/tmp";
  if (node_namespace != "") {
    //  This doesn't work as cxx flags must apper at end of link options, and I didn't
    //  find a way
    // std::experimental::filesystem::create_directories("/tmp/" + node_namespace);
    mkdir(("/tmp/" + node_namespace).c_str(), ACCESSPERMS);
    path = "/tmp/" + node_namespace;
  }

  plansys2_msgs::msg::Plan ret;
  std::ofstream domain_out(path + "/domain.pddl");
  domain_out << domain;
  domain_out.close();

  std::ofstream problem_out(path + "/problem.pddl");
  problem_out << problem;
  problem_out.close();

  // Currently the only way to change the solver is to do so manually here, use any solvers 
  // possible in the temporal-planning repository.
  // Same goes for plan_file, which can also be tmp_sas_plan.1
  const std::string solver =    "stp-4";
  const std::string plan_file = "tmp_sas_plan.2";

  // Run bin/plan.py from the temporal-planning reposotory, with the given solver.
  // This will create output files in the current working directory, which is why it's needed to cd /tmp/ first
  system(
    ("cd " + path + "&& ls && " + 
    "python2.7 " + stp_path_ + "/bin/plan.py " + 
    solver + " " + 
    path + "/domain.pddl " + 
    path + "/problem.pddl" +
    "&& cd -" ).c_str()
  );
  // Get the plan in the given plan file.
  std::string line;
  std::ifstream plan_file(stp_path_ + "/" + plan_file);
  bool solution = false;

  if (plan_file.is_open()) {
    while (getline(plan_file, line)) {
      plansys2_msgs::msg::PlanItem item;
      size_t colon_pos = line.find(":");
      size_t colon_par = line.find(")");
      size_t colon_bra = line.find("[");

      std::string time = line.substr(0, colon_pos);
      std::string action = line.substr(colon_pos + 2, colon_par - colon_pos - 1);
      std::string duration = line.substr(colon_bra + 1);
      duration.pop_back();

      item.time = std::stof(time);
      item.action = action;
      item.duration = std::stof(duration);

      ret.items.push_back(item);
    }
    plan_file.close();
  }

  if (ret.items.empty()) {
    return {};
  } else {
    return ret;
  }
}

}  // namespace plansys2