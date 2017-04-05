#!/bin/sh
# ddtcl.tcl - a simple unix like 'dd' command in TCL.
#
#  01b,07nov99,rkw  Fixed a bug between versions of TCL.
#  01a,07nov99,rkw  Created.
# \
exec tclsh8.3 "$0" "$@"

# Usage notes:
#  dd if=file1 of=file2 skip=10
#    Read file1, write to file2 and skip the first 512 * 10 bytes of file1. 
#  dd if=file1 of=file2 skip=10 bs=1024
#    Read file1, write to file2 and skip the first 1024 * 10 bytes of file1. 
#  dd if=file1 of=file2 skip=10 count=5
#    Read file1, write to file2 and skip the first 512 * 10 bytes of file1
#    and copy to file2 512 * 10 bytes of file1.
#  dd if=file1 of=file2 skip=10 seek=5 count=1
#    Read file1, write to file2 and skip the first 512 * 10 bytes of file1
#    and copy 512 * 10 bytes to file2 starting at offset 512 * 5 bytes
#    for 1 block of 512 bytes.

# Copyright 1998-2000 Wind River Systems, Inc.


append ebcdic2ascii "0000 0001 0002 0003 0234 0011 0206 0177 "
append ebcdic2ascii "0227 0215 0216 0013 0014 0015 0016 0017 "
append ebcdic2ascii "0020 0021 0022 0023 0235 0205 0010 0207 "
append ebcdic2ascii "0030 0031 0222 0217 0034 0035 0036 0037 "
append ebcdic2ascii "0200 0201 0202 0203 0204 0012 0027 0033 "
append ebcdic2ascii "0210 0211 0212 0213 0214 0005 0006 0007 "
append ebcdic2ascii "0220 0221 0026 0223 0224 0225 0226 0004 "
append ebcdic2ascii "0230 0231 0232 0233 0024 0025 0236 0032 "
append ebcdic2ascii "0040 0240 0241 0242 0243 0244 0245 0246 "
append ebcdic2ascii "0247 0250 0133 0056 0074 0050 0053 0041 "
append ebcdic2ascii "0046 0251 0252 0253 0254 0255 0256 0257 "
append ebcdic2ascii "0260 0261 0135 0044 0052 0051 0073 0136 "
append ebcdic2ascii "0055 0057 0262 0263 0264 0265 0266 0267 "
append ebcdic2ascii "0270 0271 0174 0054 0045 0137 0076 0077 "
append ebcdic2ascii "0272 0273 0274 0275 0276 0277 0300 0301 "
append ebcdic2ascii "0302 0140 0072 0043 0100 0047 0075 0042 "
append ebcdic2ascii "0303 0141 0142 0143 0144 0145 0146 0147 "
append ebcdic2ascii "0150 0151 0304 0305 0306 0307 0310 0311 "
append ebcdic2ascii "0312 0152 0153 0154 0155 0156 0157 0160 "
append ebcdic2ascii "0161 0162 0313 0314 0315 0316 0317 0320 "
append ebcdic2ascii "0321 0176 0163 0164 0165 0166 0167 0170 "
append ebcdic2ascii "0171 0172 0322 0323 0324 0325 0326 0327 "
append ebcdic2ascii "0330 0331 0332 0333 0334 0335 0336 0337 "
append ebcdic2ascii "0340 0341 0342 0343 0344 0345 0346 0347 "
append ebcdic2ascii "0173 0101 0102 0103 0104 0105 0106 0107 "
append ebcdic2ascii "0110 0111 0350 0351 0352 0353 0354 0355 "
append ebcdic2ascii "0175 0112 0113 0114 0115 0116 0117 0120 "
append ebcdic2ascii "0121 0122 0356 0357 0360 0361 0362 0363 "
append ebcdic2ascii "0134 0237 0123 0124 0125 0126 0127 0130 "
append ebcdic2ascii "0131 0132 0364 0365 0366 0367 0370 0371 "
append ebcdic2ascii "0060 0061 0062 0063 0064 0065 0066 0067 "
append ebcdic2ascii "0070 0071 0372 0373 0374 0375 0376 0377 "

