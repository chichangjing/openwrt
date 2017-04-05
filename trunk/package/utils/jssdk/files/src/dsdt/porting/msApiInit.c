
#include <Copyright.h>
#include "msJwSwitch.h"
#include "gtDrvSwRegs.h"
#include <gtDrvConfig.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>        //for struct ifreq

#include <sw_error.h>
#include "sal_sys.h"


#define MANUAL_MODE

GT_SYS_CONFIG   JwCfg;
GT_QD_DEV       JwDev;
GT_QD_DEV       *pJwDev=&JwDev;

int get_mac(char * addr, int len_limit)
{
	struct ifreq ifreq;
	int sock;

	sw_error_t status;
	char *tmp = NULL;
	int i;
	char data,mac[20];

	if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0){
		perror("socket");
		return -1;
	}
	strcpy (ifreq.ifr_name, "eth0");    //Currently, only get eth0
	if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0){
		perror("ioctl"); 
		return -1; 
	}
	
	status = snprintf(mac, len_limit, "%02X:%02X:%02X:%02X:%02X:%02X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
	
	tmp = strtok(mac,":");
	i = 0;
	while(tmp) {	
		sscanf(tmp,"%02x",&data);
		addr[i] = data;		
		tmp = strtok(NULL,"-:");
		i++;	
	}

	return status;
}


