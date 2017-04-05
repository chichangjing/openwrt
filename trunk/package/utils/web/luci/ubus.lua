local M = {}


require "ubus"
require "uloop"
local http = require "luci.http"
local cjson = require "luci.json"


uloop.init()

local conn = ubus.connect()
if not conn then
    error("Failed to connect to ubusd")
end

function M.getPortNum()  
    local status = conn:call("sysinfo", "sysinfo_get", { get_args={"port_num"}})
    local num = status["ret"][1]["port_num"]
    return num
end

function M.getUniqueId()  
    local status = conn:call("sysinfo", "sysinfo_get", { get_args={"unique_id"}})
    local unique = status["ret"][1]["unique_id"]
    return unique
end

function M.getPortType(num)  
    local snd = {}
    local status = {}
    for i=1,num do
        local idx = string.format("port-%d",i)
        snd[i] = {[idx] = {"type"}}
        status[i] = conn:call("port_config", "port_config_get", { get_args={snd[i]}})
    end
    return status
end

function M.get_args(path,method,message,bool) 
    local status = {}
    if bool == 1 then
        for i,j in pairs(message) do
             status[i] = conn:call(path, method, { get_args={j}})
        end
    else
         status = conn:call(path, method, { get_args=message})
    end
    return status
end

function M.set_args(path,method,message,bool) 
    local status = {}
    if bool == 1 then
        for i,j in pairs(message) do
             status[i] = conn:call(path, method, { set_args={j}})
        end
    else
         status = conn:call(path, method, { set_args={message}})
    end
    return status
end
function M.clear_args(path,method,message) 
    local  status = conn:call(path, method, { clear_args={message}})
    return status
end
return M  
