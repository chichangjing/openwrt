####################################################################################
# macsec_1.7.1_AE_PN_Saturation_test.tcl
# This test verifies the behavior of AE core when PN get saturated, all packets should
# be dropped.
# Configure the device as in macsec_1.3.1. Configure the entries 3~7 of PN of ECT 
# table to 32'hFFFFFF00. 
# 
#
# Table1: ELU Configuration Table
# =============================================================
# Entry	DA		SA		EtherType	VLAN	Default
#  0			AABBCCDDEE00				0		
#  1			AABBCCDDEE01				0
#  2			AABBCCDDEE02				0
#  3	 		AABBCCDDEE03				0
#  4			AABBCCDDEE04				0
#  5			AABBCCDDEE05				0
#  6			AABBCCDDEE06				0
#  7			AABBCCDDEE07				0
# 
#  Table2: Context/Key Configuration
# =====================================================================================
# Entry	SCI			TCI	Key			Encrypt	EncryptStartAdj
#  0	ABCDEF01FEDCBA10	2C	FEDCBA98FEDCBA98	1	0
#					FEDCBA98FEDCBA98
#  1	FEDCBA10ABCDEF01	2D	1246857965894521	1	30
#  					6986454744412569
#  2	ABCDEF01FEDCBA22	2E	1447586945214521	1	50
#					7874698522365478		
#  3	FEDCBA10ABCDEF12	2F	AABBCCDDEEFF9988	1	0
#					AABBCCDDEEFF9988
#  4	FEDCBA10ABCDEF39	2C	FEDCBA98FEDCBA98	0	30
#	  				FEDCBA98FEDCBA98
#  5	FEDCBA10ABCDEF46	2D	1245369885476325	0	0
#	 				9645312524869731	
#  6	FEDCBA10ABCDEF05	2E	1141559747469931	0	50
#					FEDEBABADDEFFEDA
#  7	FEDCBA10ABCDEFAB	2F	ABACDDECFEECCADB	0	0
#					4745698521454786
#
# Test procedure:
# Configure IGR and EGR as defined in Table1 &Table2.
# Set PN of ECT table to 0xFFFFFF00 for entries 3~7.
# send 300 bursty frames each matching the SA in Table1. 
# check statistics on receiving port. frames that hit entry 3~7 shall have only first
# 256 frames sent, and rest frames will generate PN saturation and be dropped be EGR.
# After that, set pn to 1 for entry 3~7 and verify the normal functionality resumes.
####################################################################################

package require AgtClient
source n2xCtrl.tcl
source setMACsec.tcl

set hResult [create_result_file $argv0]
global hResult
puts $hResult "MACSec validation $argv0"
puts $hResult "========================================================"
flush $hResult

puts $hResult "\n
# initialize n2x connection"
if [catch {initialize_n2x_connection my4650e704 macSecValidation}] {
	Error "Connection to N2X failed."
	puts $hResult "Connection to N2X failed."
	return -1
}

puts $hResult "\n
# initialize MACsec API on usbport 0"

set devp [initialize_macsec_api 0]
if {$devp == 0} {
	puts $hResult "Initialization of MACSec failed."
	return -1
}

puts $hResult "\n
# initialize MAC and MACsec module"
macsec_start $devp 2

puts $hResult "\n
###########################################################
# setup per-port Common config
###########################################################"
set conf_offset 0;set def_vlan 0;set mc_high 0; set mc_low_msk 0;set trail_sz 0;
set cmn_cfg [new_cmn_cfg_t $conf_offset $def_vlan $mc_high $mc_low_msk $trail_sz]
set cmn_port1 "0 1"
foreach port $cmn_port1 {
  puts $hResult "port conf_offset def_vlan mc_high mc_low_msk"
  puts $hResult "$port $conf_offset $def_vlan $mc_high $mc_low_msk"
  set_dev_ctrl_port_num $devp $port
  msec_port_cmn_cfg $devp $port $cmn_cfg
}
set cmn_port2 "2 3"
set conf_offset 30
foreach port $cmn_port1 {
  puts $hResult "port conf_offset def_vlan mc_high mc_low_msk"
  puts $hResult "$port $conf_offset $def_vlan $mc_high $mc_low_msk"
  set_dev_ctrl_port_num $devp $port
  set_cmn_cfg_offset $cmn_cfg $conf_offset
  msec_port_cmn_cfg $devp $port $cmn_cfg
}

