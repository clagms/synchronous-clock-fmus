#include "fmi3Functions.h"

#define MAX_MSG_SIZE 100

typedef enum {
	Instantiated = 1,
	EventMode,
	ContinuousTimeMode,
	Terminated
} ModelState;

typedef enum {
	vr_s = 1,       // Clock s
	vr_x,           // Sample from Plant
	vr_as,          // Output that is fed to the Controller
	vr_as_previous  // Previous value for as
} ValueReference;

typedef struct {
	bool s;    // Clock
	double x;  // Sample
	double as; // Output and ClockedState that is fed to the Controller
	double as_previous; // Previous value for as
	double z; // Event indicator
	double pz; // Previous Event Indicator
} SupervisorData;

typedef struct {

	const char* instanceName;

	// callback functions
	fmi3LogMessageCallback logMessage;

	void* componentEnvironment;

	SupervisorData data;

	ModelState state;
} SupervisorInstance;

fmi3Instance fmi3InstantiateModelExchange(
	fmi3String                 instanceName,
	fmi3String                 instantiationToken,
	fmi3String                 resourcePath,
	fmi3Boolean                visible,
	fmi3Boolean                loggingOn,
	fmi3InstanceEnvironment    instanceEnvironment,
	fmi3LogMessageCallback     logMessage) {

	SupervisorInstance* comp = (SupervisorInstance*)calloc(1, sizeof(SupervisorInstance));

	if (!comp) return NULL;

	comp->instanceName = instanceName;
	comp->logMessage = logMessage;
	comp->componentEnvironment = instanceEnvironment;

	fmi3Reset((fmi3Instance)comp);

	return (fmi3Instance)comp;
}

fmi3Status fmi3Reset(fmi3Instance instance) {
	fmi3Status status = fmi3OK;
	SupervisorInstance* comp = (SupervisorInstance*)instance;
	
	comp->state = Instantiated;

	comp->data.s = false;       // Clock
	comp->data.x = 0.0;         // Sample
	comp->data.as = 1.0;        // Discrete state/output
	comp->data.as_previous = 1.0;
	comp->data.z = 0.0;
	comp->data.pz = 0.0;
	// The following is suggested by Masoud to avoid an initial detection of the event.
	comp->data.pz = 2.0 - comp->data.x;

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

fmi3Status fmi3GetNumberOfEventIndicators(fmi3Instance instance,
    size_t* nEventIndicators) {
	*nEventIndicators = 1;
    return fmi3OK;
}

fmi3Status fmi3GetEventIndicators(fmi3Instance instance,
    fmi3Float64 eventIndicators[],
    size_t nEventIndicators) {
    
	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nEventIndicators == 0) return status;

	comp->data.pz = comp->data.z;
	comp->data.z = 2.0 - comp->data.x;

	size_t i;
	for (i = 0; i < nEventIndicators; i++) {
		fmi3Status s;
		eventIndicators[i] = comp->data.z;
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

	SupervisorInstance* comp = (SupervisorInstance*)instance;

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

	fmi3Status status = fmi3Error;

	SupervisorInstance* comp = (SupervisorInstance*)instance;

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

	SupervisorInstance* comp = (SupervisorInstance*)instance;

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

	SupervisorInstance* comp = (SupervisorInstance*)instance;

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

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	snprintf(msg_buff, MAX_MSG_SIZE, "Function not relevant for this fmu.");
	comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);

	return status;
}

fmi3Status fmi3GetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	fmi3Status s;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_as:
			if (comp->state == EventMode && comp->data.s) {
				// We need this here because when clock s is ticking, we need to output the next state already
				//   (because the clocked partition depends on that value),
				//   without executing the state transition.
				// Therefore we compute the next state "comp->data.as * -1.0" and output it.
				// The actual execution of the state transition happens in the eventUpdate function below.
				// See definition of clocked partition in 2.2.8.3. Model Partitions and Clocked Variables.
				values[i] = comp->data.as * -1.0;
				s = fmi3OK;
			}
			else {
				snprintf(msg_buff, MAX_MSG_SIZE, "Value of clock s being observed outside of event mode.");
				comp->logMessage(comp->componentEnvironment, status, "Warning", msg_buff);
				s = fmi3Warning;
				values[i] = comp->data.as_previous;
			}
			break;
		case vr_x:
			values[i] = comp->data.x;
			s = fmi3OK;
			break;
		case vr_as_previous:
			values[i] = comp->data.as_previous;
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

fmi3Status fmi3GetContinuousStates(fmi3Instance instance,
    fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %d.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	return status;
}

fmi3Status fmi3GetClock(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	fmi3Clock values[]) {

	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	size_t i;
	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_s:
			values[i] = comp->data.s;
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

fmi3Status fmi3SetTime(fmi3Instance instance, fmi3Float64 time) {
	SupervisorInstance* comp = (SupervisorInstance*)instance;
	return fmi3OK;
}

fmi3Status fmi3SetContinuousStates(fmi3Instance instance,
    const fmi3Float64 continuousStates[],
    size_t nContinuousStates) {
	
    char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nContinuousStates == 0) return status;

	if (nContinuousStates != 0) {
		snprintf(msg_buff, MAX_MSG_SIZE, "Unexpected nContinuousStates: %d.", nContinuousStates);
		comp->logMessage(comp->componentEnvironment, status, "Error", msg_buff);
		status = fmi3Error;
	}

	return status;
}

fmi3Status fmi3SetFloat64(fmi3Instance instance,
	const fmi3ValueReference valueReferences[],
	size_t nValueReferences,
	const fmi3Float64 values[],
	size_t nValues) {

	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return status;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_x:
			comp->data.x = values[i];
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
	
	return fmi3OK;
}

fmi3Status fmi3EnterEventMode(fmi3Instance instance) {
	SupervisorInstance* comp = (SupervisorInstance*)instance;
	comp->state = EventMode;

	if (comp->data.pz * comp->data.z < 0.0) {
		// Clock s is ticking
		comp->data.s = true;
	}

	return fmi3OK;
}

fmi3Status fmi3UpdateDiscreteStates(fmi3Instance instance,
    fmi3Boolean* discreteStatesNeedUpdate,
    fmi3Boolean* terminateSimulation,
    fmi3Boolean* nominalsOfContinuousStatesChanged,
    fmi3Boolean* valuesOfContinuousStatesChanged,
    fmi3Boolean* nextEventTimeDefined,
    fmi3Float64* nextEventTime) {
    
	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (comp->data.s == true) {
		comp->data.as_previous = comp->data.as;
		comp->data.as = comp->data.as_previous * -1.0;
		comp->data.s = false;
	}
	
	*discreteStatesNeedUpdate = fmi3False;
	*terminateSimulation = fmi3False;
	*nominalsOfContinuousStatesChanged = fmi3False;
	*valuesOfContinuousStatesChanged = fmi3False;
	*nextEventTimeDefined = fmi3False;
	*nextEventTime = 0.0;

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
	return fmi3OK;
}

fmi3Status fmi3Terminate(fmi3Instance instance) {
	SupervisorInstance* comp = (SupervisorInstance*)instance;
	comp->state = Terminated;
	return fmi3OK;
}

void fmi3FreeInstance(fmi3Instance instance) {
	free(instance);
}