#include <madCopyright.h>
#include "madSample.h"
#include "mx_api_helper.h"
#include "msec_defines.h"
#include "msecTest.h"
#include "setMACsec.h"
/*
####################################################################################
# macsec_1.10.1_FIPS_compliance_test.tcl
# This test covers FIPS compliance testing.
# 1. Set fips_vec, key, eng_type values of the structure FIPS_CFG_T as below table
# for all ingress and egress port.
# 2. Call port_fips_chk API call. Output of the API call should
#   match with 128bit OBSV VEC shown below.
# 3. Clear register FIPS_VEC0 madWait(completing each set which is 
#   cpu_port_write(dev_ctrl, FIPS_VEC0, 0x0)
# ---------------------------------------------------------
# FIPS_VEC	: 128h'1
# KEY		: 128h'0
# ENG_TYPE	: AES (0x1)
# OBSV_VEC	: 128h'58e2fccefa7e3061367f1d57a4e7455a
# ---------------------------------------------------------
# ---------------------------------------------------------
# FIPS_VEC	: 128h'1
# KEY		: 128h'66e94bd4ef8a2c3b884cfa59ca342b2e
# ENG_TYPE   : GHASH (0x0)
# OBSV VEC   : 128'h52a4dcb814e54ae1b2d2402fdc6eb849
# ---------------------------------------------------------

# ---------------------------------------------------------
# FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
# KEY        : 128'h102030405060708090a0b0c0d0e0f
# ENG_TYPE   : AES (0x1)
# OBSV VEC   : 128'h69c4e0d86a7b0430d8cdb78070b4c55a
#  ---------------------------------------------------------

# ---------------------------------------------------------
#  FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
#  KEY        : 128'hc6a13b37878f5b826f4f8162a1c8d879
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hd30a3456a32df44031e85973107a7e33
#  ---------------------------------------------------------

#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'h3247184b3c4f69a44dbcd22887bbb418
#  ---------------------------------------------------------

#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'h32634afb93935d8e2d2340fdcc959e6d
#  ---------------------------------------------------------

#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'he94ab9535c72bea9e089c93d48e62fb0
#  ---------------------------------------------------------

#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hea27e813379d8641d0ae4ec5a1290392
#  ---------------------------------------------------------
####################################################################################
*/
// OBSV_VEC_0	: 128h'58e2fccefa7e3061367f1d57a4e7455a
static  char farr [16];
static  char karr [16];
static  unsigned int iports[4] = {3,0,1,0}; // "1 3"
static  unsigned int eports[4] = {0,2,0,1}; // "0 2"
static int egr_igr;
static  FIPS_CFG_T * fips_cfg;

static unsigned char obsv_vec_0[16] = {0x58, 0xe2, 0xfc, 0xce, 0xfa, 0x7e, 0x30, 0x61,
                              0x36, 0x7f, 0x1d, 0x57, 0xa4, 0xe7, 0x45, 0x5a};
static  unsigned int out_arr[4];
/*
###########################################################
# Entry 1
# setup fips_vec, karr and eng_type
# ---------------------------------------------------------
# FIPS_VEC	: 128h'1
# KEY		: 128h'66e94bd4ef8a2c3b884cfa59ca342b2e
# ENG_TYPE   : GHASH (0x0)
# OBSV VEC   : 128'h52a4dcb814e54ae1b2d2402fdc6eb849
# ---------------------------------------------------------
###########################################################"
*/
unsigned char key_vec_1[16] = {0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b, 0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e};
unsigned char obsv_vec_1 [16] = {0x52, 0xa4, 0xdc, 0xb8, 0x14, 0xe5, 0x4a, 0xe1, 0xb2, 0xd2, 0x40, 0x2f, 0xdc, 0x6e, 0xb8, 0x49};

