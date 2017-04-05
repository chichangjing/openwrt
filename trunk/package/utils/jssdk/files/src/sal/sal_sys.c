
#include "sal_sys.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "joyware.h"


/*****************************************************************************************************************************
@@@
	The Device (GE-Z20208D_2008) front panel interface : (CPU port: hport9) 2光8电
     ----------------------------------------------------------------------------------------------------
    | Physical port:    |                          |  hport7     |  hport5     | hport3     |  hport1    |       
    | Logic port:       |                          |  lport4     |  lport6     | lport8     |  lport10   |	      
    | Panel interface:  |                          |  100M RJ45  |  100M RJ45  | 100M RJ45  |  100M RJ45 |	
     ----------------------------------------------------------------------------------------------------
    | Physical port:    |  hport10   |  hport8     |  hport6     |  hport4     | hport2     |  hport0    |       
    | Logic port:       |  lport1    |  lport2     |  lport3     |  lport5     | lport7     |  lport9    |	      
    | Panel interface:  |  1000M SFP |  1000M SFP  |  100M RJ45  |  100M RJ45  | 100M RJ45  |  100M RJ45 |
     ----------------------------------------------------------------------------------------------------

@@@
	The Device (GE-Z20505D_1414) front panel interface : (CPU port: hport9) 5光5电
     ----------------------------------------------------------------------------------------------------
    | Physical port:    |                          |  hport7     |  hport5     | hport3     |  hport1    |       
    | Logic port:       |                          |  lport4     |  lport6     | lport8     |  lport10   |	      
    | Panel interface:  |                          |  100M SFP   |  100M SFP   | 100M RJ45  |  100M RJ45 |	
     ----------------------------------------------------------------------------------------------------
    | Physical port:    |  hport10   |  hport8     |  hport6     |  hport4     | hport2     |  hport0    |       
    | Logic port:       |  lport1    |  lport2     |  lport3     |  lport5     | lport7     |  lport9    |	      
    | Panel interface:  |  1000M SFP |  1000M RJ45 |  100M SFP   |  100M SFP   | 100M RJ45  |  100M RJ45 |
     ----------------------------------------------------------------------------------------------------

@@@
	The Device (GE-Z20208D_1008) front panel interface : (CPU port: hport9) 1光8电
     --------------------------------------------------------------------------------------
    | Physical port:    |            |  hport7     |  hport5     | hport3     |  hport1    |       
    | Logic port:       |            |  lport3     |  lport5     | lport7     |  lport9   |	      
    | Panel interface:  |            |  100M RJ45  |  100M RJ45  | 100M RJ45  |  100M RJ45 |	
     --------------------------------------------------------------------------------------
    | Physical port:    |  hport10   |  hport6     |  hport4     | hport2     |  hport0    |       
    | Logic port:       |  lport1    |  lport2     |  lport4     | lport6     |  lport8    |	      
    | Panel interface:  |  1000M SFP |  100M RJ45  |  100M RJ45  | 100M RJ45  |  100M RJ45 |
     --------------------------------------------------------------------------------------

@@@
	The Device (GE-Z20208D_2004) front panel interface : (CPU port: hport9) 2光4电
     --------------------------------------------------------------------------
    | Physical port:    |                          |  hport7     |  hport5     |       
    | Logic port:       |                          |  lport4     |  lport6     |	      
    | Panel interface:  |                          |  100M RJ45  |  100M RJ45  |	
     --------------------------------------------------------------------------
    | Physical port:    |  hport10   |  hport8     |  hport6     |  hport4     |      
    | Logic port:       |  lport1    |  lport2     |  lport3     |  lport5     |      
    | Panel interface:  |  1000M SFP |  1000M SFP  |  100M RJ45  |  100M RJ45  |
     --------------------------------------------------------------------------

@@@
	The Device (GE-Z20208D_1004) front panel interface : (CPU port: hport9) 1光4电
     ------------------------------------------------------------
    | Physical port:    |            |  hport7     |  hport5     |     
    | Logic port:       |            |  lport3     |  lport4     |      
    | Panel interface:  |            |  100M RJ45  |  100M RJ45  |	
     ------------------------------------------------------------
    | Physical port:    |  hport10   |  hport6     |  hport4     |      
    | Logic port:       |  lport1    |  lport2     |  lport4     |	      
    | Panel interface:  |  1000M SFP |  100M RJ45  |  100M RJ45  |
     ------------------------------------------------------------

@@@
	The Device (GE-Z20208D_2004) front panel interface : (CPU port: hport9) 2光4电 
	----------------------------------------------------------------------------------------------------
	| Physical port:	|  hport10   |  hport8 		|  hport9	  |  hport7 	| hport6	 |	hport5	  | 	  
	| Logic port:		|  lport1	 |  lport2 		|  lport3	  |  lport4 	| lport5	 |	lport6	  | 	   
	| Panel interface:  |  1000M SFP |  1000M SFP	|  100M RJ45  |  100M RJ45	| 100M RJ45  |	100M RJ45 |
	----------------------------------------------------------------------------------------------------

     
*****************************************************************************************************************************/


