#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hi_comm_sns.h"
#include "hi_ae_comm.h"
#include "hi_awb_comm.h"
#include "iniparser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//#define INIFILE_CONFIG_MODE
#ifdef INIFILE_CONFIG_MODE
 
#define ModeNumMax 6
AE_SENSOR_DEFAULT_S  g_AeDft[ModeNumMax];
AWB_SENSOR_DEFAULT_S g_AwbDft[ModeNumMax];
ISP_CMOS_DEFAULT_S   g_IspDft[ModeNumMax];

dictionary* g_dictionary = NULL;
static int MAEWeight[256];
#define ISO_EXPOSURE_LEVEL 16


static int  Weight(const char *pcStr)
{
    const char*  pszVRBegin  = pcStr;  
    const char*  pszVREnd    = pszVRBegin; 
    int      Count = 0;
    char     Temp[20];
    int      Mycount = 0;
    int      Length = strlen(pcStr);
    unsigned int re;
    int i = 0;
    
    memset(Temp, 0, 20);
    
    while((pszVREnd != NULL))
    {
        if((Mycount > Length)||(Mycount == Length))
            {
                break;
            }
        while((*pszVREnd != '|')&&(*pszVREnd != '\0')&&(*pszVREnd != ','))
        {
            pszVREnd++;
            Count++;
            Mycount++;
        }
        memcpy(Temp, pszVRBegin, Count);                  

        re = (int)strtol(Temp, NULL, 0);
        MAEWeight[i] = re;     

        memset(Temp, 0, 20);
        Count = 0;
        pszVREnd++;
        pszVRBegin = pszVREnd;
        Mycount++;
        i++;                          
    }
    
    return i;
}


HI_S32 Cmos_cfg_LoadFile(const HI_CHAR *pcName)
{
    if(NULL != g_dictionary)
    {
        g_dictionary = NULL;
    }
    else
    {
        g_dictionary = iniparser_load(pcName);
        if(NULL == g_dictionary)
        {
            printf("%s ini load failed\n",pcName);
            return HI_FAILURE;
        }   
    }
    return HI_SUCCESS;
}

HI_VOID Cmos_cfg_FreeDict()
{
    if(NULL != g_dictionary)
    {
        iniparser_freedict(g_dictionary);
    }
    g_dictionary = NULL;
}

