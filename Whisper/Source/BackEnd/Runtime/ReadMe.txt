The Runtime project contains very low level files that will be used by pretty
much everyone. This includes:

* Some startup code that handles toolbox initialization on the Mac and leak
reporting on both platforms.

* A debugging version of operator new that calls through to malloc (this
includes the usual stuff like header and trailer blocks, maintains statistics,
and uses a stack crawl for leak reporting). This can be turned off by setting
WHISPER_OPERATOR_NEW to 0.

* Low level debugging code. This includes routines for breaking into the debugger,
asserts, and debug printf like routines.

One thing to note is that Runtime.mcp is unlike all the other projects because it
uses an exp file instead of #pragma export to export symbols from the DLL.