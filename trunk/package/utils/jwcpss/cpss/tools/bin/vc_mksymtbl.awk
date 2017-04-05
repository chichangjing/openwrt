# Generate symbol table for VC libraries
# Usage:
#   1. Create library listings:
#      dubpbin /symbols library1.lib > library.list
#      dubpbin /symbols library2.lib >> library.list
#      ...
#   2. Generate symbol table:
#      awk -f vc_mksymtbl.awk library.list > symtable.c
#
/\(\) *External  *\|/ {
	if (ENVIRON["IS_64BIT_OS"] != "YES") {
		if (match($(NF), "^_[_a-zA-Z][_a-zA-Z0-9]*$") == 1) {
			sym[substr($(NF),2)] = 1
		}
	} else {
		if (match($(NF), "^[a-zA-Z][_a-zA-Z0-9]*$") == 1) {
			sym[$(NF)] = 1
		}
	}
}
END { 
	for (name in sym)
		printf "int %s();\n",name;
	print "struct { const char *name; int (*funcptr)();} __SymbolTable[] = {"
	for (name in sym)
		printf " { \"%s\", %s },\n",name,name
	print " { (void*)0L, (void*)0L }"
	print "};"
}
