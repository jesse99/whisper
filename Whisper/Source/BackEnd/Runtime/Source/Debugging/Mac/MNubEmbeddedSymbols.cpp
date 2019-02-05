/*
 *  File:       MNubEmbeddedSymbols.cpp
 *  Summary:   	Embedded Symbol Extraction (This is used by MStackCrawl.cpp)
 *  Written by: Apple Computer
 *
 *  Copyright © 1995-1997 by Apple Computer, Inc., all rights reserved.
 *
 *  Change History (most recent first):	
 *
 *		$Log: MNubEmbeddedSymbols.cpp,v $
 *		Revision 1.3  2001/04/13 07:51:31  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.2  2000/11/09 09:16:31  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include <MNubEmbeddedSymbols.h>

#include <string>

#include <Errors.h>

namespace Whisper {

using namespace std;


//==============================================================================
// Embedded Symbol Extraction (copied from debugger nub)
//==============================================================================

#define kNoErr					noErr
#define PACKET_MAX_DATA_LENGTH	512

// max length of an embedded symbol (longer will be truncated)
#define kMaxNameLength						256
#define kTTblMaxNameLength					256
#define kMaxTracebackNameSearchLength		512



// PowerPC embedded symbol stuff:

#define kMaxPwrPCFnLength	262144
#define kMaxPwrPCFnInstrs	(kMaxPwrPCFnLength >> 2)

typedef struct TracebackTblEndAlt
	{
		ULongWord		unknown;
		ULongWord		fnLength;
		UWord			nameLength;
		char			name[kTTblMaxNameLength];
	} TracebackTblEndAlt;

typedef struct TracebackTblEnd
	{
		ULongWord		fnLength;
		UWord			nameLength;
		char			name[kTTblMaxNameLength];
	} TracebackTblEnd;

typedef struct TracebackTbl
	{
		ULongWord			zero;
		char				version;
		char				language;
		char				flags[6];
		union {
		TracebackTblEnd		end;	// if flags[4] == 0
		TracebackTblEndAlt	endAlt;	// if flags[4] > 0
		} u;
	} TracebackTbl;


static TracebackTbl *FindTracebackTbl (void *addr, TracebackTbl *ttbl, ReadMemFn readMemCallback, va_list args);



// 68K embedded symbol stuff:

#define kMax68KFnLength		32768

#define kLINKA6				0x4E56
#define kJMPA0				0x4ED0
#define kRTS				0x4E75
#define kRTD				0x4E74



static UByte	*GetProcStart (UByte *addressOfFnEnd, ReadMemFn readMemCallback, va_list args);
static UByte	*FindNextModule (UByte *start, UByte *limit, Boolean *hasName, ReadMemFn readMemCallback, va_list args);
static Boolean	ValidName (UByte *name, ReadMemFn readMemCallback, va_list args);
static void		GetModuleName (UByte *address, char *name, UByte **dataStart, UByte **dataEnd, ReadMemFn readMemCallback, va_list args);
static Boolean	FindReturn (UByte *start, UByte *limit, UByte **afterReturn, ReadMemFn readMemCallback, va_list args);
static Boolean	LegalSymbolChar (short ch);
static Boolean	LegalTargetChar (short ch);

#pragma segment CodeTracking

//
// Tries to find a traceback table for the routine in which addr is presumed to be.
// If successful, returns the name of the routine in name and the addresses of the
// beginning and end of the routine in fnBegin and fnEnd.  Else scans forward for
// a blr instruction and backward for an mflr instruction to guesstimate fnBegin
// and fnEnd, and leaves name empty.
//
OSErr LookupPowerPCSym (TargetAddress addr, char *name, TargetAddress *fnBegin, TargetAddress *fnEnd,
		ReadMemFn readMemCallback, ...)
{
	va_list			args;
	TracebackTbl	ttbl;
	TracebackTbl	*where;
	char			*namePtr;
	UWord			nameLength;
	ULongWord		codeStart, fnLength;
	ULongWord		instr;
	long			i, offset;

	va_start( args, readMemCallback );
	
	#define	kmflrInstr	0x7C0802A6
	#define	kblrInstr	0x4E800020
	
	*fnBegin = addr;
	*fnEnd = addr;

	where = FindTracebackTbl ((void*) addr, &ttbl, readMemCallback, args);

	if (where)
	{
		fnLength = ttbl.flags[4] ? ttbl.u.endAlt.fnLength : ttbl.u.end.fnLength;
		codeStart = (ULongWord) where - fnLength;
		offset = (long) (addr - codeStart);
		if (offset >= 0)
		{
			namePtr = ttbl.flags[4] ? ttbl.u.endAlt.name : ttbl.u.end.name;
			nameLength = ttbl.flags[4] ? ttbl.u.endAlt.nameLength : ttbl.u.end.nameLength;
			if (nameLength > kMaxNameLength - 2)
				// leave room for leading length byte and terminating null
				nameLength = kMaxNameLength - 2;
			strncpy (&name[1], namePtr, nameLength);
			name[0] = (char) nameLength;			// now it's a Pascal string
			name[nameLength + 1] = 0;		// terminating null to make it a TProtocolString
			if (nameLength & 1)				// do we need a pad byte?
				name[nameLength + 2] = 0;	// add one
			*fnBegin = (TargetAddress) codeStart;
			*fnEnd = (TargetAddress) where;
			return kNoErr;
		}
	}

	// couldn't find a traceback, so no embedded name, but we'll scan for the fn boundaries anyway
	for (i = 0; i < kMaxPwrPCFnInstrs; ++i)
	{
		if ((*readMemCallback) ((void*) *fnBegin, 4, &instr, args) != kNoErr)
			return kSymbolNotFound;
		if (instr == kmflrInstr)
			break;
		else if (instr == kblrInstr)
			{
			// probably a leaf routine, and we've backed up into the preceding fn
			*fnBegin += 4;
			break;
			}
		*fnBegin -= 4;
	}
	
	if (i == kMaxPwrPCFnInstrs)
	{
		// didn't find the fn begin
		*fnBegin = addr;
		*fnEnd = addr;
		return kSymbolNotFound;
	}

	// found the fn begin, now try to find the fn end
	for (i = 0; i < kMaxPwrPCFnInstrs; ++i)
	{
		if ((*readMemCallback) ((void*) *fnEnd, 4, &instr, args) != kNoErr)
			return kSymbolNotFound;
		*fnEnd += 4;
		if (instr == kblrInstr)
			break;
	}
	
	if (i == kMaxPwrPCFnInstrs)
	{
		// didn't find the fn end
		*fnBegin = addr;
		*fnEnd = addr;
		return kSymbolNotFound;
	}
	
	return kNoErr;
}


//
// Tries to find a traceback table for the routine in which addr is presumed to be.  If
// successful, returns a pointer to the traceback table and copies its contents into ttbl.
// If not successful, returns NULL.
//
static TracebackTbl *FindTracebackTbl (void *addr, TracebackTbl *ttbl, ReadMemFn readMemCallback, va_list args)
{
	ULongWord		where = (ULongWord) addr;
	ULongWord		i, memLongWord, fnLength;
	char			*name;
	UWord			nameLength;
	
	for (i = 0; i < kMaxPwrPCFnLength; i += 4)
	{
		if ((*readMemCallback) ((void *) where, 4, &memLongWord, args) != kNoErr)
			return NULL;
		if (memLongWord == 0)
			break;
		where += 4;
	}
	
	if (i == kMaxPwrPCFnLength)
		return NULL;

	if ((*readMemCallback) ((void *) where, sizeof(TracebackTbl), ttbl, args) != kNoErr)
		return NULL;
	
	// now for some sanity checks
	fnLength = ttbl->flags[4] ? ttbl->u.endAlt.fnLength : ttbl->u.end.fnLength;
	name = ttbl->flags[4] ? ttbl->u.endAlt.name : ttbl->u.end.name;
	nameLength = ttbl->flags[4] ? ttbl->u.endAlt.nameLength : ttbl->u.end.nameLength;
	if ((fnLength > kMaxPwrPCFnLength) || (nameLength > kMaxTracebackNameSearchLength) || (name[0] < ' ') || (name[0] > '}'))
		return NULL;
	else
		return (TracebackTbl *) where;
}


//
// Tries to find a Macsbug symbol for the routine in which addr is presumed to be.
// If successful, returns the name of the routine in name and the addresses of the
// beginning and end of the routine in fnBegin and fnEnd.
// Name is a "PC" string (a Pascal string with a terminating null).
// fnBegin should normally always be less than or equal to addr, but if addr happens
// to be somewhere strange (like in an embedded symbol itself!), we may actually
// find the next function.
//
OSErr Lookup68KSym (TargetAddress addr, char *name, TargetAddress *fnBegin, TargetAddress *fnEnd,
		ReadMemFn readMemCallback, ...)
{
	va_list			args;
	UByte*			address = (UByte*) addr;
	UByte*			limit = (UByte*) (addr + kMax68KFnLength);
	UByte*			addressOfFnEnd;
	UByte*			codeStart;
	UByte*			ignore1;
	UByte*			ignore2;
	Boolean			hasName;

	va_start( args, readMemCallback );
	
	addressOfFnEnd = FindNextModule (address, limit, &hasName, readMemCallback, args);
	
	if (addressOfFnEnd)
		{
		//	10/18/94 dkk
		//	Code here used to search backwards from the end of the function to find the start of this function.
		//	If the start of the function wasn't before the address we were interested in finding the symbol for
		//	then we searched again from the original address. We then would search forward again for the end of
		//	the function. This seemed like a lot of extra work. I changed the code look for the start of the
		//	function from the original start address, and don't look for the end of function twice.
		//	We've found a module for the address, find the start of the procedure.
		//
		codeStart = GetProcStart (address, readMemCallback, args);

		if (hasName)
			GetModuleName (addressOfFnEnd, name, &ignore1, &ignore2, readMemCallback, args);
		else
			*name = 0;
			
		*fnBegin = (TargetAddress) codeStart;
		*fnEnd = (TargetAddress) addressOfFnEnd;
		return kNoErr;
		}
	
	*fnBegin = addr;
	*fnEnd = addr + kMax68KFnLength;
	return kSymbolNotFound;
}


//	
//	GetProcStart
//
//		Search backwards for the start of the procedure and return its address.
//
//		10/18/94 - dkk
//		This code was pulled from MacsBug, which didn't suffer any overhead hit from reading one word of memory
//		at a time. Since when MacsBug is executing, it always has its own bus error handler installed, there
//		is no need to install a special handler for each memory access. For the nubs, this is not the case.
//		Each memory access require installation and removal of the bus error handler. For this reason, this code
//		was changes to read a buffer at a time, and step through the buffer. In addition the check for RTS,
//		JMP (A0), and RTD were moved to here, even though this duplicateds code in FindNextModule. FindNextModule
//		will also do the check, but another memory access is required to do the check. By checking here first,
//		we save the overhead of that memory access most of the time.

static UByte *GetProcStart (UByte *addressOfFnEnd, ReadMemFn readMemCallback, va_list args)
{
	UWord		instr;
	UByte		*codeStart;
	UByte		*limit = NULL;
	UByte		*prevDataStart;
	char		prevProcName[kMaxNameLength];
	char		memBuffer[PACKET_MAX_DATA_LENGTH];
	short		offset;
	Boolean		hasName;

	if ((ULongWord) addressOfFnEnd >= (ULongWord) kMax68KFnLength)
		limit = addressOfFnEnd - kMax68KFnLength;

	codeStart = addressOfFnEnd - 2 - (PACKET_MAX_DATA_LENGTH - 2);
	
	if ((*readMemCallback) ((void *) codeStart, PACKET_MAX_DATA_LENGTH, memBuffer, args) != kNoErr)
		return NULL;
	offset = PACKET_MAX_DATA_LENGTH - 2;
	instr = (unsigned short) (*(short *) &memBuffer[offset]);

	//	RTD is a 4 byte instruction. RTS and JMP (A0) are 2 byte instructions.
	//
	if ((instr != kRTS) && (instr != kJMPA0))
		offset -= 2;

	//	Step backwards looking for the start of the procedure.
	//
	while (codeStart > limit /* gCurBlock.data */)
		{
		offset -= 2;

		if (offset < 0)
		{
			codeStart -= PACKET_MAX_DATA_LENGTH;
			offset = PACKET_MAX_DATA_LENGTH - 2;
			if ((*readMemCallback) ((void *) codeStart, PACKET_MAX_DATA_LENGTH, memBuffer, args) != kNoErr)
				return NULL;
		}
		instr = (unsigned short) (*(short *) &memBuffer[offset]);

		//	LinkA6 starts a procedure.
		//
		if (instr == kLINKA6)
			return (codeStart + offset);

		if ((instr == kJMPA0) || (instr == kRTS) || (instr == kRTD))
			{
			addressOfFnEnd = FindNextModule (codeStart + offset, codeStart + offset + 2, &hasName, readMemCallback, args);
			if (addressOfFnEnd)
				{
				// Found the previous procedure. Its dataEnd is the same as the codeStart we are looking for.
				//	10/18/94 dkk
				//	This used to call GetModuleName all the time. Can save the overhead of determining the module
				//	name again, by checking hasName returned from FindNextModule.
				//
				if (hasName)
					{
					GetModuleName (addressOfFnEnd, prevProcName, &prevDataStart, &codeStart, readMemCallback, args);
					return (codeStart);
					}
				else
					return (addressOfFnEnd);
				}
			}
		}
	return NULL /* (gCurBlock.data) */;
}


