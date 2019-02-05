The parsing package contains two parsers. The first is XSimpleParser which works
fairly well for light-weight text parsing. When you use XSimpleParser you call
methods like ParseIdentifier and ParseInt32 to extract information from a string.
XParser.h contains a framework for a more industrial strength parser. This parser
builds an in-memory parse tree. XParserTest.cpp has an example showing how XParser.h
can be used to evaluate simple mathematical expressions.

This package also contains a regular expression class and a set of automata classes
that are used by the lexer and the regular expression class.