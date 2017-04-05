------------------------------------------------------------------------------
-- cmdWebServer.lua
--
-- DESCRIPTION:
--      Web server implementation
--
-- DEPENDENCIES:
--     This script is run by cmdWebServer.c:cmdWebServerHttpProto()
--
-- FILE REVISION NUMBER:
--      $Revision: 1$
--
-- COMMENTS:
--      The following data is set before this script run:
--          http.proto      protocol. Fx: HTTP/1.0
--          http.method     query method. Fx: GET, POST
--          http.url        Fx: http://user:pass@host.com:8081/script/path?var1=val1&var2=val2
--          http.uri        Fx: http://user:pass@host.com:8081/script/path
--          http.scheme     Fx: http
--          http.host       Fx: host.com
--          http.port       Fx: 8081
--          http.user       Fx: user
--          http.pass       Fx: pass
--          http.path       Fx: script/path
--          http.headers{}  Fx: http.headers={
--                                  { "Accept-Charset", "utf-8" },
--                                  { "Content-length", "1024" }
--                              }
--          http.get_vars   variables passed in URL (method GET)
--                          Fx: http.get_vars={ var1 = "val1", var2 = "val2" }
--          http.post_vars  variables passed in body (method POST)
--                          Fx: http.get_vars={ var1 = "val1", var2 = "val2" }
--          http.cookies{}
--
--      The following functions are defined:
--          http.get_header(
--              header_name
--          )                   lookup header by case insensetive name
--                              Fx: http.get_header("accept-language")
--
--          http.set_responce(
--              resp_code,
--              resp_string
--          )                   Set responce code and responce string
--
--          http.set_header(
--              header_name,
--              header_string,
--              override
--          )                   Set responce header
--
--          http.data(string)   add data
--
--          http.send_header()  After that headers cannot be changed
--
--
--          --------------------------------------------------------------
--          for fs.*() functions see ../lua/luaCLI/cmdLuaFS.c in
--          LUA API Documentation section
--
--      Examples:
--      1. redirect
--          http.set_responce("302", "Moved")
--          http.set_header("Content-Disposition", "http://somewhere.else/")
------------------------------------------------------------------------------

Web_root="/Web"
Web_index_files={
    "index.lua",
    "Index.lua",
    "index.html",
    "Index.html",
    "index.htm",
    "Index.htm"
}
Web_content_types={
    { ".html", "text/html" },
    { ".htm", "text/html" },
    { ".lua", "application/lua" },
    { ".txt", "text/plain" },
    { ".jpg", "image/jpeg" },
    { ".gif", "image/gif" },
    { ".png", "image/png" }
}

function http.send_file(filename)
    local fd = fs.open(filename)
    if fd == nil then
        return false
    end
    while true do
        local data
        data = fs.read(fd, 1024);
        if data == nil or data == "" then
            break
        end
        http.data(data)
    end
    fs.close(fd)
end

function to_string(val,level)
    if level == nil then
        level = 0
    end
    if val == nil then
        return "nil"
    elseif type(val) == "boolean" then
        if val then
            return "true"
        end
        return "false"
    elseif type(val) == "number" then
        return tostring(val)
    elseif type(val) == "string" then
        return string.format("%q",val)
    elseif type(val) == "table" then
        local s,k,v,not_first,recno
        s = "{"
        not_first = false
        recno = 0
        for k,v in pairs(val) do
            if not_first then s = s .. "," end
            not_first = true
            recno = recno + 1
            s = s .. "\n" .. string.rep("  ",level+1) 
            if type(k) == "string" and string.match(k,"%a[%a%d_]+") == k then
                s = s .. k .. "="
            elseif type(k) == "number" and k == recno then
                
            else
                s = s .. "[" .. to_string(k,level+1) .. "]="
            end
            s = s .. to_string(v,level+1)
        end
        if not_first then
            s = s .. "\n" .. string.rep("  ",level)
        end
        s = s .. "}"
        return s
    elseif type(val) == "function" then
        return "&lt;function&gt;"
    elseif type(val) == "userdata" then
        return "&lt;userdata&gt;"
    elseif type(val) == "thread" then
        return "&lt;thread&gt;"
    else
        return "nil"
    end
end


--print("<html>")
--print("<body>")
--print("hello, world")
--print("<pre>")
--print("http="..to_string(http))
--print("</pre>")
--print("<pre>")
--print("fs="..to_string(fs))
--print("</pre>")

do
 
-- resolve file name to load
local filename
local fname = Web_root .. "/" .. http.path
fname = string.gsub(fname, "//", "/")
if string.sub(fname,1,1) == "/" then
    fname=string.sub(fname,2)
end
if fs.fileexists(fname) then
    http.script_path = fname
else
    if fname ~= "" and string.sub(fname, -1) ~= "/" then
        fname = fname .. "/"
    end
    local i
    for i = 1, #Web_index_files do
        if fs.fileexists(fname .. Web_index_files[i]) then
            http.script_path = fname .. Web_index_files[i]
            break
        end
    end
end

if http.script_path == nil then
    http.set_responce("404", "Not found")
    print("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">")
    print("<HTML><HEAD>")
    print("<TITLE>404 Not Found</TITLE>")
    print("</HEAD><BODY>")
    print("<H1>Not Found</H1>")
    print("The requested URL ".. http.url .. " was not found on this server.<P>")
    print("</BODY></HTML>")
else
    local ctype
    local i
    for i = 1, #Web_content_types do
        local suffix = Web_content_types[i][1]
        if string.sub(http.script_path, -string.len(suffix)) == suffix then
            ctype = Web_content_types[i][2];
            break
        end
    end
    if ctype == "application/lua" then
        require(string.sub(http.script_path,1,-5))
    else
        http.set_header("Content-Type", ctype, true)
        http.send_file(http.script_path)
    end
end

end

--print("</body>")
--print("</html>");
