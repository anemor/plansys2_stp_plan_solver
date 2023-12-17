# Simultaneous Temporal Planner (STP) solver for PlanSys2
This package contains a plan solver that uses STP from the [Classical Planning for Temporal Planning (CP4TP)](https://github.com/aig-upf/temporal-planning) repo for solving PDDL plans. This is heavily inspired by the [TFD](https://github.com/PlanSys2/plansys2_tfd_plan_solver) plan solver. Uses STP-4 by default, can be manually changed in `src/plansys2_stp_plan_solver`

## Dependencies
1. Follow the instructions in the [Temporal Planning](https://github.com/aig-upf/temporal-planning) repo to install the planner
2. Export a `$STP_HOME` environment variable that points to the `temporal-planning` directory, e.g. `export STP_HOME=${HOME}/plansys2_planners/temporal-planning`. Add it to `~/.bashrc` if you feel like it.

## Installation
1. Clone this repository into your ros2 workspace
``` bash
git clone https://github.com/anemor/plansys2_stp_plan_solver
```
2. Modify the `ros2_planning_system/plansys2_bringup/params/plansys2_params.yaml` to include STP:
``` bash
planner:
  ros__parameters:
    plan_solver_plugins: ["STP"]
    POPF:
      plugin: "plansys2/POPFPlanSolver"
    TFD:
      plugin: "plansys2/TFDPlanSolver"
    STP:
      plugin: "plansys2/STPPlanSolver"
```
