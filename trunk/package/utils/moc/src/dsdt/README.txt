
                    DSDT Release version 3.6.3
                   ==========================


DSDT Release version 3.6.1 GPL 2.0 Compliant.

Copyright (C) 2010 (or other year developed and each year modified and/or added to) 
Marvell International Ltd.
 
This program is free software; you can redistribute it and/or modify it under the terms 
of the GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc., 
675 Mass Ave, Cambridge, MA 02139, USA.


Marvell Semiconductor, Inc.
5488 Marvell Lane
Santa Clara, CA 95054, USA

Tel: 1.408.222.2500
Fax: 1.408.988.8279

www.marvell.com


Table of Content:
-----------------
1) Release History 
2) Source Code Organization
3) General Introduction
4) How to - build Marvell drivers
5) Example of global building variables

1) Release History
------------------
DSDT_3.6.3/switch - Oct.04.2016
	1. add support for Remote Management
	2. Fixed bugs to support phy related feature for 88E6190X, 88E6390X

DSDT_3.6.2/switch - May.27.2016
	1. add the support of 88E6190X, 88E6390X

DSDT_3.6.1/switch - Mar.25.2016
	1. fixed all the compile errors on the former release

DSDT_3.6/switch - Mar.17. 2016.
	  1. fixed the bugs related mibs, serdes, vtu, interal phy, 64bit architecture specific bugs. 

DSDT_3.5_pre_11/switch - Feb.10. 2015.
          1. Updated Serdes functions.
          2. Fixed bugs.

DSDT3.5_pre.zip - Jan.10. 2014. 
         Added switch Peridot.

DSDT3.4.zip - Dec.1. 2013. 
         Added XMDC/XMDIO access on swtch global 2 register.
         Updated Phy debug log processing.
DSDT3.3.zip - Feb.1. 2013. 
         Added WOL on Phy 151x.
         Added switch 6320
         Added Phy 1680.
         Added DSP VCT on Phy 1680.
         Moved shered directory to phy/src/macfunc.

DSDT3.2.zip - Feb.1. 2012. 
         Added switch 6320
         Added Phy 1548.

DSDT3.1.zip - April.1. 2011. 
         Added switch 6352
         Added Phy 1540.
         Added switch and phy in FirefoxAv.

DSDT3.0.zip - May 26. 2010. 
         Merged and developed from Marvell SOHO Switch driver DSDT_2.8a, 
	 and Marvell Phy driver MAD_2.5.
         Supports switch chips, 88EC0XX and 88E6250.
         Update phy macsec to support 1540 y0.
         Fixed bugs.

2) Source Code Organization
--------------------------
    2.1) Switch 
        Marvell SOHO Switch Driver. See README_SW.

    2.2) Phy 
        Marvell Phy Driver.	See README_phy

3) General Introduction
-----------------------

The DSDT 3.xx supports Marvell SOHO Switch and Phy products.
The DSDT 3.xx starts from original DSDT 2.8a and MAD 2.5.
The DSDT 3.xx follows original DSDT 2.x and MAD 2.x. 
The customers, who use DSDT 2.x (qdDrv), use DSDT_3.x/switch to get support of switch driver.
The customers, who use MAD 2.x (madDrv), use DSDT_3.x/phy to get support of phy driver.
The DSDT 3.xx provided more new functions to support Marvell Switch and Phy products.
The complete DSDT driver suite includes
  Marvell SOHO Switch driver (switch), 
  Marvell phy driver (phy), 
  ...

The DSDT driver might not include the complete driver, that is based on customer's application.
Phy customer might include DSDT_x.y/phy part only.
SOHO switch customer might include DSDT_x.y/switch part only.

In more detail see README_SW and README_PHY in each driver.

4) How to - build Marvell drivers
---------------------------------
    In Linux system,
    4.1) Build Marvell SOHO Switch driver only (qdDrv)
        <make SW> in root directory.
    4.2) Build Marvell Phy driver (madDrv)
        <make PHY> in root directory.
    4.3) Build Marvell SOHO Switch and Phy drivers
        <make> in root directory.
    4.4) Build Marvell SOHO Switch driver on madDrv 
        <make DSDT_USE_MAD=TRUE> in root directory.

5) Example of global building variables		
---------------------------------
  <setenv> includes example of building global variables in Linux environment.