puts $hResult "\n
###########################################################
# Entry 0
# setup egress on port 0 & 2
###########################################################"

set ent_num 0
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE00
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xABCDEF01FEDCBA10
set tci 0x2C
set pn 1

set key [create_u8_arr 16]
set keyStr "0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn"
puts $hResult "$ent_num $sci $pn $tci"
puts -nonewline $hResult "keyStr: 0X"
foreach key_fld $keyStr {
  puts -nonewline $hResult [format "%02X" $key_fld]
}
puts $hResult "\n"
flush $hResult

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t

puts $hResult "\n
###########################################################
# Entry 0
# setup Ingress on port 1 & 3
###########################################################"

# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup ICT
set sci 0
set tci 0
set pn 0
# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ICT:"
puts $hResult "ent_num: sci 		tci pn"
puts $hResult "$ent_num: $sci $pn $tci"
puts -nonewline $hResult "keyStr:0x"
foreach keyBt $keyStr {
  puts -nonewline $hResult [format "%02X" $keyBt]
}
puts $hResult "\n"
flush $hResult


# setup ctrl port_num
set iports "1 3"
set retain_hdr 1
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  # set port3 to retain encryption header
  if {$iport == 3} {
    msec_port_set_igr_retain_hdr $devp $iport $retain_hdr
  }
  print_port_igr_hkey $hResult $devp $ent_num
  # setup Replay memory table to 1
  set nxtpn 1
  msec_port_set_nxtpn_ent $devp $iport $ent_num $nxtpn
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 1
# setup egress on port 0 & 2
###########################################################"
set ent_num 1
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE01
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEF01
set tci 0x2D
set pn 1

set key [create_u8_arr 16]
set keyStr "0x12 0x46 0x85 0x79 0x65 0x89 0x45 0x21 0x69 0x86 0x45 0x47 0x44 0x41 0x25 0x69"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn"
puts $hResult "$ent_num $sci $pn $tci"
foreach key_fld $keyStr {
	puts -nonewline $hResult [format "%02X" $key_fld]
}
puts $hResult "\n"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t


puts $hResult "\n
###########################################################
# Entry 1
# setup Ingress entry 1 on port 1 & 3
###########################################################"
# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
  # setup Replay memory table to 1
  set nxtpn 1
  msec_port_set_nxtpn_ent $devp $iport $ent_num $nxtpn
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t


puts $hResult "\n
###########################################################
# Entry 2
# setup egress on port 0 & 2
###########################################################"

set ent_num 2
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE02
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xABCDEF01FEDCBA22
set tci 0x2E
set pn 1

set key [create_u8_arr 16]
set keyStr "0x14 0x47 0x58 0x69 0x45 0x21 0x45 0x21 0x78 0x74 0x69 0x85 0x22 0x36 0x54 0x78"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t

puts $hResult "\n
###########################################################
# Entry 2
# setup Ingress on port 1 & 3
###########################################################"

# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use same key as EGR, sci, tci, pn fields are not valid for ingress
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
  # setup Replay memory table to 1
  set nxtpn 1
  msec_port_set_nxtpn_ent $devp $iport $ent_num $nxtpn
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 3
# setup egress on port 0 & 2
###########################################################"
set ent_num 3
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE03
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEF12
set tci 0x2F
set pn 0xFFFFFF00

set key [create_u8_arr 16]
set keyStr "0xAA 0xBB 0xCC 0xDD 0xEE 0xFF 0x99 0x88 0xAA 0xBB 0xCC 0xDD 0xEE 0xFF 0x99 0x88"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t


puts $hResult "\n
###########################################################
# Entry 3
# setup Ingress on port 1 & 3
###########################################################"
# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use different key from EGR, to generate ICV failure case
# sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 4
# setup egress on port 0 & 2
###########################################################"

set ent_num 4
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE04
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEF39
set tci 0x2C
set pn 0xFFFFFF00

set key [create_u8_arr 16]
set keyStr "0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98 0xFE 0xDC 0xBA 0x98"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t

