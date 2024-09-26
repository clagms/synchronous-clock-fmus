#pragma once

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


// Plantmodel vrefs
#define Plantmodel_U_ref  3
#define Plantmodel_X_ref  1

// Controller vrefs
#define Controller_UR_ref 3
#define Controller_X_ref 2
#define Controller_R_ref  1
#define Controller_S_ref  6
#define Controller_AS_ref 5

// Supervisor vrefs
#define Supervisor_S_ref  1
#define Supervisor_X_ref  2
#define Supervisor_AS_ref 3

// instance IDs
#define PLANTMODEL_ID 0
#define CONTROLLER_ID 1
#define SUPERVISOR_ID 2

#define N_INSTANCES 3

#define FIXED_STEP 1e-2
#define STOP_TIME 10.0

#define MAXDIRLENGTH 250


// Instance names
const char* names[N_INSTANCES] = { "plantmodel", "controller", "supervisor" };

// Constants with value references
const fmi3ValueReference plantmodel_u_refs[]  = { Plantmodel_U_ref  };
const fmi3ValueReference plantmodel_y_refs[]  = { Plantmodel_X_ref  };
const fmi3ValueReference controller_u_refs[]  = { Controller_X_ref };
const fmi3ValueReference controller_y_refs[]  = { Controller_UR_ref };
const fmi3ValueReference controller_r_refs[]  = { Controller_R_ref  };
const fmi3ValueReference controller_s_refs[]  = { Controller_S_ref  };
const fmi3ValueReference controller_as_refs[] = { Controller_AS_ref };
const fmi3ValueReference supervisor_s_refs[]  = { Supervisor_S_ref  };
const fmi3ValueReference supervisor_as_refs[] = { Supervisor_AS_ref };
const fmi3ValueReference supervisor_in_refs[] = { Supervisor_X_ref  };

// Simulation constants
const fmi3Float64 tEnd = STOP_TIME;
const fmi3Float64 tStart = 0;


static void logMessage(FMIInstance* instance, FMIStatus status, const char* category, const char* message) {

    switch (status) {
    case FMIOK:
        printf("[OK] ");
        break;
    case FMIWarning:
        printf("[Warning] ");
        break;
    case FMIDiscard:
        printf("[Discard] ");
        break;
    case FMIError:
        printf("[Error] ");
        break;
    case FMIFatal:
        printf("[Fatal] ");
        break;
    case FMIPending:
        printf("[Pending] ");
        break;
    }

    printf("[%s] %s\n", instance->name, message);
}


static void logFunction(FMIInstance* instance, FMIStatus status, const char* msg_format, ...) {

    va_list args;
    va_start(args, msg_format);

    switch (status) {
    case FMIOK:
        printf("[OK] ");
        break;
    case FMIWarning:
        printf("[Warning] ");
        break;
    case FMIDiscard:
        printf("[Discard] ");
        break;
    case FMIError:
        printf("[Error] ");
        break;
    case FMIFatal:
        printf("[Fatal] ");
        break;
    case FMIPending:
        printf("[Pending] ");
        break;
    }

    printf("[%s] ", instance->name);

    vprintf(msg_format, args);

    printf("\n");

    va_end(args);
}


//**************** Output aux functions ******************//

#define OUTPUT_FILE_HEADER "time,x,r,x_r,u_r,a_s\n"

#define CALL(f) do { status = f; if (status > FMIOK) goto TERMINATE; } while (0)

static FMIStatus recordVariables(FILE *outputFile, FMIInstance* controller, FMIInstance* plant, fmi3Float64 time) {

    FMIStatus status = FMIOK;

    const fmi3ValueReference plantmodel_vref[] = {Plantmodel_X_ref};
    fmi3Float64 plantmodel_vals[] = {0};
    CALL(FMI3GetFloat64(plant, plantmodel_vref, 1, plantmodel_vals, 1));

    const fmi3ValueReference controller_vref[] = {Controller_X_ref, Controller_UR_ref , Controller_AS_ref };
    fmi3Float64 controller_vals[] = {0.0, 0.0 , 0.0 };
    CALL(FMI3GetFloat64(controller, controller_vref, 3, controller_vals, 3));

    //                                         time,     x,              r,       x_r,                u_r                a_s
    fprintf(outputFile, "%g,%g,%d,%g,%g,%g\n", time, plantmodel_vals[0], 0, controller_vals[0], controller_vals[1], controller_vals[2]);

TERMINATE:
    return status;
}

FILE* initializeFile(char* fname) {
    FILE* outputFile = fopen(fname, "w");
    if (!outputFile) {
        puts("Failed to open output file.");
        return NULL;
    }
    fputs(OUTPUT_FILE_HEADER, outputFile);
    return outputFile;
}

//*******************************************************//

#if defined(_WIN32)
#define BINARY_DIR "\\binaries\\x86_64-windows\\"
#define BINARY_EXT ".dll"
#elif defined(__APPLE__)
#define BINARY_DIR "/binaries/x86_64-darwin/"
#define BINARY_EXT ".dylib"
#else
#define BINARY_DIR "/binaries/x86_64-linux/"
#define BINARY_EXT ".so"
#endif


static FMIStatus handleTimeEventController(FMIInstance* controller, FMIInstance* plant) {

    FMIStatus status = FMIOK;

    // Activate Controller's clock r
    fmi3Clock controller_r_vals[] = { fmi3ClockActive };
    CALL(FMI3SetClock(controller, controller_r_refs, 1, controller_r_vals));

    // Inputs to the clocked partition are assumed to have been set already, in the continuous time

    // Compute outputs to clocked partition: Exchange data Controller -> Plantmodel
    fmi3Float64 controller_vals[] = { 0.0 };
    CALL(FMI3GetFloat64(controller, controller_y_refs, 1, controller_vals, 1));
    CALL(FMI3SetFloat64(plant, plantmodel_u_refs, 1, controller_vals, 1));

TERMINATE:
    return status;
}

static FMIStatus handleStateEventSupervisor(FMIInstance* controller, FMIInstance* supervisor) {

    FMIStatus status = FMIOK;

    fmi3Clock supervisor_s_vals[] = { fmi3ClockInactive };
    fmi3Float64 supervisor_as_vals[] = { 0.0 };

    // Propagate clock activation Supervisor -> Controller
    CALL(FMI3GetClock(supervisor, supervisor_s_refs, 1, supervisor_s_vals));
    assert(supervisor_s_vals[0] == fmi3ClockActive);
    CALL(FMI3SetClock(controller, controller_s_refs, 1, supervisor_s_vals));

    // Exchange data Supervisor -> Controller
    CALL(FMI3GetFloat64(supervisor, supervisor_as_refs, 1, supervisor_as_vals, 1));
    CALL(FMI3SetFloat64(controller, controller_as_refs, 1, supervisor_as_vals, 1));

TERMINATE:
    return status;
}

#ifdef __cplusplus
}  /* end of extern "C" { */
#endif
