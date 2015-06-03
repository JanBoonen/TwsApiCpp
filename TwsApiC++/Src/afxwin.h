#ifndef _afxwin_h_
#define _afxwin_h_
//----------------------------------------------------------------------------
// Replacement for the windows afxwin.h include file
//----------------------------------------------------------------------------
#define CorrectAfxWinIncluded

// To cut the windows specific paths in EClientSocketBaseImpl.h
#undef _MSC_VER

#endif _afxwin_h_