/*
###########################################################
# Entry 2
# setup fips_vec, key and eng_type
# ---------------------------------------------------------
# FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
# KEY        : 128'h000102030405060708090a0b0c0d0e0f
# ENG_TYPE   : AES (0x1)
# OBSV VEC   : 128'h69c4e0d86a7b0430d8cdb78070b4c55a
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_2 [16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
unsigned char key_vec_2 [16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
unsigned char obsv_vec_2 [16] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
/*
###########################################################
# Entry 3
# setup fips_vec, key and eng_type
# ---------------------------------------------------------
#  FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
#  KEY        : 128'hc6a13b37878f5b826f4f8162a1c8d879
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hd30a3456a32df44031e85973107a7e33
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_3 [16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
unsigned char key_vec_3 [16] = {0xc6, 0xa1, 0x3b, 0x37, 0x87, 0x8f, 0x5b, 0x82, 0x6f, 0x4f, 0x81, 0x62, 0xa1, 0xc8, 0xd8, 0x79};
unsigned char obsv_vec_3 [16] = {0xd3, 0x0a, 0x34, 0x56, 0xa3, 0x2d, 0xf4, 0x40, 0x31, 0xe8, 0x59, 0x73, 0x10, 0x7a, 0x7e, 0x33};

/*
###########################################################
# Entry 4
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'h3247184b3c4f69a44dbcd22887bbb418
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_4 [16] = {0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88, 0x00, 0x00, 0x00, 0x01};
unsigned char key_vec_4 [16] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};
unsigned char obsv_vec_4 [16] = {0x32, 0x47, 0x18, 0x4b, 0x3c, 0x4f, 0x69, 0xa4, 0x4d, 0xbc, 0xd2, 0x28, 0x87, 0xbb, 0xb4, 0x18};

/*
###########################################################
# Entry 5
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'h32634afb93935d8e2d2340fdcc959e6d
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_5 [16] = {0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad, 0xde, 0xca, 0xf8, 0x88, 0x00, 0x00, 0x00, 0x01};
unsigned char key_vec_5 [16] = {0xb8, 0x3b, 0x53, 0x37, 0x08, 0xbf, 0x53, 0x5d, 0x0a, 0xa6, 0xe5, 0x29, 0x80, 0xd5, 0x3b, 0x78};
unsigned char obsv_vec_5 [16] = {0x32, 0x63, 0x4a, 0xfb, 0x93, 0x93, 0x5d, 0x8e, 0x2d, 0x23, 0x40, 0xfd, 0xcc, 0x95, 0x9e, 0x6d};

/*
###########################################################
# Entry 6
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'he94ab9535c72bea9e089c93d48e62fb0
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_6 [16] = {0xc4, 0x3a, 0x83, 0xc4, 0xc4, 0xba, 0xde, 0xc4, 0x35, 0x4c, 0xa9, 0x84, 0xdb, 0x25, 0x2f, 0x7d};
unsigned char key_vec_6 [16] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c, 0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};
unsigned char obsv_vec_6 [16] = {0xe9, 0x4a, 0xb9, 0x53, 0x5c, 0x72, 0xbe, 0xa9, 0xe0, 0x89, 0xc9, 0x3d, 0x48, 0xe6, 0x2f, 0xb0};

/*
###########################################################
# Entry 7
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hea27e813379d8641d0ae4ec5a1290392
#  ---------------------------------------------------------
###########################################################"
*/
unsigned char fips_vec_7 [16] = {0xc4, 0x3a, 0x83, 0xc4, 0xc4, 0xba, 0xde, 0xc4, 0x35, 0x4c, 0xa9, 0x84, 0xdb, 0x25, 0x2f, 0x7d};
unsigned char key_vec_7 [16] = {0xb8, 0x3b, 0x53, 0x37, 0x08, 0xbf, 0x53, 0x5d, 0x0a, 0xa6, 0xe5, 0x29, 0x80, 0xd5, 0x3b, 0x78};
unsigned char obsv_vec_7 [16] = {0xea, 0x27, 0xe8, 0x13, 0x37, 0x9d, 0x86, 0x41, 0xd0, 0xae, 0x4e, 0xc5, 0xa1, 0x29, 0x03, 0x92};


