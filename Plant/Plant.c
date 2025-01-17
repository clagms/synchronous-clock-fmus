#include "fmi3Functions.h"
#include "Custom.h"

#define MAX_MSG_SIZE 100

typedef enum {
	vr_time = 0, // time
	vr_x = 1, // continuous state
	vr_der_x = 2, // derivative of continuous state
	vr_u = 3 // input
} ValueReference;

typedef struct {
	fmi3Float64 time; // time
	fmi3Float64 x; // continuous state
	fmi3Float64 der_x; // derivative of continuous state
	fmi3Float64 u; // input
} PlantData;

typedef struct {

	const char* instanceName;

	// callback functions
	fmi3LogMessageCallback logMessage;

	void* componentEnvironment;

	PlantData data;

} PlantInstance;

fmi3Instance fmi3InstantiateModelExchange(
	fmi3String                 instanceName,
	fmi3String                 instantiationToken,
	fmi3String                 resourcePath,
	fmi3Boolean                visible,
	fmi3Boolean                loggingOn,
	fmi3InstanceEnvironment    instanceEnvironment,
	fmi3LogMessageCallback     logMessage) {

	PlantInstance* comp = (PlantInstance*)calloc(1, sizeof(PlantInstance));

	if (!comp) return NULL;

	comp->instanceName = instanceName;
	comp->logMessage = logMessage;
	comp->componentEnvironment = instanceEnvironment;

	fmi3Reset((fmi3Instance)comp);

	return (fmi3Instance)comp;
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
    
	PlantInstance* comp = (PlantInstance*)calloc(1, sizeof(PlantInstance));

	if (!comp) return NULL;

	comp->instanceName = instanceName;
	comp->logMessage = logMessage;
	comp->componentEnvironment = instanceEnvironment;

	fmi3Reset((fmi3Instance)comp);

	return (fmi3Instance)comp;
}

fmi3Status fmi3Reset(fmi3Instance instance) {
	fmi3Status status = fmi3OK;
	PlantInstance* comp = (PlantInstance*)instance;

	comp->data.time = 0.0;
	comp->data.x = 0.0;
	comp->data.der_x = 0.0;
	comp->data.u = 0.0;

	return status;
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
	fmi3Boolean toleranceDefined,
	fmi3Float64 tolerance,
	fmi3Float64 startTime,
	fmi3Boolean stopTimeDefined,
	fmi3Float64 stopTime) {
	return fmi3OK;
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {
	return fmi3OK;
}

fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance) {
	return fmi3OK;
}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 intervals[],
    fmi3IntervalQualifier qualifiers[]) {

	char msg_buff[MAX_MSG_SIZE];

	fmi3Status status = fmi3Error;

	PlantInstance* comp = (PlantInstance*)instance;

	snprintf(msg_buff, MAX_MSG_SIZE, "Function not relevant for this fmu.");
	comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);

	return status;
}

fmi3Status fmi3GetIntervalFraction(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 counters[],
    fmi3UInt64 resolutions[],
    fmi3IntervalQualifier qualifiers[]) {

	char msg_buff[MAX_MSG_SIZE];

	fmi3Status status = fmi3Error;

	PlantInstance* comp = (PlantInstance*)instance;

	snprintf(msg_buff, MAX_MSG_SIZE, "Function not relevant for this fmu.");
	comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);

	return status;
}


fmi3Status fmi3GetShiftDecimal(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 shifts[]) {
    char msg_buff[MAX_MSG_SIZE];

	fmi3Status status = fmi3Error;

	PlantInstance* comp = (PlantInstance*)instance;

	snprintf(msg_buff, MAX_MSG_SIZE, "Function not relevant for this fmu.");
	comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);

	return status;
}

fmi3Status fmi3GetShiftFraction(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3UInt64 counters[],
    fmi3UInt64 resolutions[]) {
    char msg_buff[MAX_MSG_SIZE];

	fmi3Status status = fmi3Error;

	PlantInstance* comp = (PlantInstance*)instance;

	snprintf(msg_buff, MAX_MSG_SIZE, "Function not relevant for this fmu.");
	comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);

	return status;
}


fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance,
    size_t* nEventIndicators) {
	*nEventIndicators = 0;
    return fmi3OK;
}

fmi3Status fmi3GetEventIndicators(fmi3Instance instance,
    fmi3Float64 eventIndicators[],
    size_t nEventIndicators) {
    
	char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nEventIndicators == 0) return status;

	if (nEventIndicators != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nEventIndicators: %zd.", nEventIndicators);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	return status;
}

fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance,
    size_t* nContinuousStates) {
    *nContinuousStates = 1;
    return fmi3OK;
}

void update_derivative(PlantInstance* comp) {
	comp->data.der_x = - comp->data.x + comp->data.u;
}

fmi3Status fmi3GetContinuousStateDerivatives(fmi3Instance instance,
    fmi3Float64 derivatives[],
    size_t nContinuousStates) {

    char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 1) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %zd.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	update_derivative(comp);

	derivatives[0] = comp->data.der_x;

	return status;
}

fmi3Status fmi3GetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	fmi3Status s;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_time:
			values[i] = comp->data.time;
			s = fmi3OK;
			break;
		case vr_x:
			values[i] = comp->data.x;
			s = fmi3OK;
			break;
		case vr_der_x:
			values[i] = comp->data.der_x;
			s = fmi3OK;
			break;
		case vr_u:
			values[i] = comp->data.u;
			s = fmi3OK;
			break;
		default:
			snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected value reference: %d.", vr);
			comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
			s = fmi3Error;
		}
		status = max(status, s);
		if (status > fmi3Warning) return status;
	}

	return status;
}

fmi3Status fmi3GetClock(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	fmi3Clock values[]) {

	// TODO: implement
	return fmi3OK;
}

fmi3Status fmi3GetContinuousStates(fmi3Instance instance,
    fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
	char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 1) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %zd.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	continuousStates[0] = comp->data.x;

	return status;
}

fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time) {
	PlantInstance* comp = (PlantInstance*)instance;
	comp->data.time = time;
	return fmi3OK;
}

fmi3Status fmi3SetContinuousStates(fmi3Instance instance,
    const fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
    char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 1) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %zd.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	comp->data.x = continuousStates[0];

	return status;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	const fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	fmi3Status s;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_u:
			comp->data.u = values[i];
			s = fmi3OK;
			break;
		default:
			snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected value reference: %d.", vr);
			comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
			s = fmi3Error;
		}
		status = max(status, s);
		if (status > fmi3Warning) return status;
	}

	return status;
}

fmi3Status fmi3SetClock(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	const fmi3Clock values[]) {
	return fmi3OK;
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance) {
	return fmi3OK;
}

fmi3Status fmi3UpdateDiscreteStates(fmi3Instance instance,
    fmi3Boolean* discreteStatesNeedUpdate,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* nominalsOfContinuousStatesChanged,
    fmi3Boolean* valuesOfContinuousStatesChanged,
    fmi3Boolean* nextEventTimeDefined,
    fmi3Float64* nextEventTime) {
    
	fmi3Status status = fmi3OK;

	return status;
}

fmi3Status fmi3DoStep(fmi3Instance instance,
	fmi3Float64 currentCommunicationPoint,
	fmi3Float64 communicationStepSize,
	fmi3Boolean noSetFMUStatePriorToCurrentPoint,
	fmi3Boolean* eventHandlingNeeded,
	fmi3Boolean* terminateSimulation,
	fmi3Boolean* earlyReturn,
	fmi3Float64* lastSuccessfulTime) {
	
	PlantInstance* comp = (PlantInstance*)instance;

	fmi3Status status = fmi3OK;

	update_derivative(comp);

	comp->data.x = comp->data.x + communicationStepSize * comp->data.der_x;

	*eventHandlingNeeded = fmi3False;
	*terminateSimulation = fmi3False;
	*earlyReturn = fmi3False;
	*lastSuccessfulTime = currentCommunicationPoint + communicationStepSize;

	return status;
}

fmi3Status fmi3EnterStepMode(fmi3Instance instance) { 
	return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {
	// Nothing to do.
	return fmi3OK;
}

void fmi3FreeInstance(fmi3Instance instance) {
	free(instance);
}