//
//	FindNextModule
//
//		Search for the next module in the address range start to limit-2.  Module names immediately follow either
//		an RTS, RTD (plus offset), or JMP (A0) instruction.  The three formats for legal module names are described
//		in the header comments for ValidName.  If no module name found, return address of end of module where name
//		would have been.  hasName parameter set to indicate whether a name was found.
//
//		return value	-	address of next module
//

static UByte *FindNextModule (UByte *start, UByte *limit, Boolean *hasName, ReadMemFn readMemCallback, va_list args)
{
	UByte *fnEnd;
	UByte *firstFollowingModule = NULL;

	while ((start < limit) && FindReturn (start, limit, &fnEnd, readMemCallback, args))
		{
		// After the call to FindReturn, fnEnd contains the address following the return instruction (either an RTS,
		//	RTD, or JMP (A0)).  This will be the address of procedure name, if there is one.
		//
		if (firstFollowingModule == NULL)
			firstFollowingModule = fnEnd;
		
		if (ValidName (fnEnd, readMemCallback, args))
			{
			// Found a module name, return its address.
			//
			*hasName = true;
			return (fnEnd);
			}
		else
			// Wasn't a valid name, keep looking starting after return instruction.
			//
			start = fnEnd;
		}
	
	*hasName = false;
	return firstFollowingModule;
}