append ascii2ebcdic "0000 0001 0002 0003 0067 0055 0056 0057 "
append ascii2ebcdic "0026 0005 0045 0013 0014 0015 0016 0017 "
append ascii2ebcdic "0020 0021 0022 0023 0074 0075 0062 0046 "
append ascii2ebcdic "0030 0031 0077 0047 0034 0035 0036 0037 "
append ascii2ebcdic "0100 0117 0177 0173 0133 0154 0120 0175 "
append ascii2ebcdic "0115 0135 0134 0116 0153 0140 0113 0141 "
append ascii2ebcdic "0360 0361 0362 0363 0364 0365 0366 0367 "
append ascii2ebcdic "0370 0371 0172 0136 0114 0176 0156 0157 "
append ascii2ebcdic "0174 0301 0302 0303 0304 0305 0306 0307 "
append ascii2ebcdic "0310 0311 0321 0322 0323 0324 0325 0326 "
append ascii2ebcdic "0327 0330 0331 0342 0343 0344 0345 0346 "
append ascii2ebcdic "0347 0350 0351 0112 0340 0132 0137 0155 "
append ascii2ebcdic "0171 0201 0202 0203 0204 0205 0206 0207 "
append ascii2ebcdic "0210 0211 0221 0222 0223 0224 0225 0226 "
append ascii2ebcdic "0227 0230 0231 0242 0243 0244 0245 0246 "
append ascii2ebcdic "0247 0250 0251 0300 0152 0320 0241 0007 "
append ascii2ebcdic "0040 0041 0042 0043 0044 0025 0006 0027 "
append ascii2ebcdic "0050 0051 0052 0053 0054 0011 0012 0033 "
append ascii2ebcdic "0060 0061 0032 0063 0064 0065 0066 0010 "
append ascii2ebcdic "0070 0071 0072 0073 0004 0024 0076 0341 "
append ascii2ebcdic "0101 0102 0103 0104 0105 0106 0107 0110 "
append ascii2ebcdic "0111 0121 0122 0123 0124 0125 0126 0127 "
append ascii2ebcdic "0130 0131 0142 0143 0144 0145 0146 0147 "
append ascii2ebcdic "0150 0151 0160 0161 0162 0163 0164 0165 "
append ascii2ebcdic "0166 0167 0170 0200 0212 0213 0214 0215 "
append ascii2ebcdic "0216 0217 0220 0232 0233 0234 0235 0236 "
append ascii2ebcdic "0237 0240 0252 0253 0254 0255 0256 0257 "
append ascii2ebcdic "0260 0261 0262 0263 0264 0265 0266 0267 "
append ascii2ebcdic "0270 0271 0272 0273 0274 0275 0276 0277 "
append ascii2ebcdic "0312 0313 0314 0315 0316 0317 0332 0333 "
append ascii2ebcdic "0334 0335 0336 0337 0352 0353 0354 0355 "
append ascii2ebcdic "0356 0357 0372 0373 0374 0375 0376 0377 "

# set some default information.
set	obsize      0
set	ibsize      0
set	bsize       0
set	skipn	    0
set	seekn	    0
set	countn	    0
set	countw	    0
set seeksize    0
set skipsize    0
set notrunc     0
set ascii       0
set ebcdic      0
set	kbytes	    1024
set wordsize    2
set	mbytes	    [expr 1024 * 1024];         # 1048576 bytes
set gigabytes   [expr 1024 * $mbytes];      # 1073741824 bytes
set bigsize     [expr $mbytes * 16];        # 16777216 bytes

# The usage line.
set usage "[file tail $argv0] if=file1 of=file2 \[operands ...\]\n"
append usage "    bs=n\[b,k,m,g,w\] ibs=n\[b,k,m,g,w\] obs=n\[b,k,m,g,w\]\n"
append usage "    skip=n count=n seek=n conv=\[ascii,ebcdic,notrunc\]\n"
append usage "    (see UNIX dd command for more information.)"

# Did they put in at lest two arguments.
if {$argc < 2} {
	puts "Usage: $usage"
	exit 1
}

# Find the if= argument, if not found then exit.
set nn [lsearch -glob $argv "if=*"]
if {$nn == -1} {
	puts "Usage: $usage"
	exit 1
} else {
	set infile [lindex [split [lindex $argv $nn] "="] 1]
}

# Find the of= argument, if not found then exit.
set nn [lsearch -glob $argv "of=*"]
if {$nn == -1} {
	puts "Usage: $usage"
	exit 1
} else {
	set outfile [lindex [split [lindex $argv $nn] "="] 1]
}