puts $hResult "\n
###########################################################
# Entry 4
# setup Ingress on port 1 & 3
###########################################################"

# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use different key from EGR, to generate ICV failure case
# sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
  
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 5
# setup egress on port 0 & 2
###########################################################"
set ent_num 5
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE05
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEF46
set tci 0x2D
set pn 0xFFFFFF00

set key [create_u8_arr 16]
set keyStr "0x12 0x45 0x36 0x98 0x85 0x47 0x63 0x25 0x96 0x45 0x31 0x25 0x24 0x86 0x97 0x31"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t


puts $hResult "\n
###########################################################
# Entry 5
# setup Ingress on port 1 & 3
###########################################################"
# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use different key from EGR, to generate ICV failure case
# sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 6
# setup egress on port 0 & 2
###########################################################"

set ent_num 6
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE06
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEF05
set tci 0x2E
set pn 0xFFFFFF00

set key [create_u8_arr 16]
set keyStr "0x11 0x41 0x55 0x97 0x47 0x46 0x99 0x31 0xFE 0xDE 0xBA 0xBA 0xDD 0xEF 0xFE 0xDA"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t

puts $hResult "\n
###########################################################
# Entry 6
# setup Ingress on port 1 & 3
###########################################################"

# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use different key from EGR, to generate ICV failure case
# sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t

puts $hResult "\n
###########################################################
# Entry 7
# setup egress on port 0 & 2
###########################################################"
set ent_num 7
# setup ELU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 1
set en_vlan 0
set en_et 0
set en_scian 0
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ELU match enables:"
puts $hResult "ent_num en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ELU match fields
set da_fld 0
set sa_fld 0xAABBCCDDEE07
set vlan_fld 0
set et_fld 0
set sci_fld 0
set tci_fld 0
set tci_msk_fld 0
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ELU match fields:"
puts $hResult "ent_num	da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num	$da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"


# setup ELU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ELU action:"
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set elk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup Lookup Table as configured above."

# setup ECT
# The key array and ct are shared structures for egress and ingress,
# it is created here and deleted later.
set sci 0xFEDCBA10ABCDEFAB
set tci 0x2F
set pn 0xFFFFFF00