//
//	ValidName
//
//		Checks to see if the name at the pointer one of three valid formats for a module name.
//
//		The three formats are:
//
//			Variable length:	The first byte is in the range $80 to $9F and is a length in the
//									range 0 to $1F. The high order bit must be set. A length of 0
//									implies the second byte is the actual length in the range $01 
//									thru $FF. The length byte(s) and name may be an odd number of
//									bytes. However, the data after the name is word aligned.
//			Fixed length 8:		The first byte is in the range $20 to $7F and is an ASCII character.
//									The high order bit may be set but is not required to be.
//			Fixed length 16:	The first byte is in the range $20 to $7F and is an ASCII character.
//									The high order bit may be set but is not required to be.
//									The high order bit in the second byte is required to be set.
//									This distinguishes the two types of fixed names.
//

static Boolean ValidName (UByte *name, ReadMemFn readMemCallback, va_list args)
{
	char		nameCopy[256];
	UByte		*namePtr = (UByte*) nameCopy;
	UWord		length;
	Boolean		valid;

	if ((*readMemCallback) ((void *) name, 256, nameCopy, args) != kNoErr)
		return false;
	
	valid = true;

	// The high order bit of the first byte of the name is set for variable length names.
	//
	if (*namePtr & 0x80)
		{
		length = (unsigned short) (*namePtr++ & 0x7F);

		// After clearing the high order bit, the first byte should be in the range of 0-1F, for variable
		//	length names.  If the length is 0, the length is in the next byte.  The next byte should not be 0.
		//
		if (length == 0)
			if (*namePtr)
				length = *namePtr++;
			else
				valid = false;
		else
			// Maximum length if in the first byte is 1F.
			//
			if (length > 0x1F)
				valid = false;
		
		if (valid)
			// So far the name is valid, loop through the name checking for the valid symbol characters 
			//	'a'..'z', 'A'..'Z', '0'..'9', '_', '%', '.' (Variable length names cannot have spaces)
			//	10/18/94 dkk
			//	Added check to see that valid was still true. Used to set valid to whatever the last character
			//	in name used to be.
			//
			for (;(length > 0) && valid; --length)
				if (valid)
					valid = LegalSymbolChar ((short) *namePtr++);
		
		}
	else
		{
		//	High order bit in first byte of fixed length name may or may not be set.
		//
		valid = LegalTargetChar ((short) (*namePtr++ & 0x7F));
		if (valid)

			{
			// First character was legal, check the high order bit of the second byte to see whether it is a 8 byte or
			// 16 byte fixed length name.
			//
			if (*namePtr & 0x80)
			
				// 16 byte name.  Check the second character and set the length for the remaining 14 characters.
				//
				{
				valid = LegalTargetChar ((short) (*namePtr++ & 0x7F));
				length = 14;
				}
			else
				// 8 byte name.  Check the second character and set the length for the remaining 6 characters.
				//
				{
				valid = LegalTargetChar ((short) *namePtr++);
				length = 6;
				}
			
			//	10/18/94 dkk
			//	Added check to see that valid was still true. Used to set valid to whatever the last character
			//	in name used to be.
			//
			for (;(length > 0) && valid; --length)
				if (valid)
					valid = LegalTargetChar ((short) *namePtr++);
			}
		}
	return (valid);
}
	

