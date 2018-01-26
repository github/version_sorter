require "helix_runtime"

begin
  require "version_sorter/native"
rescue LoadError
  warn "Unable to lead version_sorter/native. Please run `rake build`"
end