GT_STATUS jwSwitchStart(int cpuPort, int en_init_chip)
{
	GT_STATUS status;
	GT_LPORT hport;
	GT_U16 InitCtl,InitPolarity;

	GT_PHY_INFO	phyInfo;
	GT_U16 data;
	sal_sys_model_t model;

    memset((char*)&JwCfg,0,sizeof(GT_SYS_CONFIG));
    memset((char*)&JwDev,0,sizeof(GT_QD_DEV));

    JwCfg.BSPFunctions.readMii   = jwMiiRead;
    JwCfg.BSPFunctions.writeMii  = jwMiiWrite;
#ifdef USE_SEMAPHORE
    JwCfg.BSPFunctions.semCreate = osSemCreate;
    JwCfg.BSPFunctions.semDelete = osSemDelete;
    JwCfg.BSPFunctions.semTake   = osSemWait;
    JwCfg.BSPFunctions.semGive   = osSemSignal;
#else
    JwCfg.BSPFunctions.semCreate = NULL;
    JwCfg.BSPFunctions.semDelete = NULL;
    JwCfg.BSPFunctions.semTake   = NULL;
    JwCfg.BSPFunctions.semGive   = NULL;
#endif
    jwMiiInit();

    JwCfg.initPorts = GT_FALSE;
    JwCfg.cpuPortNum = cpuPort;
	JwCfg.skipInitSetup = GT_SKIP_INIT_SETUP;
	
#ifdef MANUAL_MODE
    JwCfg.mode.scanMode = SMI_MANUAL_MODE;
    JwCfg.mode.baseAddr = 0x0;
#else
#ifdef MULTI_ADDR_MODE
    JwCfg.mode.scanMode = SMI_MULTI_ADDR_MODE;
    JwCfg.mode.baseAddr = 1;
#else
    JwCfg.mode.scanMode = SMI_AUTO_SCAN_MODE;
    JwCfg.mode.baseAddr = 0;
#endif
#endif

    if((status=qdLoadDriver(&JwCfg, pJwDev)) != GT_OK) {
        printf("qdLoadDriver return Failed\n");
        return status;
    }
	#if 0
	printf("\nQuarterDeck Driver Initialized.\n");
    printf("  Switch deviceId   : 0x%x (%s)\n",pJwDev->deviceId, (pJwDev->deviceId == GT_88E6097)? "88E6097":"Unkown");
    printf("  Number of Ports   : %d\n",pJwDev->numOfPorts);
    printf("  CPU Port number   : %ld\n",pJwDev->cpuPortNum);
	#endif

	if(en_init_chip) {

		GT_ATU_ENTRY macEntry;
		unsigned char DevMac[18];
		GT_ETHERADDR PauseSA;
		int nRtn = get_mac(DevMac, sizeof(DevMac));
		GT_U16 SerdesIntEn, IntEn, PhyIntEn, IntStatus;
		
		if(nRtn > 0){        
			memset(&macEntry,0,sizeof(GT_ATU_ENTRY));	
			macEntry.macAddr.arEther[0] = DevMac[0];
			macEntry.macAddr.arEther[1] = DevMac[1];
			macEntry.macAddr.arEther[2] = DevMac[2];
			macEntry.macAddr.arEther[3] = DevMac[3];
			macEntry.macAddr.arEther[4] = DevMac[4];
			macEntry.macAddr.arEther[5] = DevMac[5];
			macEntry.DBNum = 0;
			macEntry.portVec = 1<<pJwDev->cpuPortNum;
			macEntry.prio = 7;
			macEntry.entryState.ucEntryState = GT_UC_STATIC;//GT_UC_TO_CPU_STATIC;

			if((status = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
				printf("gfdbAddMacEntry Failed!\n");
				return status;
			}
		}
		/* Set the Pause Frame's Souece Mac Address */
		memcpy(&PauseSA.arEther, DevMac, 6);
		if((status = gsysSetDuplexPauseMac(pJwDev, &PauseSA)) != GT_OK) {
	        printf("Error: gsysSetDuplexPauseMac failed\r\n");
	        return status;
	    }

		/* Bind Paging Multicast frame to All ports */
		memset(&macEntry, 0, sizeof(GT_ATU_ENTRY));
		macEntry.macAddr.arEther[0] = 0x01;
		macEntry.macAddr.arEther[1] = 0x00;
		macEntry.macAddr.arEther[2] = 0x5E;
		macEntry.macAddr.arEther[3] = 0x64;
		macEntry.macAddr.arEther[4] = 0xE4;
		macEntry.macAddr.arEther[5] = 0xE4;	
		macEntry.DBNum = 0;
		macEntry.portVec = 0x7FF;//1<<pJwDev->cpuPortNum;
		macEntry.prio = 0;
		macEntry.entryState.mcEntryState = GT_MC_STATIC;

		if((status = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
			printf("gfdbAddMacEntry return failed\r\n");
			return SW_FAIL;
		}

#if 0	
		memset(&macEntry,0,sizeof(GT_ATU_ENTRY));		
		macEntry.macAddr.arEther[0] = 0xFF;
		macEntry.macAddr.arEther[1] = 0xFF;
		macEntry.macAddr.arEther[2] = 0xFF;
		macEntry.macAddr.arEther[3] = 0xFF;
		macEntry.macAddr.arEther[4] = 0xFF;
		macEntry.macAddr.arEther[5] = 0xFF;
		macEntry.DBNum = 0;
		macEntry.portVec = 0x7FF;
		macEntry.prio = 0;
		macEntry.entryState.ucEntryState = GT_UC_STATIC;

		if((status = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
			return status;
		}
#endif

		/* Set cpu port to highest priority */
		if((status = gcosSetPortDefaultTc(pJwDev,pJwDev->cpuPortNum,7)) != GT_OK) {
	        printf("Error: gcosSetPortDefaultTc failed\r\n");
			return status;
	    }
		
		/* Default blocking all ports except Cpu port */
		for(hport=0; hport<pJwDev->numOfPorts; hport++) {
			if(hport == pJwDev->cpuPortNum){
				if((status = gstpSetPortState(pJwDev, hport, GT_PORT_FORWARDING)) != GT_OK) {
			        printf("Error: gstpSetPortState failed\r\n");
			        return status;
			    }
			} else{
				if((status = gstpSetPortState(pJwDev, hport, GT_PORT_BLOCKING)) != GT_OK) {
			        printf("Error: gstpSetPortState failed\r\n");
			        return status;
			    }
			}
		}

		/* Disable all Reserved DAs */
	    if((status = gsysSetRsvd2CpuEnables(pJwDev, 0)) != GT_OK) {
	        printf("Error: gsysSetRsvd2CpuEnables failed\r\n");
	        return status;
	    }

		/* Disable detect MGMT frames */
	    if((status = gsysSetRsvd2Cpu(pJwDev, GT_FALSE)) != GT_OK) {
	        printf("Error: gsysSetRsvd2CpuEnables failed\r\n");
	        return status;
	    }

		/* CPU Destination port */
	    if((status = gsysSetCPUDest(pJwDev, pJwDev->cpuPortNum)) != GT_OK) {
	        printf("Error: gsysSetRsvd2CpuEnables failed\r\n");
	        return status;
	    }

		/* PIRL initialize */
	    if((status = gpirl2Initialize(pJwDev)) != GT_OK) {
	        printf("Error: gpirl2Initialize failed\r\n");
	        return status;
	    }

		/* Boardcast are not consider Multicast frames, regardless of Egress Floods */
		if((status = gsysSetFloodBC(pJwDev, GT_TRUE)) != GT_OK) {
	        printf("Error: gsysSetFloodBC failed\r\n");
	        return status;
	    }

		/* egress frame with unknown  DA(unicast and multicast) except CPU port */	
		for(hport=0; hport<pJwDev->numOfPorts; hport++) {
			if(hport == pJwDev->cpuPortNum) {
				if((status = gprtSetEgressFlood(pJwDev, hport, GT_BLOCK_EGRESS_UNKNOWN)) != GT_OK) {
					printf("Error: gprtSetEgressFlood failed\r\n");
					return status;
				}
			} else {
				if((status = gprtSetEgressFlood(pJwDev, hport, GT_BLOCK_EGRESS_NONE)) != GT_OK) {
					printf("Error: gprtSetEgressFlood failed\r\n");
					return status;
				}
			}
		}

		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			case GE_Z20208D_2008:
			case GE_Z20208D_1008:
			case GE_Z20208D_2004:
			case GE_Z20208D_1004:
			case GE_Z20208D_2008_P:
			case GE_Z20208D_2004_P:
				/* enable 0-7 port's admin-mode */
				for(hport=0; hport<8; hport++) {
					if((status = gprtPortPowerDown(pJwDev, hport, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail!\n");
				        return SW_FAIL;
				    } 
				}
				/* hport 8 and 10 admin-mode enable */
				hport = 8;
				if((status = gpcsSetForcedLink(pJwDev, hport, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedLink Fail!\n");
			        return SW_FAIL;
			    }
				hport = 10;
				if((status = gpcsSetForcedLink(pJwDev, hport, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedLink Fail!\n");
			        return SW_FAIL;
			    }			
			break;
			
			case GE_Z20505D_1414:
				/* enable 0-8 port's admin-mode */
				for(hport=0; hport<9; hport++) {
					if((status = gprtPortPowerDown(pJwDev, hport, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail!\n");
				        return SW_FAIL;
				    } 
				}
				/* hport 10 admin-mode enable */
				hport = 10;
				if((status = gpcsSetForcedLink(pJwDev, hport, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedLink Fail!\n");
			        return SW_FAIL;
			    }


				/* Check hport 8 external phy id */
				hport = 8;
				phyInfo.phyId = IS_CONFIGURABLE_PHY(pJwDev,hport);
				if((phyInfo.phyId & PHY_MODEL_MASK) != DEV_E1112) {
					printf("External PHY Not found!\n");
					return SW_FAIL;		
				} else {
					/* 88E1112 PHY, Page Address 0x16 Reset to 3 */
					hwWritePhyReg(pJwDev, hport, 0x16, 3);
					/* Set Intn Pin function as Interrupt, hardware reset 0x21e */
					hwReadPhyReg(pJwDev, hport, 0x10, &InitCtl);
					printf("InitCtl = 0x%x\n",InitCtl);
					InitCtl &= 0xF0FF;
					InitCtl |= 0xE00;
					printf("InitCtl = 0x%x\n",InitCtl);
					hwWritePhyReg(pJwDev, hport, 0x10, InitCtl);

					/* Init Pin Polarity,On-Drive Low, Off-Drive High, 5:4 - 00 mode */
					hwReadPhyReg(pJwDev, hport, 0x11, &InitPolarity);
					printf("InitPolarity = 0x%x\n",InitPolarity);
					InitPolarity &= 0xFFCF;
					printf("InitPolarity = 0x%x\n",InitPolarity);
					hwWritePhyReg(pJwDev, hport, 0x11, InitPolarity);
					
					/* 88E1112 PHY hport 8,Page Address 0x16 Reset to 0 */
					hwWritePhyReg(pJwDev, hport, 0x16, 0);			
					/* Restart auto-negotiation */
					if((status = gprtPortRestartAutoNeg(pJwDev, hport)) != GT_OK) {
						printf("Error: hport 8 gprtPortRestartAutoNeg Fail!\n");
						return SW_FAIL;
					}
	
					/* External PHY Copper Interrupt initialize */
					PhyIntEn = GT_LINK_STATUS_CHANGED;
					if((status = gprtPhyIntEnable(pJwDev, hport, PhyIntEn)) != GT_OK) {
						printf("gprtPhyIntEnable Fail!\n");
						return SW_FAIL;
					}
				}					
			break;

			/* Yuxi customization product */
			case GE_SDI1000_0014_T:
			case GE_SDI1000_0014_R:
				/* Default blocking all ports except Cpu port */
				for(hport=0; hport<pJwDev->numOfPorts; hport++) {
					if((status = gstpSetPortState(pJwDev, hport, GT_PORT_FORWARDING)) != GT_OK) {
				        printf("Error: gstpSetPortState failed\r\n");
				        return status;
				    }
				}
				
				/* enable 0-3 and 8 port's admin-mode */
				for(hport=0; hport<9; hport++) {
					if((hport == 4) || (hport == 5) || (hport == 6) | (hport == 7))
						continue;
					if((status = gprtPortPowerDown(pJwDev, hport, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail!\n");
				        return SW_FAIL;
				    } 
				}
				/* Force P10 link up, full-duplex, 1000M speed, to communication with FPGA */
				hport = 10;
				/* Disable auto-negotiation */
				if((status = gpcsSetPCSAnEn(pJwDev, hport, GT_FALSE)) != GT_OK) {
		            printf("Error: gpcsSetPCSAnEn\n");
					return SW_FAIL;
		        }
				/* Set speed to 1000M bps */
				if((status = gpcsSetForceSpeed(pJwDev, hport, PHY_SPEED_1000_MBPS)) != GT_OK) {
		            printf("Error: gpcsSetForceSpeed\n");
					return SW_FAIL;
		        }
				/* Set duplex to full-duplex */
				if((status = gpcsSetDpxValue(pJwDev, hport, GT_TRUE)) != GT_OK) {
		            printf("Error: gpcsSetDpxValue\n");
					return SW_FAIL;
		        }						
				if((status = gpcsSetForcedDpx(pJwDev, hport, GT_TRUE)) != GT_OK) {
		            printf("Error:gpcsSetForcedDpx\n");
					return SW_FAIL;
		        }
				if((status = gpcsSetLinkValue(pJwDev, hport, GT_TRUE)) != GT_OK) {
					printf("Error: gpcsSetLinkValue\n");
			        return SW_FAIL;
			    }
			    if((status = gpcsSetForcedLink(pJwDev, hport, GT_TRUE)) != GT_OK) {
					printf("Error: gpcsSetForcedLink\n");
			        return SW_FAIL;
			    }


				/* Check hport 8 external phy id */
				hport = 8;
				phyInfo.phyId = IS_CONFIGURABLE_PHY(pJwDev,hport);
				if((phyInfo.phyId & PHY_MODEL_MASK) != DEV_E1112) {
					printf("External PHY Not found!\n");
					return SW_FAIL;		
				} else {				
					/* 88E1112 PHY, Page Address 0x16 Reset to 0 */
					hwWritePhyReg(pJwDev, hport, 0x16, 0);
					/* Restart Copper auto-negotiation */
					if((status = gprtPortRestartAutoNeg(pJwDev, hport)) != GT_OK) {
						printf("Error: hport_%d gprtPortRestartAutoNeg Fail!\n",hport);
						return SW_FAIL;
					}	
				}	
			break;

			case GE_Z20204A_2013:
			case GE_Z20204A_1013:
			case GE_Z20204A_2017:
			case GE_Z20204A_1017:
				/* enable 0-7, 9 port's admin-mode */
				for(hport=0; hport<10; hport++) {
					if(hport == 8)
						continue;
					if((status = gprtPortPowerDown(pJwDev, hport, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail!\n");
				        return SW_FAIL;
				    } 
				}
				
				/* hport 8 and 10 admin-mode enable */
				hport = 8;
				if((status = gpcsSetForcedLink(pJwDev, hport, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedLink Fail!\n");
			        return SW_FAIL;
			    }
				hport = 10;
				if((status = gpcsSetForcedLink(pJwDev, hport, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedLink Fail!\n");
			        return SW_FAIL;
			    }
				
				/* Check hport 9 external phy id */
				hport = 9;
				phyInfo.phyId = IS_CONFIGURABLE_PHY(pJwDev,hport);
				if((phyInfo.phyId & PHY_MODEL_MASK) != DEV_E1112) {
					printf("External PHY Not found!\n");
					return SW_FAIL;		
				} else {
					/* 88E1112 PHY, Page Address 0x16 Reset to 3 */
					hwWritePhyReg(pJwDev, hport, 0x16, 3);
					/* Set Intn Pin function as Interrupt, hardware reset 0x21e */
					hwReadPhyReg(pJwDev, hport, 0x10, &InitCtl);
					printf("InitCtl = 0x%x\n",InitCtl);
					InitCtl &= 0xF0FF;
					InitCtl |= 0xE00;
					printf("InitCtl = 0x%x\n",InitCtl);
					hwWritePhyReg(pJwDev, hport, 0x10, InitCtl);

					/* Init Pin Polarity,On-Drive Low, Off-Drive High, 5:4 - 00 mode */
					hwReadPhyReg(pJwDev, hport, 0x11, &InitPolarity);
					printf("InitPolarity = 0x%x\n",InitPolarity);
					InitPolarity &= 0xFFCF;
					printf("InitPolarity = 0x%x\n",InitPolarity);
					hwWritePhyReg(pJwDev, hport, 0x11, InitPolarity);
					
					/* 88E1112 PHY hport 9,Page Address 0x16 Reset to 0 */
					hwWritePhyReg(pJwDev, hport, 0x16, 0);
					/* Restart auto-negotiation */
					if((status = gprtPortRestartAutoNeg(pJwDev, hport)) != GT_OK) {
						printf("Error: hport 9 gprtPortRestartAutoNeg Fail!\n");
						return SW_FAIL;
					}

					/* External PHY Copper Interrupt initialize */
					PhyIntEn = GT_LINK_STATUS_CHANGED;
					if((status = gprtPhyIntEnable(pJwDev, hport, PhyIntEn)) != GT_OK) {
						printf("gprtPhyIntEnable Fail!\n");
				        return SW_FAIL;
				    }
				}	
			break;

			case GE_Z20210D_2028:
			case GE_Z20210D_2024:
				/* enable 0-10, exccept 9 port's admin-mode */
				for(hport=0; hport<11; hport++) {
					if(hport == 9)
						continue;
					if((status = gprtPortPowerDown(pJwDev, hport, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail!\n");
				        return SW_FAIL;
				    } 
				}
		
				/* Check hport 8 external phy id */
				for(hport=8; hport<11; hport++){
					if(hport == 9)
						continue;
					
					phyInfo.phyId = IS_CONFIGURABLE_PHY(pJwDev,hport);
					if((phyInfo.phyId & PHY_MODEL_MASK) != DEV_E1112) {
						printf("External PHY Not found!\n");
						return SW_FAIL;		
					} else {
						/* 88E1112 PHY, Page Address 0x16 Reset to 0 */
						hwWritePhyReg(pJwDev, hport, 0x16, 0);
						/* Restart Copper auto-negotiation */
						if((status = gprtPortRestartAutoNeg(pJwDev, hport)) != GT_OK) {
							printf("Error: hport_%d gprtPortRestartAutoNeg Fail!\n",hport);
							return SW_FAIL;
						}	
					}	
				}
			break;

			default:
			break;
		}	

		/* Interrupt, Enable SerdesIntEn, PHYIntEn, PHYLinkIntEn */
		if((status = hwReadGlobal2Reg(pJwDev, QD_REG_DEVINT_MASK, &SerdesIntEn)) != GT_OK) {
			printf("hwReadGlobal2Reg Fail!\n");
	        return SW_FAIL;
	    }
		SerdesIntEn &= 0xF8FF;
		SerdesIntEn |= 0x0700;  
		if((status = hwWriteGlobal2Reg(pJwDev, QD_REG_DEVINT_MASK, SerdesIntEn)) != GT_OK) {
			printf("hwWriteGlobal2Reg Fail!\n");
	        return SW_FAIL;
	    }

		/*PPU enable, and device.phy interrupt enable*/
		IntEn = GT_PHY_INTERRUPT | GT_DEVICE_INT;
		if((status = eventSetActive(pJwDev, IntEn)) != GT_OK) {
			printf("eventSetActive Fail!\n");
	        return SW_FAIL;
	    }
		PhyIntEn = GT_LINK_STATUS_CHANGED;
		for(hport=0; hport<8; hport++) {
			if((status = gprtPhyIntEnable(pJwDev, hport, PhyIntEn)) != GT_OK) {
				printf("gprtPhyIntEnable Fail!\n");
		        return SW_FAIL;
		    }
		}
		
		/* Only Read Interrupt status(SerdesInt, PHYInt),therefore the Int pin status can recover */
		if((status = hwReadGlobal2Reg(pJwDev, QD_REG_DEVINT_SOURCE, &IntStatus)) != GT_OK) {
			printf("hwReadGlobal2Reg Fail!\n");
	        return SW_FAIL;
	    }
		for(hport=0; hport<8; hport++) {
			/* PHY Interrupt Status Read */
			if((status = hwReadPhyReg(pJwDev, hport, 0x13, &IntStatus)) != GT_OK) {
				printf("hwReadGlobal2Reg Fail!\n");
				return SW_FAIL;
			}
		}
		
		printf("!! up to date !!\n");
	}

    if((status=sysEnable(pJwDev)) != GT_OK) {
        printf("sysConfig return Failed\n");
        return status;
    }
    //printf("QuarterDeck has been started.\n");

    return GT_OK;
}


