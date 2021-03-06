--
-- Key-value storage stuff
--

function freeminer.kv_put(key, data)
   freeminer.kv_put_string(key, freeminer.write_json(data))
end

function freeminer.kv_get(key)
   local data = freeminer.kv_get_string(key)
   if data ~= nil then
   	  data = freeminer.parse_json(data)
   end
   return data
end

function freeminer.kv_rename(key1, key2)
   local data = freeminer.kv_get_string(key1)
   freeminer.kv_delete(key1)
   freeminer.kv_put_string(key2, data)
end
