/*
The example consists of a controller and a plant, and a supervisor FMU.
It uses model exchange in FMI3.0. The Controller fmu declares an input periodic clock,
and the supervisor has an output clock that triggers when a state event occurs.
The output clock of the supervisor is connected to another input clock of the controller.

See more details in [README.md](./README.md)
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "FMI3.h"
#include "orchestration_common.h"

// Define struct to hold outputs of FMI3DoStep
typedef struct {
    fmi3Boolean stateEvent;
    fmi3Boolean terminateSimulation;
    fmi3Boolean earlyReturn;
    fmi3Float64 last_successul_time;
} FMI3DoStepOutput;


int main(int argc, char *argv[])
{
    printf("Running Supervisory Control example... \n");

    FMIStatus status = FMIOK;
    fmi3Float64 h = FIXED_STEP;
    fmi3Float64 tNext = h;
    fmi3Float64 time = 0;

    // Flags for FMI3UpdateDiscreteStates
    FMI3UpdateDiscreteStatesOutput controller_FMI3UpdateDiscreteStatesOutput = { fmi3False, fmi3False, fmi3False, fmi3False, fmi3False, 0.0 };
    FMI3UpdateDiscreteStatesOutput plant_FMI3UpdateDiscreteStatesOutput = { fmi3False, fmi3False, fmi3False, fmi3False, fmi3False, 0.0 };
    FMI3UpdateDiscreteStatesOutput supervisor_FMI3UpdateDiscreteStatesOutput = { fmi3False, fmi3False, fmi3False, fmi3False, fmi3False, 0.0 };

    // Flags for FMI3DoStep
    FMI3DoStepOutput controller_FMI3DoStepOutput = { fmi3False, fmi3False, fmi3False, 0.0 };
    FMI3DoStepOutput plant_FMI3DoStepOutput = { fmi3False, fmi3False, fmi3False, 0.0 };
    FMI3DoStepOutput supervisor_FMI3DoStepOutput = { fmi3False, fmi3False, fmi3False, 0.0 };

    // Will hold exchanged values: Controller -> Plantmodel
    fmi3Float64 controller_vals[] = { 0.0 };
    // Will hold exchanged values: Plantmodel -> Controller
    fmi3Float64 plantmodel_vals[] = { 0.0 };
    fmi3Float64 plantmodel_der_vals[] = { 0.0 };

    // Controller's clock r timer
    fmi3Float64 controller_r_period = 0.0;
    fmi3Float64 controller_r_timer = 0.0;
    // Will hold output from FMI3GetIntervalDecimal
    fmi3Float64 controller_interval_vals[] = { 0.0 };
    fmi3IntervalQualifier controller_interval_qualifiers[] = { fmi3IntervalNotYetKnown };

    // Open file
    FILE * outputFile = initializeFile("synchronous_control_cs_out.csv");
    if (!outputFile) {
        return EXIT_FAILURE;
    }

    // Instantiate
    FMIInstance* controller = FMICreateInstance("controller", "Controller" BINARY_DIR "Controller" BINARY_EXT, logMessage, logFunction);
    FMIInstance* plant = FMICreateInstance("plant", "Plant"      BINARY_DIR "Plant"      BINARY_EXT, logMessage, logFunction);
    FMIInstance* supervisor = FMICreateInstance("supervisor", "Supervisor" BINARY_DIR "Supervisor" BINARY_EXT, logMessage, logFunction);

    if (!controller || !plant || !supervisor) {
        puts("Failed to load shared libraries.");
        return FMIError;
    }

    CALL(FMI3InstantiateCoSimulation(controller, "{e1f14bf0-302d-4ef9-b11c-e01c7ed456cb}", NULL, fmi3False, fmi3True, fmi3True, fmi3False, NULL, 0, NULL));
    CALL(FMI3InstantiateCoSimulation(plant,      "{6e81b08d-97be-4de1-957f-8358a4e83184}", NULL, fmi3False, fmi3True, fmi3True, fmi3False, NULL, 0, NULL));
    CALL(FMI3InstantiateCoSimulation(supervisor, "{64202d14-799a-4379-9fb3-79354aec17b2}", NULL, fmi3False, fmi3True, fmi3True, fmi3False, NULL, 0, NULL));

    // Initialize
    CALL(FMI3EnterInitializationMode(controller, fmi3False, 0.0, tStart, fmi3True, tEnd));
    CALL(FMI3EnterInitializationMode(plant,      fmi3False, 0.0, tStart, fmi3True, tEnd));
    CALL(FMI3EnterInitializationMode(supervisor, fmi3False, 0.0, tStart, fmi3True, tEnd));

    // Exchange data Controller -> Plantmodel
    CALL(FMI3GetFloat64(controller, controller_y_refs, 1, controller_vals, 1));
    CALL(FMI3SetFloat64(plant,      plantmodel_u_refs, 1, controller_vals, 1));

    // Exchange data Plantmodel -> Supervisor
    CALL(FMI3SetFloat64(supervisor, supervisor_in_refs, 1, plantmodel_vals, 1));

    // Get clock r's interval
    CALL(FMI3GetIntervalDecimal(controller, controller_r_refs, 1, controller_interval_vals, controller_interval_qualifiers));
    controller_r_period = controller_interval_vals[0];
    controller_r_timer = controller_r_period;

    CALL(FMI3ExitInitializationMode(controller));
    CALL(FMI3ExitInitializationMode(plant));
    CALL(FMI3ExitInitializationMode(supervisor));

    time = tStart;

    CALL(FMI3EnterStepMode(controller));
    CALL(FMI3EnterStepMode(plant));
    CALL(FMI3EnterStepMode(supervisor));

    // Record initial outputs
    CALL(recordVariables(outputFile, controller, plant, time));

    // Get number of simulation steps to be carried out. 
    int nSteps = (int)ceil((tEnd - tStart) / h) + 1; // +1 to include tEnd

    // Main simulation loop
    for (int i = 0; i < nSteps; i++) {
        CALL(FMI3DoStep(controller, time, h, fmi3True, 
            &controller_FMI3DoStepOutput.stateEvent, 
            &controller_FMI3DoStepOutput.terminateSimulation, 
            &controller_FMI3DoStepOutput.earlyReturn, 
            &controller_FMI3DoStepOutput.last_successul_time));
        CALL(FMI3DoStep(plant, time, h, fmi3True, 
            &plant_FMI3DoStepOutput.stateEvent, 
            &plant_FMI3DoStepOutput.terminateSimulation, 
            &plant_FMI3DoStepOutput.earlyReturn, 
            &plant_FMI3DoStepOutput.last_successul_time));
        CALL(FMI3DoStep(supervisor, time, h, fmi3True, 
            &supervisor_FMI3DoStepOutput.stateEvent, 
            &supervisor_FMI3DoStepOutput.terminateSimulation, 
            &supervisor_FMI3DoStepOutput.earlyReturn, 
            &supervisor_FMI3DoStepOutput.last_successul_time));

        // Advance time and update timers
        time = tStart + i*h;
        controller_r_timer -= h;

        // Check for state events or time events.
        bool timeEvent = controller_r_timer <= 0.0;
        bool stateEvent = false;
        // Update stateEvent
        stateEvent = controller_FMI3DoStepOutput.stateEvent || plant_FMI3DoStepOutput.stateEvent || supervisor_FMI3DoStepOutput.stateEvent;

        // Exchange data Plantmodel -> Supervisor
        CALL(FMI3GetFloat64(plant, plantmodel_y_refs, 1, plantmodel_vals, 1));
        CALL(FMI3SetFloat64(supervisor, supervisor_in_refs, 1, plantmodel_vals, 1));

        // Record data
        CALL(recordVariables(outputFile, controller, plant, time));

        printf("Time event: %d \t State Event: %d \n", timeEvent, stateEvent);

        // Check if controller or supervisor need to execute
        if (timeEvent || stateEvent) {
            if (timeEvent && !stateEvent) {
                printf("Entering event mode for ticking clock r. \n");

                // Reset timer
                controller_r_timer = controller_r_period;

                // Put Controller into event mode, as clocks are about to tick
                CALL(FMI3EnterEventMode(controller));
                // Put Plantmodel into event mode, as its input is a discrete time variable
                CALL(FMI3EnterEventMode(plant));

                // Handle time event in controller
                CALL(handleTimeEventController(controller, plant));

                // Update discrete states of the controller
                CALL(FMI3UpdateDiscreteStates(controller, 
                    &controller_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &controller_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTime));
                CALL(FMI3UpdateDiscreteStates(plant, 
                    &plant_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &plant_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &plant_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nextEventTime));

                // Exit event mode
                CALL(FMI3EnterStepMode(controller));
                CALL(FMI3EnterStepMode(plant));
                printf("Exiting event mode. \n");
            }
            else if (!timeEvent && stateEvent) {
                printf("Entering event mode for ticking clock s. \n");

                // Put Supervisor and Controller into event mode, as clocks are about to tick. Note the flags used.
                CALL(FMI3EnterEventMode(supervisor));
                CALL(FMI3EnterEventMode(controller));

                // Handle state event supervisor
                CALL(handleStateEventSupervisor(controller, supervisor));

                // Update discrete states of the controller and supervisor
                CALL(FMI3UpdateDiscreteStates(supervisor, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nextEventTime));
                CALL(FMI3UpdateDiscreteStates(controller, 
                    &controller_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &controller_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTime));

                // Exit event mode
                CALL(FMI3EnterStepMode(supervisor));
                CALL(FMI3EnterStepMode(controller));
                printf("Exiting event mode. \n");
            }
            else {
                assert(timeEvent && stateEvent);

                // Reset timer
                controller_r_timer = controller_r_period;

                // Handle both time event and state event.
                printf("Entering event mode for ticking clocks s and r. \n");
                // Now we must respect the dependencies. The supervisor gets priority, and then the controller.

                // Put Supervisor and Controller into event mode, as clocks are about to tick.
                CALL(FMI3EnterEventMode(supervisor));
                CALL(FMI3EnterEventMode(controller));
                // Put Plantmodel into event mode, as its input is a discrete time variable
                CALL(FMI3EnterEventMode(plant));

                CALL(handleStateEventSupervisor(controller, supervisor));
                CALL(handleTimeEventController(controller, plant));

                // Update discrete states of the controller and supervisor
                CALL(FMI3UpdateDiscreteStates(supervisor, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &supervisor_FMI3UpdateDiscreteStatesOutput.nextEventTime));
                CALL(FMI3UpdateDiscreteStates(controller, 
                    &controller_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &controller_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &controller_FMI3UpdateDiscreteStatesOutput.nextEventTime));
                CALL(FMI3UpdateDiscreteStates(plant, 
                    &plant_FMI3UpdateDiscreteStatesOutput.discreteStatesNeedUpdate, 
                    &plant_FMI3UpdateDiscreteStatesOutput.terminateSimulation, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nominalsChanged, 
                    &plant_FMI3UpdateDiscreteStatesOutput.statesChanged, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nextEventTimeDefined, 
                    &plant_FMI3UpdateDiscreteStatesOutput.nextEventTime));

                // Exit event mode
                CALL(FMI3EnterStepMode(supervisor));
                CALL(FMI3EnterStepMode(controller));
                CALL(FMI3EnterStepMode(plant));
                printf("Exiting event mode. \n");
            }
        }
    }

    // Trigger reset just for testing purposes
    CALL(FMI3Reset(controller));
    CALL(FMI3Reset(plant));
    CALL(FMI3Reset(supervisor));

    CALL(FMI3Terminate(controller));
    CALL(FMI3Terminate(plant));
    CALL(FMI3Terminate(supervisor));

TERMINATE:

    CALL(FMI3FreeInstance(controller));
    CALL(FMI3FreeInstance(plant));
    CALL(FMI3FreeInstance(supervisor));

    fclose(outputFile);

    printf("Done! \n");

    return status == FMIOK ? EXIT_SUCCESS : EXIT_FAILURE;
}