void test_FIPS(MAD_DEV *devp, unsigned char *fips_vec, unsigned char *key_vec, 
			   unsigned char *obsv_vec, int eng_type)
{
  int rc, i = 0;

  printf("fips_vec: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
  fips_vec[0], fips_vec[1],fips_vec[2],fips_vec[3],
  fips_vec[4], fips_vec[5],fips_vec[6],fips_vec[7],
  fips_vec[8], fips_vec[8],fips_vec[10],fips_vec[11],
  fips_vec[12], fips_vec[13],fips_vec[14],fips_vec[15]);
  printf("key_vec: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
  key_vec[0], key_vec[1],key_vec[2],key_vec[3],
  key_vec[4], key_vec[5],key_vec[6],key_vec[7],
  key_vec[8], key_vec[8],key_vec[10],key_vec[11],
  key_vec[12], key_vec[13],key_vec[14],key_vec[15]);
  printf("obsv_vec: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
  obsv_vec[0], obsv_vec[1],obsv_vec[2],obsv_vec[3],
  obsv_vec[4], obsv_vec[5],obsv_vec[6],obsv_vec[7],
  obsv_vec[8], obsv_vec[8],obsv_vec[10],obsv_vec[11],
  obsv_vec[12], obsv_vec[13],obsv_vec[14],obsv_vec[15]);

  for (i=0; i<16; i++) {
    set_u8_arr_fld(farr, i, fips_vec[i]);
    set_u8_arr_fld(karr, i, key_vec[i]);
  }

// check the OBSV_VEC for each port, it shall match OBSV_VEC shown below:
  printf("Set fips_cfg and get the fips output_vec for each egr port...\n");
  egr_igr = 1;
  fips_cfg = new_fips_cfg_t(farr, karr, egr_igr, eng_type);
  set_fips_cfg_t(fips_cfg, farr, karr, egr_igr, eng_type);
  if(validate_fips_compliance(devp, eports, fips_cfg, out_arr, obsv_vec) !=0) {
    rc = -1;
  }

  printf("Set fips_cfg and get the fips output_vec for each igr port...\n");
  egr_igr = 0;
  set_fips_cfg_t(fips_cfg, farr, karr, egr_igr, eng_type);
  if(validate_fips_compliance(devp, iports, fips_cfg, out_arr, obsv_vec) !=0) {
    rc = -1;
  }

  delete_fips_cfg_t(fips_cfg);

  madWait(1000);

  printf("\n");
}