//
//	GetModuleName
//
//   Copy the valid MacsBug module name at address into the name string.
//   Set dataStart to point to the first byte after the name after making sure it is word aligned.
//   Set dataEnd to point to the first byte of the next procedure.
//

static void GetModuleName (UByte *address, char *name, UByte **dataStart, UByte **dataEnd, ReadMemFn readMemCallback, va_list args)
{
	UWord		memWord;
	UByte		ch, index;
	char		nameCopy[kMaxNameLength];
	UByte		*namePtr = (UByte*) nameCopy;

	*name = 0;
	*dataStart = address;
	*dataEnd = address;
	
	if ((*readMemCallback) ((void *) address, kMaxNameLength, nameCopy, args) != kNoErr)
		return;

	// Read first character of name, stripping off most significant bit.
	//
	ch = (unsigned char) (*namePtr++ & 0x7F);

	// Variable length name, if value is 0 thru 1F.
	//
	if (ch < 0x20)
		{
		// The name is the new variable format. This may be a module or a method name.
		//
		if (ch == 0)
			// Second byte is the actual length.
			//
			ch = *namePtr++;

		name[0] = 0;
		for (index = 1; index <= ch; ++index)
			// When the name gets over kMaxNameLength, truncate the name and just increment to the end of the name.
			//
			if (index <= kMaxNameLength)
				name[++name[0]] = (char) (*namePtr++);
			else
				break;
		
		// The name may not be word aligned. Data after the name always starts on a word boundary.
		//	10/18/94 dkk
		//	Added additional 2 character offset to address. In translation from MacsBug my guess is the length
		//	bytes were accidentally omitted. This caused a bug where the last character of the symbol name was
		//	added to the symbol name address as the literal pool size. This caused the returned address range
		//	to not contain the address we were interested in.
		//
		address += ch + 2;
		if ((ULongWord) address % 2)
			++address;
			
		*dataStart = address;

		//	Variable format names are followed by a word which defines the length of the literal pool
		//	after the code of this procedure and before the code of the next procedure. If the word is
		//	odd then assume it is not a length.
		//
		if ((*readMemCallback) ((void *) address, sizeof(UWord), &memWord, args) != kNoErr)
			return;
		if (memWord & 1)
			// Length word is odd, not the length.
			//
			*dataEnd = address;
		else
			// Add length word + length.
			//
			*dataEnd = address + 2 + memWord;

		name[name[0] + 1] = 0;			// terminating null to make it a TProtocolString
		if (name[0] & 1)				// do we need a pad byte?
			name[name[0] + 2] = 0;		// add one
		}
	else
		{
		// If the most significant bit of the second character is set, the name is the 16 byte class.method format,
		// other wise its the 8 byte format.
		//
		if (*namePtr & 0x80)
			{
			//	The name is the 16 byte class.method format. Class and method are stored in reverse order in memory.
			// Skip to byte 9 of the name to copy.  Address currently points to byte 2.
			//
			namePtr += 7;

			// Initialize length byte.
			//
			name[0] = 0;
			
			// Copy characters in the class.
			//
			for (index = 1; index <= 8; ++index)

				// Strip the spaces used to pad to 8 characters.
				//
				if (*namePtr != ' ')
					name[++name[0]] = (char) (*namePtr++);
				else
					++namePtr;

			//	Insert the '.' to indicate a method
			//
			name[++name[0]] = '.';
			
			// Reset pointer to beginning of the name.
			//
			namePtr -= 16;
			
			// Copy characters in method
			//
			for (index = 1; index <= 8; ++index)

				// Strip the spaces used to pad to 8 characters.  First two bytes also have most significant bit set.
				//
				if (*namePtr & 0x7F != ' ')
					name[++name[0]] = (char) (*namePtr++ & 0x7F);
				else
					++namePtr;

			// Skip to byte after end of name.
			//
			address += 16;
			}
		else
			{
			// 8 byte format.  Copy first character and initialize length byte.
			//
			name[1] = (char) ch;
			name[0] = 1;
			
			// Copy remaining characters in name.
			//
			for (index = 2; index <= 8; ++index)

				// Strip the spaces used to pad to 8 characters.
				//
				if (*namePtr != ' ')
					name[++name[0]] = (char) (*namePtr++);
				else
					++namePtr;

			// Skip to byte after end of name.
			//
			address += 8;
			}

		//	Fixed length names do not indicate the data length. Assumed immediately after the name.
		//
		*dataStart = address;
		*dataEnd = address;

		name[name[0] + 1] = 0;			// terminating null to make it a TProtocolString
		if (name[0] & 1)				// do we need a pad byte?
			name[name[0] + 2] = 0;		// add one
		}
	
}


