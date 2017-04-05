#!/bin/ash  
  
statusFile="/usr/lib/lua/luci/controller/admin/status.lua"  
statusBakFile="/usr/lib/lua/luci/controller/admin/status.lua.bak"  
  
systemFile="/usr/lib/lua/luci/controller/admin/system.lua"  
systemBakFile="/usr/lib/lua/luci/controller/admin/system.lua.bak"  

networkFile="/usr/lib/lua/luci/controller/admin/network.lua"  
networkBakFile="/usr/lib/lua/luci/controller/admin/network.lua.bak"  

if [ -f "$statusFile" ]; then  
    mv "$statusFile"  "$statusBakFile"
fi  

if [ -f "$systemFile" ]; then  
    mv "$systemFile"  "$systemBakFile"
fi  

if [ -f "$networkFile" ]; then  
    mv "$networkFile"  "$networkBakFile"
fi  

