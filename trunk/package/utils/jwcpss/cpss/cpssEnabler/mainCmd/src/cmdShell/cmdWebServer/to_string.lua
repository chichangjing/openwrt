
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
        return "<function>"
    elseif type(val) == "userdata" then
        return "<userdata>"
    elseif type(val) == "thread" then
        return "<thread>"
    else
        return "nil"
    end
end