HI_S32 cmos_loadAE_cfg()
{
    HI_U8 i;
    HI_S32 s32Temp = 0;
    HI_S32 AEModeNumber = 0;
    HI_CHAR acTempStr[128];

    /********************AE:AEModeNumber*******************/
    AEModeNumber = iniparser_getint(g_dictionary, "AE:AEModeNumber", HI_FAILURE);
    if (HI_FAILURE == AEModeNumber)
    {
        printf("AE:AEModeNumber failed\n");
        return HI_FAILURE;
    }
    
    if (AEModeNumber > ModeNumMax)
    {
        printf("notice : Current AEModeNumber is larger!!!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < AEModeNumber; i++)
    {
        /*AeCompensation*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:AeCompensation_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:AeCompensation_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u8AeCompensation = s32Temp;

        /*MaxIntTimeTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MaxIntTimeTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MaxIntTimeTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MaxIntTimeTarget = s32Temp; 

        /*MinIntTimeTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MinIntTimeTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MinIntTimeTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MinIntTimeTarget = s32Temp;  

        /*MaxAgainTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MaxAgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MaxAgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MaxAgainTarget = s32Temp; 

        /*MinAgainTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MinAgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MinAgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MinAgainTarget = s32Temp;

        /*MaxDgainTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MaxDgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MaxDgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MaxDgainTarget = s32Temp; 

        /*MinDgainTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MinDgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MinDgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MinDgainTarget = s32Temp;

        /*ISPDgainShift*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:ISPDgainShift_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:ISPDgainShift_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32ISPDgainShift = s32Temp; 

        /*MaxISPDgainTarget*/
        snprintf(acTempStr, sizeof(acTempStr), "AE:MaxISPDgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MaxISPDgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MaxISPDgainTarget = s32Temp; 

        /*MinISPDgainTarget*/    
        snprintf(acTempStr, sizeof(acTempStr), "AE:MinISPDgainTarget_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AE:MinISPDgainTarget_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AeDft[i].u32MinISPDgainTarget = s32Temp;         
            
    }

    return HI_SUCCESS;
}


HI_S32 cmos_loadAWB_cfg()
{
    HI_U8 i,j;
    HI_S32 s32Temp = 0;
    HI_S32 AWBModeNumber = 0;
    HI_CHAR *pcTempStr = NULL;
    HI_CHAR acTempStr[128];
    HI_S32  FindKey = 0;

    /********************AWB:AWBModeNumber*******************/
    AWBModeNumber = iniparser_getint(g_dictionary, "AWB:AWBModeNumber", HI_FAILURE);
    if (HI_FAILURE == AWBModeNumber)
    {
        printf("AWB:AWBModeNumber failed\n");
        return HI_FAILURE;
    }
    
    if (AWBModeNumber > ModeNumMax)
    {
        printf("notice : Current AWBModeNumber is larger!!!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < AWBModeNumber; i++)
    {
        /* HighColorTemp */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:HighColorTemp_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AWB:HighColorTemp_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AwbDft[i].stCcm.u16HighColorTemp = s32Temp;
        
        /* HighCCM */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:HighCCM_%d", i);
        pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
        if(NULL == pcTempStr)
        {
            printf("AWB:HighCCM_%d error\n", i);
            return HI_FAILURE;
        }

        s32Temp = Weight(pcTempStr);
        for (j = 0; j < 9; j++)
        {
            g_AwbDft[i].stCcm.au16HighCCM[j] = MAEWeight[j];
        } 

        /* MidColorTemp */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:MidColorTemp_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AWB:MidColorTemp_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AwbDft[i].stCcm.u16MidColorTemp = s32Temp;
        
        /* MidCCM */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:MidCCM_%d", i);
        pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
        if(NULL == pcTempStr)
        {
            printf("AWB:MidCCM_%d error\n", i);
            return HI_FAILURE;
        }

        s32Temp = Weight(pcTempStr);
        for (j = 0; j < 9; j++)
        {
            g_AwbDft[i].stCcm.au16MidCCM[j] = MAEWeight[j];
        } 

        /* LowColorTemp */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:LowColorTemp_%d", i);        
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AWB:LowColorTemp_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AwbDft[i].stCcm.u16LowColorTemp = s32Temp;
        
        /* LowCCM */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:LowCCM_%d", i);
        pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
        if(NULL == pcTempStr)
        {
            printf("AWB:LowCCM_%d error\n", i);
            return HI_FAILURE;
        }

        s32Temp = Weight(pcTempStr);
        for (j = 0; j < 9; j++)
        {
            g_AwbDft[i].stCcm.au16LowCCM[j] = MAEWeight[j];
        } 

        /* WbRefTemp */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:WbRefTemp_%d", i);   
        s32Temp = 0;
        s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
        if (HI_FAILURE == s32Temp)
        {
            printf("AWB:WbRefTemp_%d failed\n", i);
            return HI_FAILURE;
        }
        g_AwbDft[i].u16WbRefTemp = s32Temp;

        /* GainOffset */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:GainOffset_%d", i);  
        pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
        if(NULL == pcTempStr)
        {
            printf("AWB:GainOffset_%d error\n", i);
            return HI_FAILURE;
        }
        
        s32Temp = Weight(pcTempStr);
        for (j = 0; j < 4; j++)
        {
            g_AwbDft[i].au16GainOffset[j] = MAEWeight[j];
        } 

        /* WbPara */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:WbPara_%d", i);        
        pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
        if(NULL == pcTempStr)
        {
            printf("AWB:WbPara_%d error\n", i);
            return HI_FAILURE;
        }
        
        s32Temp = Weight(pcTempStr);
        for (j = 0; j < 6; j++)
        {
            g_AwbDft[i].as32WbPara[j] = MAEWeight[j];
        } 

        /* SatValid */
        snprintf(acTempStr, sizeof(acTempStr), "AWB:SatValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
               if (HI_FAILURE == s32Temp)
            {
                printf("AWB:SatValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_AwbDft[i].stAgcTbl.bValid = s32Temp;

            /* Saturation */
            snprintf(acTempStr, sizeof(acTempStr), "AWB:Saturation_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("AWB:Saturation_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0;j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_AwbDft[i].stAgcTbl.au8Saturation[j] = MAEWeight[j];
            } 

        }
    
    }

    return HI_SUCCESS;
}


HI_S32 cmos_loadISP_cfg()
{
    HI_U8 i,j;
    HI_S32 s32Temp = 0;
    HI_S32  s32Offset = 0;
    HI_S32 ISPModeNumber = 0;
    HI_CHAR *pcTempStr = NULL;
    HI_CHAR acTempStr[128];
    HI_S32  FindKey = 0;

    /********************ISP:ISPModeNumber*******************/
    ISPModeNumber = iniparser_getint(g_dictionary, "ISP:ISPModeNumber", HI_FAILURE);
    if (HI_FAILURE == ISPModeNumber)
    {
        printf("ISP:ISPModeNumber failed\n");
        return HI_FAILURE;
    }
    
    if (ISPModeNumber > ModeNumMax)
    {
        printf("notice : Current ISPModeNumber is larger!!!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < ISPModeNumber; i++)
    {
        /* AgcValid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:AgcValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
               if (HI_FAILURE == s32Temp)
            {
                printf("ISP:AgcValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stAgcTbl.bValid = s32Temp; 

            /* SharpenAltD */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SharpenAltD_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:SharpenAltD_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8SharpenAltD[j] = MAEWeight[j];
            } 

            /* SharpenAltUd */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SharpenAltUd_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:SharpenAltUd_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8SharpenAltUd[j] = MAEWeight[j];
            } 

            /* SnrThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SnrThresh_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:SnrThresh_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8SnrThresh[j] = MAEWeight[j];
            } 

            /* DemosaicLumThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:DemosaicLumThresh_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:DemosaicLumThresh_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8DemosaicLumThresh[j] = MAEWeight[j];
            }            

            /* DemosaicNpOffset */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:DemosaicNpOffset_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:DemosaicNpOffset_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8DemosaicNpOffset[j] = MAEWeight[j];
            } 

            /* GeStrength */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:GeStrength_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:GeStrength_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8GeStrength[j] = MAEWeight[j];
            } 

            /* SharpenRGB */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SharpenRGB_%d", i); 
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:SharpenRGB_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < ISO_EXPOSURE_LEVEL; j++)
            {
                g_IspDft[i].stAgcTbl.au8SharpenRGB[j] = MAEWeight[j];
            } 
        }

        /* WeightValid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:WeightValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:WeightValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stNoiseTbl.bValid = s32Temp;
            
            /* NoiseProfileWeight */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:NoiseProfileWeight_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:NoiseProfileWeight_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < 128; j++)
            {
                g_IspDft[i].stNoiseTbl.au8NoiseProfileWeightLut[j] = MAEWeight[j];
            } 
            
            /* DemosaicWeight */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:DemosaicWeight_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:DemosaicWeight_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < 128; j++)
            {
                g_IspDft[i].stNoiseTbl.au8DemosaicWeightLut[j] = MAEWeight[j];
            } 
        }


        /* demosaicValid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:demosaicValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:demosaicValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.bValid = s32Temp;

            /* VhSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VhSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VhSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8VhSlope = s32Temp;

            /* AaSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:AaSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:AaSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8AaSlope = s32Temp;

            /* VaSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VaSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VaSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8VaSlope = s32Temp;

            /* UuSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:UuSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:UuSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8UuSlope = s32Temp;

            /* SatSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SatSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:SatSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8SatSlope = s32Temp;            

            /* AcSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:AcSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:AcSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8AcSlope = s32Temp;  

             /* FcSlope */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:FcSlope_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:FcSlope_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u8FcSlope = s32Temp;         

             /* VhThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VhThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VhThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16VhThresh = s32Temp; 

             /* AaThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:AaThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:AaThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16AaThresh = s32Temp; 

             /* VaThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VaThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VaThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16VaThresh = s32Temp; 

             /* UuThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:UuThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:UuThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16UuThresh = s32Temp; 

             /* SatThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SatThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:SatThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16SatThresh = s32Temp;             

             /* AcThresh */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:AcThresh_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:AcThresh_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDemosaic.u16AcThresh = s32Temp;             

        }

        /* gammaRGBValid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:gammaRGBValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Offset = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:gammaRGBValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stGamma.bValid = s32Temp;

            /* gammaRGB0 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammaRGB0_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammaRGB0_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGamma.au16Gamma[j] = MAEWeight[j];
            }  
            s32Offset += s32Temp;

            /* gammaRGB1 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammaRGB1_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammaRGB1_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGamma.au16Gamma[s32Offset + j] = MAEWeight[j];
            }  
            s32Offset += s32Temp;

            /* gammaRGB2 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammaRGB2_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammaRGB2_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGamma.au16Gamma[s32Offset + j] = MAEWeight[j];
            }  
        }

        /* gammafevalid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:gammafevalid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Offset = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:gammafevalid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stGammafe.bValid = s32Temp;


            /* gammafe0 */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammafe0_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if (NULL == pcTempStr)
            {
                printf("ISP:gammafe0_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = 0;
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGammafe.au16Gammafe0[j] = MAEWeight[j];
            }

            /* gammafe1.0 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammafe1.0_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammafe1.0_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGammafe.au16Gammafe1[j] = MAEWeight[j];
            }  
            s32Offset += s32Temp;
        
            /* gammafe1.1 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammafe1.1_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammafe1.1_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGammafe.au16Gammafe1[s32Offset + j] = MAEWeight[j];
            }  
            s32Offset += s32Temp;
        
            /* gammafe1.2 */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:gammafe1.2_%d", i);
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:gammafe1.2_%d error\n", i);
                return HI_FAILURE;
            }
            
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < s32Temp; j++)
            {
                g_IspDft[i].stGammafe.au16Gammafe1[s32Offset + j] = MAEWeight[j];
            }  
        }

        /* RGBsharpenLutValid */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:RGBsharpenLutValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:RGBsharpenLutValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stRgbSharpen.bValid = s32Temp;
            
            /* Core */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:LutCore_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:LutCore_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stRgbSharpen.u8LutCore = s32Temp;

            /* LutStrength */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:LutStrength_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:LutStrength_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stRgbSharpen.u8LutStrength = s32Temp;

            /* LutMagnitude */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:LutMagnitude_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:LutMagnitude_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stRgbSharpen.u8LutMagnitude = s32Temp;

        }

        /* DrcEnable */
        snprintf(acTempStr, sizeof(acTempStr), "ISP:DrcEnable_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);       
        if(1 == FindKey)
        {      
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:DrcEnable_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.bEnable = s32Temp;
            
            /* BlackLevel */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:BlackLevel_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:BlackLevel_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32BlackLevel = s32Temp;

            /* WhiteLevel */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:WhiteLevel_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:WhiteLevel_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32WhiteLevel = s32Temp;

            /* SlopeMax */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SlopeMax_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:SlopeMax_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32SlopeMax = s32Temp;
            
            /* SlopeMin */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:SlopeMin_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:SlopeMin_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32SlopeMin = s32Temp;

            /* VarianceSpace */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VarianceSpace_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VarianceSpace_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32VarianceSpace = s32Temp;

            /* VarianceIntensity */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:VarianceIntensity_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:VarianceIntensity_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stDrc.u32VarianceIntensity = s32Temp;

        }

        /* ShadingValid */ 
        snprintf(acTempStr, sizeof(acTempStr), "ISP:ShadingValid_%d", i);
        FindKey = 0;
        FindKey = iniparser_find_entry(g_dictionary, acTempStr);    
        if(1 == FindKey)
        { 
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
               if (HI_FAILURE == s32Temp)
            {
                printf("ISP:ShadingValid_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.bValid = s32Temp;  

            /* RCenterX */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:RCenterX_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
               if (HI_FAILURE == s32Temp)
            {
                printf("ISP:RCenterX_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16RCenterX = s32Temp;

            /* RCenterY */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:RCenterY_%d", i);        
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
               if (HI_FAILURE == s32Temp)
            {
                printf("ISP:RCenterY_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16RCenterY = s32Temp;

            /* GCenterX */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:GCenterX_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:GCenterX_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16GCenterX = s32Temp;

            /* GCenterY */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:GCenterY_%d", i);        
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:GCenterY_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16GCenterY = s32Temp;

            /* BCenterX */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:BCenterX_%d", i); 
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:BCenterX_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16BCenterX = s32Temp;

            /* BCenterY */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:BCenterY_%d", i);        
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:BCenterY_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16BCenterY = s32Temp;

            /* TblNodeNum */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:TblNodeNum_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:TblNodeNum_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16TblNodeNum = s32Temp;

            /* RShadingTbl */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:RShadingTbl_%d", i);        
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:RShadingTbl_%d error\n", i);
                return HI_FAILURE;
            }
            s32Temp = 0;
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < g_IspDft[i].stShading.u16TblNodeNum; j++)
            {
                g_IspDft[i].stShading.au16RShadingTbl[j] = MAEWeight[j];
            } 

            /* GShadingTbl */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:GShadingTbl_%d", i); 
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:GShadingTbl_%d error\n", i);
                return HI_FAILURE;
            }
            s32Temp = 0;
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < g_IspDft[i].stShading.u16TblNodeNum; j++)
            {
                g_IspDft[i].stShading.au16GShadingTbl[j] = MAEWeight[j];
            } 

            /* BShadingTbl */
            snprintf(acTempStr, sizeof(acTempStr), "ISP:BShadingTbl_%d", i); 
            pcTempStr = iniparser_getstring(g_dictionary, acTempStr, "-1");
            if(NULL == pcTempStr)
            {
                printf("ISP:BShadingTbl_%d error\n", i);
                return HI_FAILURE;
            }
            s32Temp = 0;
            s32Temp = Weight(pcTempStr);
            for (j = 0; j < g_IspDft[i].stShading.u16TblNodeNum; j++)
            {
                g_IspDft[i].stShading.au16BShadingTbl[j] = MAEWeight[j];
            } 

            /* R\G\B offcenter */ 
            snprintf(acTempStr, sizeof(acTempStr), "ISP:ROffCenter_%d", i); 
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:ROffCenter_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16ROffCenter = s32Temp;

            snprintf(acTempStr, sizeof(acTempStr), "ISP:GOffCenter_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:GOffCenter_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16GOffCenter = s32Temp;

            snprintf(acTempStr, sizeof(acTempStr), "ISP:BOffCenter_%d", i);
            s32Temp = 0;
            s32Temp = iniparser_getint(g_dictionary, acTempStr, HI_FAILURE);
            if (HI_FAILURE == s32Temp)
            {
                printf("ISP:BOffCenter_%d failed\n", i);
                return HI_FAILURE;
            }
            g_IspDft[i].stShading.u16BOffCenter = s32Temp;

        }

    }

    return HI_SUCCESS;
}


HI_S32 Cmos_LoadINIPara(const HI_CHAR *pcName)
{
    HI_S32 s32Ret = HI_SUCCESS;   

    if(NULL == pcName)
    {
        printf("null pointer when get inifile path!!!\n");
        return HI_FAILURE;
    }

    s32Ret = Cmos_cfg_LoadFile(pcName);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s load ini filename failed\n", pcName);
        Cmos_cfg_FreeDict();
        return HI_FAILURE;
    }

    s32Ret = cmos_loadAE_cfg();
    if (HI_SUCCESS != s32Ret)
    {
        printf("cmos_loadAE_cfg failed\n");
        Cmos_cfg_FreeDict();
        return HI_FAILURE;
    }
    
    s32Ret = cmos_loadAWB_cfg();
    if (HI_SUCCESS != s32Ret)
    {
        printf("cmos_loadAWB_cfg failed\n");
        Cmos_cfg_FreeDict();
        return HI_FAILURE;
    }

     s32Ret = cmos_loadISP_cfg();
     if (HI_SUCCESS != s32Ret)
    {
        printf("cmos_loadISP_cfg failed\n");
        Cmos_cfg_FreeDict();
        return HI_FAILURE;
    }

    Cmos_cfg_FreeDict();

    return HI_SUCCESS;
}

#else

#endif


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