//
//	FindReturn
//
//		Search for an RTS, RTD or JMP (A0) instruction.  The address following the instruction is returned in afterReturn.
//		Function value is true if return is found, false otherwise.
//
//		start				-	Address to start search for return.
//		limit				-	Address to stop search for return.
//		afterReturn		-	Address after return instruction (if found).
//		return value	-	True if return is found, false otherwise.
//

static Boolean FindReturn (UByte *start, UByte *limit, UByte **afterReturn, ReadMemFn readMemCallback, va_list args)
{
	UWord		memWord;
	
	while (start < limit)
		{
		if ((*readMemCallback) ((void *) start, sizeof(UWord), &memWord, args) != kNoErr)
			return false;
		
		switch (memWord)
			{
			case kJMPA0:
			case kRTS:
				*afterReturn = start + 2;			// JMP (A0) and RTS instruction are both two byte instructions
				return (true);
				break;
	
			case kRTD:
				*afterReturn = start + 4;			//	RTD is a four byte instruction.
				return (true);
				break;
	
			default:
				start += 2;
				break;
			}
		}
		
	return (false);
}


//
//	LegalSymbolChar
//
//		Return True if ch is in the set ['a'..'z', 'A'..'Z', '0'..'9', '_', '%', '.']
//

static Boolean LegalSymbolChar (short ch)
{
	return (((ch >= 'a') && (ch <= 'z')) ||
				((ch >= 'A') && (ch <= 'Z')) ||
				 ((ch >= '0') && (ch <= '9')) ||
				  (ch == '_') || (ch == '%') || (ch == '.') ||
				   (ch == '<') || (ch == '>') || (ch == ',')	// $$$ [Edmark, PBE] added '<', '>', and ','
				    );
}


//
//	LegalTargetChar
//
//		Return True if ch is in the set ['a'..'z', 'A'..'Z', '0'..'9', '_', '%', '.', ' '].  Same as LegalSymbolChar
//		except that a space is also a legal character.
//

static Boolean LegalTargetChar (short ch)
{
	return (((ch >= 'a') && (ch <= 'z')) ||
				((ch >= 'A') && (ch <= 'Z')) ||
				 ((ch >= '0') && (ch <= '9')) ||
				  (ch == '_') || (ch == '%') || (ch == '.') || (ch == ' ') ||
				   (ch == '<') || (ch == '>') || (ch == ',')	// $$$ [Edmark, PBE] added '<', '>', and ','
				    );
}


}	// namespace Whisper

