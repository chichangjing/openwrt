# A sa
python ../../scripts/make_type_wrapper.py \
	-i ../../data/type_info_file_dx \
	-C /home/serg/cpss \
	-p tgf_manual_types.lst \
	-o wraplTgfTypeWrappers.c \
	`cat tgf_automatic_types.lst`
