#include "fmi3Functions.h"

#define MAX_MSG_SIZE 100

typedef enum {
	Instantiated = 1,
	EventMode,
	ContinuousTimeMode
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
	bool clock_s_ticking; // Keeps track of whether clock s is ticking during event mode.
	double z; // Event indicator
	double pz; // Previous Event Indicator
} SupervisorData;

typedef struct {

	const char* instanceName;

	fmi3Boolean loggingOn;

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

	SupervisorInstance* instance = (SupervisorInstance*)calloc(1, sizeof(SupervisorInstance));

	if (!instance) return NULL;

	instance->instanceName = instanceName;
	instance->loggingOn = loggingOn;
	instance->logMessage = logMessage;
	instance->componentEnvironment = instanceEnvironment;

	instance->state = Instantiated;

	instance->data.s = false;       // Clock
	instance->data.x = 0.0;         // Sample
	instance->data.as = 1.0;        // Discrete state/output
	instance->data.as_previous = 1.0;
	instance->data.clock_s_ticking = false; // State Event
	instance->data.z = 0.0;
	instance->data.pz = 0.0;
	// The following is suggested by Masoud.
	instance->data.pz = 2.0 - instance->data.x;

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

	char msg_buff[MAX_MSG_SIZE];

	SupervisorInstance* comp = (SupervisorInstance*)instance;

	fmi3Status status = fmi3OK;

	if (nValueReferences == 0) return (fmi3Status)status;

	fmi3Status s;

	size_t i;

	for (i = 0; i < nValueReferences; i++) {
		fmi3Status s;
		ValueReference vr = valueReferences[i];
		switch (vr) {
		case vr_as:
			if (comp->state == EventMode && comp->data.clock_s_ticking) {
				// We need this here because when clock s is ticking, we need to output the next state already
				//   (because the clocked partition depends on that value),
				//   without executing the state transition.
				// Therefore we compute the next state "comp->data.as_previous * -1.0" and output it.
				// The actual execution of the state transition happens in the eventUpdate function below.
				// See definition of clocked partition in 2.2.8.3. Model Partitions and Clocked Variables.
				values[i] = comp->data.as_previous * -1.0;
			}
			else {
				values[i] = comp->data.as_previous;
			}
			s = fmi3OK;
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

	// TODO: implement
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

	// TODO: implement
	return fmi3OK;
}
