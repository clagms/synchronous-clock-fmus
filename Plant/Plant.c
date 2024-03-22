#include "fmi3Functions.h"

typedef enum {
    vr_time, 
    vr_x, 
    vr_der_x,
    vr_u_r
} ValueReference;

typedef struct {
    fmi3Float64 time;
    fmi3Float64 x;
    fmi3Float64 der_x;
    fmi3Float64 u_r;
} Variables;

static Variables variables;

static void setStartValues() {
    variables.time = 0;
    variables.x = 0;
    variables.der_x = 0;
    variables.u_r = 0;
}

fmi3Instance fmi3InstantiateCoSimulation(
    fmi3String                     instanceName,
    fmi3String                     instantiationToken,
    fmi3String                     resourcePath,
    fmi3Boolean                    visible,
    fmi3Boolean                    loggingOn,
    fmi3Boolean                    eventModeUsed,
    fmi3Boolean                    earlyReturnAllowed,
    const fmi3ValueReference       requiredIntermediateVariables[],
    size_t                         nRequiredIntermediateVariables,
    fmi3InstanceEnvironment        instanceEnvironment,
    fmi3LogMessageCallback         logMessage,
    fmi3IntermediateUpdateCallback intermediateUpdate) {

    setStartValues();

    return (fmi3Instance)&variables;
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
    fmi3Boolean toleranceDefined,
    fmi3Float64 tolerance,
    fmi3Float64 startTime,
    fmi3Boolean stopTimeDefined,
    fmi3Float64 stopTime) {

    // TODO: implement
    return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {

    // TODO: implement
    return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {

    // TODO: implement
    return fmi3OK;
}

fmi3Status fmi3GetFloat64(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 values[],
    size_t nValues) {

    for (size_t i = 0; i < nValueReferences; i++) {
        switch (valueReferences[i]) {
        case vr_time:
            values[i] = variables.time;
            break;
        case vr_x:
            values[i] = variables.x;
            break;
        case vr_der_x:
            values[i] = variables.der_x;
            break;
        case vr_u_r:
            values[i] = variables.u_r;
            break;
        default:
            return fmi3Error;
        }
    }

    return fmi3OK;
}

fmi3Status fmi3GetClock(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Clock values[]) {

    // TODO: implement
    return fmi3OK;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Float64 values[],
    size_t nValues) {

    for (size_t i = 0; i < nValueReferences; i++) {
        switch (valueReferences[i]) {
        case vr_x:
            variables.x = values[i];
            break;
        case vr_der_x:
            variables.der_x = values[i];
            break;
        case vr_u_r:
            variables.u_r = values[i];
            break;
        default:
            return fmi3Error;
        }
    }
    
    return fmi3OK;
}

fmi3Status fmi3SetClock(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    const fmi3Clock values[]) {

    // TODO: implement
    return fmi3OK;
}

fmi3Status fmi3DoStep(fmi3Instance instance,
    fmi3Float64 currentCommunicationPoint,
    fmi3Float64 communicationStepSize,
    fmi3Boolean noSetFMUStatePriorToCurrentPoint,
    fmi3Boolean* eventHandlingNeeded,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* earlyReturn,
    fmi3Float64* lastSuccessfulTime) {

    variables.x = variables.x + communicationStepSize * variables.der_x + variables.u_r;

    variables.time = currentCommunicationPoint + communicationStepSize;

    return fmi3OK;
}
