# Examples of Synchronous Clocked FMUs

**NOTE: The orchestration code shown in [synchronous_control_me.c](src/synchronous_control_me.c) and [synchronous_control_cs.c](src/synchronous_control_cs.c) do not constitute official FMI recommendations regarding how these FMUs should be orchestrated. These represents one way, for the purposes of testing the FMU and highlighting how the different clocks are used.**

This example consists of a [Controller](./Controller), a [Plant](./Plant), and a [Supervisor](./Supervisor) FMU.
It uses both model exchange and co-simulation interfaces of [FMI 3.0.1](https://fmi-standard.org/docs/3.0.1/)
The Controller FMU declares an input periodic clock, and the Supervisor has an output clock that triggers when a state event occurs.
The output clock of the Supervisor is connected to another input clock of the controller.

The supervisor monitors the plant's continuous output and when a particular event happens, activates a clock and inverts one of the controller's inputs, which in turn changes the controller algorithm.
The plant is just solving a basic ODE with an input from the controller.

The scenario is as follows:

![synchronous_control_me_scenario](synchronous_control_me_scenario.svg)

The key part of the example is to show that both clocks can activate at the same time, if the conditions are right (and the conditions are right roughly once per co-simulation).
When that happens, the dependencies between the variables must be taken into account, in order to correctly propagate the values for the clocked partitions.

The output should look like:

![synchronous_control_me_ref](synchronous_control_me_ref.svg)

The reference result CSV is in [synchronous_control_me_out_ref.csv](synchronous_control_me_out_ref.csv)

The following reference logs are provided:
- Model exchange: [reference_me](./reference_me.log)
- Co-simulation: [reference_cs](./reference_cs.log)

## Building the FMUs and Running Test Script

First make sure to install [cmake](https://cmake.org/).

To build the FMUs run
```powershell
cmake -B build .
cmake --build build
```

The FMUs will be created in `build/fmus`.

The test scripts are compiled into binaries `synchronous_control_me` and `synchronous_control_cs`, running Model Exchange and Co-simulation, respectively, and are available somewhere under the `build` folder.
