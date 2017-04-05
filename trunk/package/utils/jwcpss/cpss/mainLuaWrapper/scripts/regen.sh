#
# A script for manual rebuild of generated files
C=$HOME/cpss
L=$C/mainLuaWrapper
S=$L/scripts
D=$L/data

# all_dxCh_lua_c_type_wrappers.c:
python $S/make_type_wrapper.py \
	-i $D/type_info_file_dx -p $S/already_implemented.lst -X $S/excludelist \
	-C $C -M $S/lua_C_mapping -N -T -F dxCh \
	-o $L/src/dxCh/all_dxCh_lua_c_type_wrappers.c
# all_exMxPm_lua_c_type_wrappers.c
#python $S/make_type_wrapper.py \
#	-i $D/type_info_file_pm -p $S/already_implemented.lst -X $S/excludelist \
#	-C $C -M $S/lua_C_mapping -N -T -F exMxPm \
#	-o $L/src/exMxPm/all_exMxPm_lua_c_type_wrappers.c

# all_common_lua_c_type_wrappers.c
python $S/make_type_wrapper.py \
	-i $D/type_info_file_dx -i $D/type_info_file_pm \
	-p $S/already_implemented.lst -X $S/excludelist \
	-C $C -M $S/lua_C_mapping -N -T -F NONE \
	-o $L/src/generic/all_common_lua_c_type_wrappers.c



# cpssAPI.xml
X=$D/dxCh_xml
python $S/gen_xml_funcdescr.py -i $D/type_info_file_dx -A $D/hiddenAPIs -C $C -F dxCh \
	-o $X/cpssAPI.xml -B $X/bad_s.xml -c \
	-M $X/manual_s.xml -m $S/lua_C_mapping

# cpssAPI_xml_embed_file.c
python $S/cmdFS_RAM_embed_files.py \
    -E _XML -z .xml \
    -S 10000000 \
    -o $X/cpssAPI_xml_embed_file.c \
    -F $X/cpssAPI.xml
