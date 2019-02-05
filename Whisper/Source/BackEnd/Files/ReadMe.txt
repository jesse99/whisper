The Files project contains classes that simplify working with files. One thing to note
is that file paths are generally not used. Instead XFileSpec is used to identify files
and XFolderSpec is used to identify directories. Using these classes provides a better
abstraction than, for example, a file path object and works better on the Mac where
using file paths is discouraged.

This package also includes a class called XResource. This data can come from a 
platform resource or from a uniform resource identifier (URI). When an URI is used
to initialize an XResource the XURIResolver singleton is used to load the resource.
XURIResolver traverses a list of XURIFetcher's to find one that can load the URI.
XURIResolver installs file and (platform) resource fetchers by default.