void macsec_1_10_1_FIPS_compliance_test(MAD_DEV *devp)
{
  int rc = 0;

  int i;
  unsigned char eng_type = 1;

 //  setMACsec();

  printf("MACSec validation macsec_1_10_1_FIPS_compliance_test\n");
  printf("========================================================\n");

  // initialize MACsec API on usbport 0
  if (devp == NULL) {
	printf("Initialization of MACSecAPI failed.\n");
	return;
  }
  initialize_macsec_api(devp);
  stat_arr_init ();

  printf( "Test in progress, please wait ...\n");

  // initialize MACSec moduel with link speed 1000Mbps
  macsec_start(devp, 2);

  madWait(2000);
 
  printf("\n");
/*
###########################################################
# Entry 0
# setup fips_vec, key and eng_type
# ---------------------------------------------------------
# FIPS_VEC	: 128h'1
# KEY		: 128h'0
# ENG_TYPE	: AES (0x1)
# OBSV_VEC	: 128h'58e2fccefa7e3061367f1d57a4e7455a
# ---------------------------------------------------------
###########################################################"
*/

  for (i=0; i<15; i++) {
    set_u8_arr_fld(farr, i, 0);
  }
  set_u8_arr_fld(farr, 15, 0x01);

  for (i=0; i<16; i++) {
    set_u8_arr_fld(karr, i, 0);
  }
  eng_type = 1;

  printf("Set fips_cfg and get the fips output_vec for each egr port...\n");

  test_FIPS(devp, farr, karr, obsv_vec_0, eng_type);
  madWait(1000);


  printf("\n");
/*
###########################################################
# Entry 1
# setup fips_vec, karr and eng_type
# ---------------------------------------------------------
# FIPS_VEC	: 128h'1
# KEY		: 128h'66e94bd4ef8a2c3b884cfa59ca342b2e
# ENG_TYPE   : GHASH (0x0)
# OBSV VEC   : 128'h52a4dcb814e54ae1b2d2402fdc6eb849
# ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x0;
  for (i=0; i<15; i++) {
    set_u8_arr_fld(farr, i, 0);
  }
  set_u8_arr_fld(farr, 15, 0x01);

  eng_type = 0;

  // check the OBSV_VEC for each port, it shall match OBSV_VEC shown below:
  printf("Set fips_cfg and get the fips output_vec for each egr port...\n");
  test_FIPS(devp, farr, key_vec_1, obsv_vec_1, eng_type);
  madWait(1000);
  printf("\n");
/*
###########################################################
# Entry 2
# setup fips_vec, key and eng_type
# ---------------------------------------------------------
# FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
# KEY        : 128'h000102030405060708090a0b0c0d0e0f
# ENG_TYPE   : AES (0x1)
# OBSV VEC   : 128'h69c4e0d86a7b0430d8cdb78070b4c55a
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x1;
  test_FIPS(devp, fips_vec_2, key_vec_2, obsv_vec_2, eng_type);

  madWait(1000);

  printf("\n");
/*
###########################################################
# Entry 3
# setup fips_vec, key and eng_type
# ---------------------------------------------------------
#  FIPS_VEC   : 128'h00112233445566778899aabbccddeeff
#  KEY        : 128'hc6a13b37878f5b826f4f8162a1c8d879
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hd30a3456a32df44031e85973107a7e33
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x0;
  test_FIPS(devp, fips_vec_3, key_vec_3, obsv_vec_3, eng_type);
  madWait(1000);

  printf("\n");
/*
###########################################################
# Entry 4
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'h3247184b3c4f69a44dbcd22887bbb418
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x1;
  test_FIPS(devp, fips_vec_4, key_vec_4, obsv_vec_4, eng_type);

  madWait(1000);

  printf("\n");
/*
###########################################################
# Entry 5
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hcafebabefacedbaddecaf88800000001
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'h32634afb93935d8e2d2340fdcc959e6d
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x0;
  test_FIPS(devp, fips_vec_5, key_vec_5, obsv_vec_5, eng_type);
  madWait(1000);

  printf("\n");
/*
###########################################################
# Entry 6
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hfeffe9928665731c6d6a8f9467308308
#  ENG_TYPE   : AES (0x1)
#  OBSV VEC   : 128'he94ab9535c72bea9e089c93d48e62fb0
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x1;
  test_FIPS(devp, fips_vec_6, key_vec_6, obsv_vec_6, eng_type);

  madWait(1000);
  printf("\n");
/*
###########################################################
# Entry 7
# setup fips_vec, key and eng_type
#  ---------------------------------------------------------
#  FIPS_VEC   : 128'hc43a83c4c4badec4354ca984db252f7d
#  KEY        : 128'hb83b533708bf535d0aa6e52980d53b78
#  ENG_TYPE   : GHASH (0x0)
#  OBSV VEC   : 128'hea27e813379d8641d0ae4ec5a1290392
#  ---------------------------------------------------------
###########################################################"
*/
  eng_type = 0x0;
  test_FIPS(devp, fips_vec_7, key_vec_7, obsv_vec_7, eng_type);

  printf("\n");
/*
###########################################################
# Check and print test result
###########################################################"
*/
  if(rc == 0) {
    printf("Test passed.\n");
    printf( "Test passed. Please check result/ for detailed information.\n");
  } else {
    printf("Test failed.\n");
    printf( "Test failed. Please check result/ for detailed information.\n");
  }

}