static sal_sys_model_t sal_sys_model[] = {
	{GE_Z20208D_2008,   "53-20-01-0A-01-10-00-00"}, 
	{GE_Z20505D_1414,   "53-20-01-0A-02-01-00-00"},
	{GE_Z20208D_1008,   "53-20-01-09-01-02-00-00"},
	{GE_Z20208D_2004,   "53-20-01-06-01-0F-00-00"},
	{GE_Z20208D_1004,   "53-20-01-05-01-01-00-00"},
	{GE_Z20204A_2013,   "53-20-01-06-04-02-00-00"},
	{GE_Z20204A_1013,   "53-20-01-05-04-01-00-00"},
	{GE_Z20204A_2017,   "53-20-01-0A-04-04-00-00"},
	{GE_Z20204A_1017,   "53-20-01-09-04-03-00-00"},
	{GE_Z20210D_2028,   "53-20-01-0C-06-01-00-00"},
	{GE_Z20210D_2024,   "53-20-01-08-06-01-00-00"},
	{GE_Z20208D_2008_P, "53-20-01-0A-01-1C-00-00"},
	{GE_Z20208D_2004_P, "53-20-01-06-01-1B-00-00"},
	{GE_SDI1000_0014_T, "54-10-01-06-01-01-00-00"},
	{GE_SDI1000_0014_R, "54-20-01-06-01-02-00-00"}
};


sw_error_t sal_sys_unique_id_get(sal_sys_unique_id_t  *unique_id)
{
    sw_error_t rv = SW_OK;
	char id[]=CONFIG_JWS_SERIAL_NUMBER;


	/*GE_Z20208D_2008*/
	if(!strcasecmp(id, "53-20-01-0A-01-10-00-00"))
		unique_id->sys_hardware_version = 0x01;
	/*GE_Z20505D_1414*/
	else if(!strcasecmp(id, "53-20-01-0A-02-01-00-00"))
		unique_id->sys_hardware_version = 0x02;
	/*GE_Z20208D_1008*/
	else if(!strcasecmp(id, "53-20-01-09-01-02-00-00"))
		unique_id->sys_hardware_version = 0x03;
	/*GE_Z20208D_2004*/
	else if(!strcasecmp(id, "53-20-01-06-01-0F-00-00"))
		unique_id->sys_hardware_version = 0x04;
	/*GE_Z20208D_1004*/
	else if(!strcasecmp(id, "53-20-01-05-01-01-00-00"))
		unique_id->sys_hardware_version = 0x05;
	/*GE_Z20204A_2013*/
	else if(!strcasecmp(id, "53-20-01-06-04-02-00-00"))
		unique_id->sys_hardware_version = 0x06;
	/*GE_Z20204A_1013*/
	else if(!strcasecmp(id, "53-20-01-05-04-01-00-00"))
		unique_id->sys_hardware_version = 0x07;
	/*GE_Z20204A_2017*/
	else if(!strcasecmp(id, "53-20-01-0A-04-04-00-00"))
		unique_id->sys_hardware_version = 0x08;
	/*GE_Z20204A_1017*/
	else if(!strcasecmp(id, "53-20-01-09-04-03-00-00"))
		unique_id->sys_hardware_version = 0x09;
	/*GE_Z20210D_2028*/
	else if(!strcasecmp(id, "53-20-01-0C-06-01-00-00"))
		unique_id->sys_hardware_version = 0x0A;
	/*GE_Z20210D_2024*/
	else if(!strcasecmp(id, "53-20-01-08-06-01-00-00"))
		unique_id->sys_hardware_version = 0x0B;
	/*GE_Z20208D_2008_P*/
	else if(!strcasecmp(id, "53-20-01-0A-01-1C-00-00"))
		unique_id->sys_hardware_version = 0x0C;
	/*GE_Z20208D_2004_P*/
	else if(!strcasecmp(id, "53-20-01-06-01-1B-00-00"))
		unique_id->sys_hardware_version = 0x0D;
	/*GE_SDI1000_0014_T*/
	else if(!strcasecmp(id, "54-10-01-06-01-01-00-00"))
		unique_id->sys_hardware_version = 0x0E;
	/*GE_SDI1000_0014_R*/
	else if(!strcasecmp(id, "54-20-01-06-01-02-00-00"))
		unique_id->sys_hardware_version = 0x0F;


	unique_id->sys_type = 0x53;
	unique_id->sys_sub_type = 0x21;
	unique_id->sys_hardware_type = 0x10;
	//unique_id->sys_hardware_version = 0x02;
	memset(&unique_id->reserved[0], 0xFF, 4);
	
    return rv;
}


sw_error_t sal_sys_model_get(sal_sys_model_t *sys_model)
{
    sw_error_t rv = SW_OK;
	a_uint32_t idx;
	a_uint32_t dev_model_id;
	sal_sys_unique_id_t uniqueId;
		
	SW_RTN_ON_ERROR(sal_sys_unique_id_get(&uniqueId));

	dev_model_id  = (a_uint32_t)uniqueId.sys_type << 24;
	dev_model_id |= (a_uint32_t)uniqueId.sys_sub_type << 16;
	dev_model_id |= (a_uint32_t)uniqueId.sys_hardware_type << 8;
	dev_model_id |= (a_uint32_t)uniqueId.sys_hardware_version;
	
	for(idx = 0; idx < sizeof (sal_sys_model) / sizeof (sal_sys_model[0]); idx++) {
		if(dev_model_id == sal_sys_model[idx].model_id) {
			sys_model->model_id = sal_sys_model[idx].model_id;
			aos_mem_copy(sys_model->model_string, sal_sys_model[idx].model_string, SAL_SYS_MODEL_STR_LEN);
			break;
		}
	}
	//if(idx == sizeof (sal_sys_model) / sizeof (sal_sys_model[0]))
		//return SW_NOT_FOUND;
	
    return rv;
}