proc translateSize {s nn} {
    # get the size argument on the right side of the equal sign.
	set n [lindex [split [lindex $s $nn] "="] 1]

    # see if the last character is a letter, not a number.
	set key [string tolower [string index $n [expr [string length $n] - 1]]]

	switch -exact -- $key {
		"b" {
            # bytes (1 octet)
			set bsize [string trim $n "b"]
		}
		"k" {
            # k-bytes 1024
			set bsize [string trim $n "b"]
			set bsize [expr $bsize * $kbytes]
		}
		"m" {
            # mega-byte (1048576 octets) (1M)
			set bsize [string trim $n "m"]
			set bsize [expr $bsize * $mbytes]
		}
        "g" {
            # giga-byte (1073741824 octets) (1G)
			set bsize [string trim $n "g"]
			set bsize [expr $bsize * $gigabytes]
        }
        "w" {
            # number of bytes/octets in a integer (2 cotets)
			set bsize [string trim $n "g"]
			set bsize [expr $bsize * $wordsize]
        }
		default {
			set bsize $n
		}
	}

    return $bsize
}

# get ibs= argument and convert into bytes if need be.
set nn [lsearch -glob $argv "ibs=*"]
if {$nn != -1} {
    set ibsize [translateSize $argv $nn]
}

# get obs= argument and convert into bytes if need be.
set nn [lsearch -glob $argv "obs=*"]
if {$nn != -1} {
    set obsize [translateSize $argv $nn]
}

# get bs= argument and convert into bytes if need be.
# This overrides the ibs= and obs= arguments. 
set nn [lsearch -glob $argv "bs=*"]
if {$nn != -1} {
    set bsize [translateSize $argv $nn]
    set obsize $bsize
    set ibsize $bsize
}

if {$ibsize == 0} {
    set ibsize 512
}

if {$obsize == 0} {
    set obsize 512
}

# get skip= argument.
set nn [lsearch -glob $argv "skip=*"]
if {$nn != -1} {
	set skipn [lindex [split [lindex $argv $nn] "="] 1]
}

# get count= argument.
set nn [lsearch -glob $argv "count=*"]
if {$nn != -1} {
	set countn [lindex [split [lindex $argv $nn] "="] 1]
}

# get seek= argument.
set nn [lsearch -glob $argv "seek=*"]
if {$nn != -1} {
	set seekn [lindex [split [lindex $argv $nn] "="] 1]
}

# get conv= argument.
set nn [lsearch -glob $argv "conv=*"]
if {$nn != -1} {
	set f [string tolower [lindex [split [lindex $argv $nn] "="] 1]]

    foreach x [split $f ","] {
        switch -exact $x {
            "ascii" {
                set ascii 1
            }
            "ebcdic" {
                set ebcdic 1
            }
            "notrunc" {
                set notrunc 1
            }
            default {
                puts "Unknown option: $x"
                exit 1
            }
        }
    }
}

######## Open input file. ##########
if {[catch {open $infile r} ifd]} {
	puts $ifd
	exit 1
}

# get size of input file.
set filesize [file size $infile]

# Calculate the number of bytes to skip or seek if any.
set skipsize [expr $ibsize * $skipn]
set seeksize [expr $obsize * $seekn]

# if bytes to skip is larger than orginal file size then error off.
if {$skipsize > $filesize} {
	puts "Blocks to skip is greater than file size."
	exit 1
}

# Over write output file.
if {$notrunc > 0} {
    if {[catch {open $outfile a} ofd]} {
        puts $ofd
        exit 1
    }
} else {
    if {[catch {open $outfile w} ofd]} {
        puts $ofd
        exit 1
    }
}

# Make sure that we read and write in binary mode.
fconfigure $ifd -translation binary
fconfigure $ofd -translation binary


if {$skipn > 0} {
    if {[catch {seek $ifd $skipsize} r]} {
        puts $r
        exit
    }
#    for {set i 0} {$i < $skipn} {incr i} {
#        # Throw away the first n block of data.
#        set data [read $ifd $ibsize]
#    }
}

if {$seekn > 0} {
    if {[catch {seek $ofd $seeksize} r]} {
        puts $r
        exit
    }
}

if {$countn > 0} {
    # write only countn blocks to the output file.
    for {set i 0} {$i < $countn} {incr i} {
        puts -nonewline $ofd [read $ifd $ibsize]
        if {[eof $ifd] == 1} {
            break
        }
    }
    set countw $i
} else {
    # write the rest of the file to the output file.
    while {[eof $ifd] != 1} {
        puts -nonewline $ofd [read $ifd $ibsize]
        incr countw
    }
}

close $ifd
close $ofd

# puts "Bytes written: $filesize - $skipsize = [expr $filesize - $skipsize]" 
if {$countw > 0} {
    puts "    Blocks written: $countw"
}

exit 0
