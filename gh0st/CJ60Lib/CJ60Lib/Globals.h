////////////////////////////////////////////////////////////////
// Copyright 1998 Paul DiLascia
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
#ifndef _GLOBALS_H
#define _GLOBALS_H

#define countof(x)	(sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG

//////////////////
// TRACEFN is a macro that lets you generate indented TRACE output so you
// can see the call stack. To use it:
//
//		SomeFn(...)
//		{
//			TRACEFN("Entering SomeFn...\n");
//			.
//			.
//		}
//
// Now all trace output after TRACEFN will be indented one space, until SomeFn
// returns. You can put TRACEFN in multiple functions to see indented trace
// output. For an example of this, see the HOOK sample program.
//
// NOTE: YOU MUST NOT USE TRACEFN IN A ONE-LINE IF STATEMENT!
// This will fail:
//
// if (foo)
//    TRACEFN(...)
//
// Instead, you must enclose the TRACE in squiggle-brackets
//
// if (foo) {
//		TRACEFN(...)
// }
//
#define TRACEFN CTraceFn __fooble; TRACE
//
// This class implements TRACEFN. Don't ever use directly!
//
class CTraceFn {
private:
	static int	nIndent;				// current indent level
	friend void AFX_CDECL AfxTrace(LPCTSTR lpszFormat, ...);
public:
	CTraceFn()  { nIndent++; }		// constructor bumps indent
	~CTraceFn() { nIndent--; }		// destructor restores it
};

#else // NOT _DEBUG

#define TRACEFN TRACE

#endif // _DEBUG

#endif // _GLOBALS_H
