# Generate symbol table for VC libraries
# Usage:
#   1. Create library listings:
#      tlib /C library1.lib ,libraryx.list
#      ...
#   2. Generate symbol table:
#      awk -f bc_mksymtbl.awk library.list > symtable.c
#
function do_name(name) {
	if (substr(name,length(name)) == "\r")
		name = substr(name,1,length(name)-1)
	if (substr(name,1,1) != "_")
		return
	if (index(name,"@") != 0)
		return
	if (index(name,":") != 0)
		return
	name = substr(name,2)
	if (name == "")
		return
	if (name == "__SymbolTable")
		return
	sym[name] = 1
}
/^[ \t]/ {
	line = $0
	while ((p = index(line, "(")) != 0 && (e = index(line, ")")) != 0) {
		line = substr(line,1,p-1) "@" substr(line,e+1)
	}
	split(line,a)
	do_name(a[1])
	do_name(a[2])
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