set key [create_u8_arr 16]
set keyStr "0xAB 0xAC 0xDD 0xEC 0xFE 0xEC 0xCA 0xDB 0x47 0x45 0x69 0x85 0x21 0x45 0x47 0x86"
set i 0
foreach key_fld $keyStr {
  set_u8_arr_fld $key $i $key_fld
  incr i
}
set ect [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup ECT:"
puts $hResult "Ent_num sci tci pn key"
puts $hResult "$ent_num $sci $pn $tci $key"

# Setup EGR entry 
# setup ctrl port_num
set eports "0 2"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  msec_port_set_egr_entry $devp $eport $ent_num $elk_t $ect
  print_port_egr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup Egress port $eports entry $ent_num as configured above."

# release memory for ct, elk_t
delete_ctx_t $ect
delete_lkup_t $elk_t


puts $hResult "\n
###########################################################
# Entry 7
# setup Ingress on port 1 & 3
###########################################################"
# setup ILU match enables
set en_def 0
set en_mc 0
set en_da 0
set en_sa 0
set en_vlan 0
set en_et 0
set en_scian 1
set lk_match_en [new_match_en $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian] 
puts $hResult "Setup ILU match enables:"
puts $hResult "ent_num  en_def en_mc en_da en_sa en_vlan en_et en_scian"
puts $hResult "$ent_num  $en_def $en_mc $en_da $en_sa $en_vlan $en_et $en_scian"

# setup ILU match fields
set da_fld 0
set sa_fld 0
set vlan_fld 0
set et_fld 0
set sci_fld $sci
set tci_fld $tci
set tci_msk_fld 0xFF
set lk_match_fld [new_match_fld $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld]
puts $hResult "Setup ILU match fields:"
puts $hResult "ent_num  da_fld sa_fld vlan_fld et_fld sci_fld tci_fld tci_msk_fld"
puts $hResult "$ent_num  $da_fld $sa_fld $vlan_fld $et_fld $sci_fld $tci_fld $tci_msk_fld"

# setup ILU action
set ctx_num $ent_num
set drop 0
set redir 0
set auth_en 1
set enc_en 1
set lk_act_fld [new_act_fld $ctx_num $drop $redir $auth_en $enc_en]
puts $hResult "Setup ILU action field."
puts $hResult "ent_num ctx_num drop redir auth_en enc_en"
puts $hResult "$ent_num $ctx_num $drop $redir $auth_en $enc_en"

# setup LKUP_T
set ilk_t [new_lkup_t $lk_match_en $lk_match_fld $lk_act_fld]
puts $hResult "Setup ILU lookup table as configured above."

# setup CT
set sci 0
set tci 0
set pn 0
# IGR use different key from EGR, to generate ICV failure case
# sci, tci, pn fields are not valid for ingress.
set ict [new_ctx_t $sci $pn $tci $key]
puts $hResult "Setup CT:"
puts $hResult "ent_num: sci 		tci pn key"
puts $hResult "$ent_num: $sci $pn $tci $key"


# setup ctrl port_num
set iports "1 3"
foreach iport $iports {
  set_dev_ctrl_port_num $devp $iport

  # Setup IGR entry 
  msec_port_set_igr_entry $devp $iport $ent_num $ilk_t $ict
  print_port_igr_hkey $hResult $devp $ent_num
}
puts $hResult "Setup port $iports IGR entry $ent_num as configured above."

# release memory for key, ct, 
delete_u8_arr $key
delete_ctx_t $ict
delete_lkup_t $ilk_t
puts $hResult "\n
####################################################################
# send traffic from N2X to port0, 2
# receive traffic from N2X connected to port1, 3
# compare frame received shall be same as frame sent
# setup WIRE side loopback between port0 and port1, port2 and port3
# Check statistics, expect frames hit entry0~2 being processed normally
# frames entry3~7 have PN saturation after first 256 frames
####################################################################"
set portLabelList "101/1 101/2 101/3 101/4"
set portList "1 2 3 4"
set startmac "AA:BB:CC:DD:EE:00"
set dmac "FE:ED:D0:0D:FF:A0"
set incrCnt 8
set bstCnt 4000
set rc 0
global logFile

set encCnt [expr $bstCnt / $incrCnt]
foreach eport $eports iport $iports {
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  for {set i 0} {$i < $incrCnt} {incr i} {
    set smac [incrMacStr $startmac $i]
    puts $hResult "send bursty frames to port $eport, receive on port $iport."
    puts $hResult "srcmac: $smac, destmac: $dmac"
    send_bursty_consecutive_frames $argv0 $portList $sport $dport  $smac $dmac $incrCnt $bstCnt
    after 100 
    dump_log_details $logFile $hResult
  }
}

puts $hResult "\n
###########################################################
# Check statistic counters on DUT
###########################################################"
dump_print_macsec_statistics $hResult $devp
foreach eport $eports iport $iports {
  set egr_hits [get_macsec_stats_by_range $eport MX_SEC_EGR_HIT_E0 8]
  puts $hResult "port $eport EGR_HIT E0~8: $egr_hits"
  set idx [lsearch $egr_hits 0]
  if {$idx != -1} {
      puts $hResult [format "EGR_HIT_E%d = 0, incorrect." $idx]
      set rc -1
  } else {
      puts $hResult "egr_hit is incremented. correct."
  }

  set egr_encs [get_macsec_stats_by_range $eport MX_SEC_EGR_PKT_ENC_E0 8]
  set igr_oks [get_macsec_stats_by_range $iport MX_SEC_IGR_OK_E0 8]
  puts $hResult "port $eport MX_SEC_EGR_PKT_ENC_E0~7 $egr_encs"
  puts $hResult "port $iport MX_SEC_IGR_OK_E0~7 $igr_oks"
  set i 0
  foreach egr_enc $egr_encs igr_ok $igr_oks {    
    if {$egr_enc == 0 } {
      puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = %d, incorrect." $i $egr_enc]
      set rc -1
    } elseif {$i<=2 && $egr_enc == $igr_ok } {
      puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = MX_SEC_IGR_OK_E%d = %d, correct." $i $i $egr_enc]
    } elseif {$i>=3} {
      # check the number of encrypted packet shall be 256 for each entry
      if {$egr_enc != 256} {
        puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = %d is not equal to 256, incorrect." $i $egr_enc]
	set rc -1
      } elseif {$egr_enc != $igr_ok} {
        puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = %d is not equal to MX_SEC_IGR_OK_E%d = %d, incorrect." $i $egr_enc $i $igr_ok]
	set rc -1
      }
    }
    incr i
  }
}

puts $hResult "\n
# Set PN to 1 for each entry in ECT. observe normal functionality."
set pn 1
foreach eport $eports iport $iports {
  set_dev_ctrl_port_num $devp $eport
  for {set ent_num 0} {$ent_num < 8} {incr ent_num} {
    msec_port_set_ect_entry $devp $eport $ent_num $ect
    #port_set_ect_pn $devp $ent_num $pn
  }
  set_dev_ctrl_port_num $devp $iport
  for {set ent_num 0} {$ent_num < 8} {incr ent_num} {
    msec_port_set_nxtpn_ent $devp $eport $ent_num $pn
  }
}

puts $hResult "\n
After configure PN,"
foreach eport $eports {
  set_dev_ctrl_port_num $devp $eport
  for {set ent_num 0} {$ent_num < 8} {incr ent_num} {
    set pn_get [port_get_ect_pn $devp $ent_num]
    puts $hResult [format "port%d ent_num%d pn = %d" $eport $ent_num $pn_get]
  }
}

foreach eport $eports iport $iports {
  # convert to 1-based port number for N2X
  set sport [expr $eport + 1]
  set dport [expr $iport + 1]
  for {set i 0} {$i < $incrCnt} {incr i} {
    set smac [incrMacStr $startmac $i]
    puts $hResult "send bursty frames to port $eport, receive on port $iport."
    puts $hResult "srcmac: $smac, destmac: $dmac"
    send_bursty_consecutive_frames $argv0 $portList $sport $dport  $smac $dmac $incrCnt $bstCnt
    after 100 
    dump_log_details $logFile $hResult
  }
}

puts $hResult "\n
###########################################################
# Check statistic counters on DUT
###########################################################"
dump_print_macsec_statistics $hResult $devp
foreach eport $eports iport $iports {
  set egr_hits [get_macsec_stats_by_range $eport MX_SEC_EGR_HIT_E0 8]
  puts $hResult "port $eport EGR_HIT E0~8: $egr_hits"
  set idx [lsearch $egr_hits 0]
  if {$idx  != -1} {
      puts $hResult [format "EGR_HIT_E%d = 0, incorrect." $idx]
      set rc -1
  } else {
      puts $hResult "egr_hit is incremented. correct."
  }
  set egr_encs [get_macsec_stats_by_range $eport MX_SEC_EGR_PKT_ENC_E0 8]
  set igr_oks [get_macsec_stats_by_range $iport MX_SEC_IGR_OK_E0 8]
  puts $hResult "port $eport MX_SEC_EGR_PKT_ENC_E0~7 $egr_encs"
  puts $hResult "port $iport MX_SEC_IGR_OK_E0~7 $igr_oks"
  set i 0
  foreach egr_enc $egr_encs igr_ok $igr_oks {    
    if {$egr_enc == 0 } {
      puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = %d, incorrect." $i $egr_enc]
      set rc -1
    } elseif {$igr_ok == 0} {
      puts $hResult [format "MX_SEC_IGR_OK_E%d = %d, incorrect." $i $igr_ok]
      set rc -1
    } elseif { $egr_enc == $igr_ok } {
      puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = MX_SEC_IGR_OK_E%d = %d, correct." $i $i $egr_enc]
    } else {
      puts $hResult [format "MX_SEC_EGR_PKT_ENC_E%d = %d is not equal MX_SEC_IGR_OK_E%d = %d, incorrect." $i $egr_enc $i $igr_ok]
      set rc -1
    }
    incr i
  }
}

puts $hResult "\n
###########################################################
# Check and print test result
###########################################################"
if {$rc == 0} {
  puts $hResult "Test pass."
} else {
  puts $hResult "Test fail."
}

