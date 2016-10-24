require 'version_sorter'

ready "VersionSorter" do
  before do
    @versions = File.readlines(File.expand_path('../tags.txt', __FILE__)).map(&:chomp)
  end

  go ".sort" do
    VersionSorter.sort @versions
  end

  go ".rsort" do
    VersionSorter.rsort @versions
  end
end
