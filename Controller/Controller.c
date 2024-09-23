#include "fmi3Functions.h"

#define MAX_MSG_SIZE 100

typedef enum {
	vr_r = 1,   // Clock
	vr_xr,      // Sample
	vr_ur,      // Discrete state/output
	vr_pre_ur,  // Previous ur
	vr_as,      // Local var
	vr_s        // Clock from supervisor
} ValueReference;

typedef enum {
	Instantiated = 1,
	EventMode,
	ContinuousTimeMode
} ModelState;

typedef struct {
	bool r;         // Clock
	double xr;      // Sample
	double ur;      // Discrete state/output
	double pre_ur;  // Previous ur
	double as;      // Local var
	bool s;         // Clock from supervisor
} ControllerData;

typedef struct {

	const char* instanceName;

	fmi3Boolean loggingOn;

	// callback functions
	fmi3LogMessageCallback logMessage;

	void* componentEnvironment;

	ControllerData data;

	ModelState state;

} ControllerInstance;

fmi3Instance fmi3InstantiateModelExchange(
	fmi3String                 instanceName,
	fmi3String                 instantiationToken,
	fmi3String                 resourcePath,
	fmi3Boolean                visible,
	fmi3Boolean                loggingOn,
	fmi3InstanceEnvironment    instanceEnvironment,
	fmi3LogMessageCallback     logMessage) {

	ControllerInstance* instance = (ControllerInstance*)calloc(1, sizeof(ControllerInstance));

	if (!instance) return NULL;

	instance->instanceName = instanceName;
	instance->loggingOn = loggingOn;
	instance->logMessage = logMessage;
	instance->componentEnvironment = instanceEnvironment;

	instance->data.r = false;       // Clock
	instance->data.xr = 0.0;                    // Sample
	instance->data.ur = 0.0;                    // Discrete state/output
	instance->data.pre_ur = 0.0;                // Previous ur
	instance->data.as = 1.0;                    // In var from Supervisor
	instance->data.s = false;       // Clock from Supervisor
	
	instance->state = Instantiated;

	return (fmi3Instance)instance;
}

fmi3Status fmi3EnterInitializationMode(fmi3Instance instance,
	fmi3Boolean toleranceDefined,
	fmi3Float64 tolerance,
	fmi3Float64 startTime,
	fmi3Boolean stopTimeDefined,
	fmi3Float64 stopTime) {
	return fmi3OK;
}

fmi3Status fmi3GetContinuousStates(fmi3Instance instance,
    fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
	char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %d.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}
}

fmi3Status fmi3ExitInitializationMode(fmi3Instance instance) {
	return fmi3OK;
}

fmi3Status fmi3EnterContinuousTimeMode(fmi3Instance instance) {
	ControllerInstance* comp = (ControllerInstance*)instance;
	comp->state = ContinuousTimeMode;
	return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {

	// TODO: implement
	return fmi3OK;
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

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nEventIndicators == 0) return status;

	if (nEventIndicators != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nEventIndicators: %d.", nEventIndicators);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	return status;
}


fmi3Status fmi3GetNumberOfContinuousStates(fmi3Instance instance,
    size_t* nContinuousStates) {
    *nContinuousStates = 0;
    return fmi3OK;
}

fmi3Status fmi3GetContinuousStateDerivatives(fmi3Instance instance,
    fmi3Float64 derivatives[],
    size_t nContinuousStates) {

    char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %d.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}
	
	return status;
}

fmi3Status fmi3GetIntervalDecimal(fmi3Instance instance,
    const fmi3ValueReference valueReferences[],
    size_t nValueReferences,
    fmi3Float64 intervals[],
    fmi3IntervalQualifier qualifiers[]) {

	char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
			case vr_r:
				intervals[i] = 0.1;
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

fmi3Status fmi3EnterEventMode(fmi3Instance instance) {
	ControllerInstance* comp = (ControllerInstance*)instance;
	comp->state = EventMode;
	return fmi3OK;
}

fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time) {
	ControllerInstance* comp = (ControllerInstance*)instance;
	return fmi3OK;
}

fmi3Status fmi3SetContinuousStates(fmi3Instance instance,
    const fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
    char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %d.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	return status;
}


fmi3Status fmi3GetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_xr:
			values[i] = comp->data.xr;
			s = fmi3OK;
			break;
		case vr_ur:
			if (comp->data.r == true) {
				values[i] = comp->data.ur + comp->data.as;
			}
			else {
				values[i] = comp->data.ur;
			}
			s = fmi3OK;
			break;
		case vr_pre_ur:
			values[i] = comp->data.pre_ur;
			s = fmi3OK;
			break;
		case vr_as:
			values[i] = comp->data.as;
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
	if (i != nValues) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Expected nValues = %zu but was %zu.", i, nValues);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		return fmi3Error;
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

fmi3Status fmi3SetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	const fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	ControllerInstance* comp = (ControllerInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_xr:
			comp->data.xr = values[i];
			s = fmi3OK;
			break;
		case vr_as:
			comp->data.as = values[i];
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
	if (i != nValues) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Expected nValues = %zu but was %zu.", i, nValues);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		return fmi3Error;
	}
	return status;
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

	// TODO: implement
	return fmi3OK;
}
