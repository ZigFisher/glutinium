#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "sample_comm.h"


void SAMPLE_VENC_Usage(char *sPrgNm) {
    printf("Usage : %s <soc_type> <sensor_type>\n", sPrgNm);
    printf("soc_types:\n");
    printf("\tHI3516EV200\n");
    printf("\tHI3518EV200\n");
    printf("\tHI3518EV201\n");
    printf("sensor_types:\n");
    printf("\tAPTINA_AR0130_DC_720P_30FPS\n");
    printf("\tAPTINA_AR0130_DC_960P_30FPS\n");
    printf("\tAPTINA_9M034_DC_720P_30FPS\n");
    printf("\tAPTINA_9M034_DC_960P_30FPS\n");
    printf("\tSONY_IMX222_DC_1080P_30FPS\n");
    printf("\tSONY_IMX222_DC_720P_30FPS\n");
    printf("\tAPTINA_AR0230_HISPI_1080P_30FPS\n");
    printf("\tPANASONIC_MN34222_MIPI_1080P_30FPS\n");
    printf("\tOMNIVISION_OV9712_DC_720P_30FPS\n");
    printf("\tOMNIVISION_OV9732_DC_720P_30FPS\n");
    printf("\tOMNIVISION_OV9750_MIPI_720P_30FPS\n");
    printf("\tOMNIVISION_OV9752_MIPI_720P_30FPS\n");
    printf("\tOMNIVISION_OV2718_MIPI_1080P_25FPS\n");
}

HI_S32 initSoc(int argc, char *argv[], SOC_TYPE *socType_ret) {
    if (argc < 3) { SAMPLE_VENC_Usage(argv[0]); return HI_FAILURE; }
    const char *socTypeStr = argv[1];
    SOC_TYPE socType;
    if (strcmp(socTypeStr, "HI3516EV200") == 0) socType = hi3516cv200;
    else if (strcmp(socTypeStr, "HI3518EV200") == 0) socType = hi3518ev200;
    else if (strcmp(socTypeStr, "HI3518EV201") == 0) socType = hi3518ev201;
    else {
        printf("I don't know sensor type '%s'", socTypeStr);
        SAMPLE_VENC_Usage(argv[0]);
        return HI_FAILURE;
    }
    *socType_ret = socType;
    return EXIT_SUCCESS;
}

HI_S32 initSensor(int argc, char *argv[], SAMPLE_VI_MODE_E *sensorType_ret) {
    if (argc < 2) { SAMPLE_VENC_Usage(argv[0]); return HI_FAILURE; }
    const char *sensorTypeStr = argv[2];
    SAMPLE_VI_MODE_E sensorType;
    if (strcmp(sensorTypeStr, "APTINA_9M034_DC_720P_30FPS") == 0) sensorType = APTINA_9M034_DC_720P_30FPS;
    else if (strcmp(sensorTypeStr, "SONY_IMX222_DC_1080P_30FPS") == 0) sensorType = SONY_IMX222_DC_1080P_30FPS;
    else if (strcmp(sensorTypeStr, "SONY_IMX222_DC_720P_30FPS") == 0) sensorType = SONY_IMX222_DC_720P_30FPS;
    else if (strcmp(sensorTypeStr, "APTINA_AR0130_DC_720P_30FPS") == 0) sensorType = APTINA_AR0130_DC_720P_30FPS;
    else if (strcmp(sensorTypeStr, "PANASONIC_MN34222_MIPI_1080P_30FPS") == 0) sensorType = PANASONIC_MN34222_MIPI_1080P_30FPS;
    else if (strcmp(sensorTypeStr, "APTINA_AR0230_HISPI_1080P_30FPS") == 0) sensorType = APTINA_AR0230_HISPI_1080P_30FPS;
    else if (strcmp(sensorTypeStr, "OMNIVISION_OV9712_DC_720P_30FPS") == 0) sensorType = OMNIVISION_OV9712_DC_720P_30FPS;
    else if (strcmp(sensorTypeStr, "OMNIVISION_OV9732_DC_720P_30FPS") == 0) sensorType = OMNIVISION_OV9732_DC_720P_30FPS;
    else if (strcmp(sensorTypeStr, "OMNIVISION_OV9750_MIPI_720P_30FPS") == 0) sensorType = OMNIVISION_OV9750_MIPI_720P_30FPS;
    else if (strcmp(sensorTypeStr, "OMNIVISION_OV9752_MIPI_720P_30FPS") == 0) sensorType = OMNIVISION_OV9752_MIPI_720P_30FPS;
    else if (strcmp(sensorTypeStr, "OMNIVISION_OV2718_MIPI_1080P_25FPS") == 0) sensorType = OMNIVISION_OV2718_MIPI_1080P_25FPS;
    else {
        printf("I don't know sensor type '%s'", sensorTypeStr);
        SAMPLE_VENC_Usage(argv[0]);
        return HI_FAILURE;
    }

    char *libsns = "";
    switch (sensorType) {
        case APTINA_AR0130_DC_720P_30FPS: { libsns = "ar0130"; break; }
        case APTINA_9M034_DC_720P_30FPS: { libsns = "9m034"; break; }
        case APTINA_AR0230_HISPI_1080P_30FPS: { libsns = "ar0230"; break; }
        case SONY_IMX222_DC_1080P_30FPS: { libsns = "imx222"; break; }
        case SONY_IMX222_DC_720P_30FPS: { libsns = "imx222"; break; }
        case PANASONIC_MN34222_MIPI_1080P_30FPS: { libsns = "mn34222"; break; }
        case OMNIVISION_OV9712_DC_720P_30FPS: { libsns = "ov9712"; break; }
        case OMNIVISION_OV9732_DC_720P_30FPS: { libsns = "ov9732"; break; }
        case OMNIVISION_OV9750_MIPI_720P_30FPS: { libsns = "ov9750"; break; }
        case OMNIVISION_OV9752_MIPI_720P_30FPS: { libsns = "ov9752"; break; }
        case OMNIVISION_OV2718_MIPI_1080P_25FPS: { libsns = "ov2718"; break; }
        default:
            printf("not support this sensor\n");
            return HI_FAILURE;
    }
    *sensorType_ret = sensorType;
    char libsns_so[250];
    int n = sprintf(libsns_so, "libsns_%s.so", libsns);
    return LoadSensorLibrary(libsns_so);
}


/******************************************************************************
* function : to process abnormal case
******************************************************************************/
void SAMPLE_VENC_HandleSig(HI_S32 signo) {
    if (SIGINT == signo || SIGTERM == signo) {
        SAMPLE_COMM_ISP_Stop();
        SAMPLE_COMM_SYS_Exit();
        UnloadSensorLibrary();
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    }
    exit(-1);
}