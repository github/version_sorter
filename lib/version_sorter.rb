require "helix_runtime"

begin
  require "version_sorter/native"
rescue LoadError
  warn "Unable to lead version_sorter/native. Please run `rake build`"
end

class VersionSorter
  def self.sort(versions, &blk)
    int_sort_with(versions, :int_sort, &blk)
  end

  def self.rsort(versions, &blk)
    int_sort_with(versions, :int_rsort, &blk)
  end

  def self.int_sort_with(versions, method, &blk)
    bases = if blk
              versions.map(&blk)
            else
              versions
            end
    send(method, bases).map { |i| versions[i] }
  end

  def self.sort!(versions, &blk)
    int_sort_with!(versions, :int_sort, &blk)
  end

  def self.rsort!(versions, &blk)
    int_sort_with!(versions, :int_rsort, &blk)
  end

  def self.int_sort_with!(versions, method, &blk)
    bases = if blk
              versions.map(&blk)
            else
              versions
            end
    indices = send(method, bases)
    origs = versions.dup
    indices.each.with_index do |ix, i|
      versions[i] = origs[ix]
    end
    versions
  end
end
