/* Notes:
- This file is for backwards compatibility. Do not include it in new projects.
*/

#ifndef TRYCATCHSIGNALHANDLER_H
#define TRYCATCHSIGNALHANDLER_H

#include "TryCatchSignalHandler2.h"
#undef TRY
#undef END_TRY
#undef FINISH_TRY

// For each try sequence:
#define TRY( identifier ) \
	do{\
		TryCatchSignalHandler::Handler::processLevelSignal();\
		TryCatchSignalHandler::Handler::preTryStatement();\
		std::jmp_buf * volatile tryCatchSignalFunctionState = 0;\
		try{\
			tryCatchSignalFunctionState = ( std::jmp_buf * )new( std::jmp_buf ); /* safe cast? */\
			TryCatchSignalHandler::Handler::pushNewFunctionState( tryCatchSignalFunctionState );\
			if( setjmp( *tryCatchSignalFunctionState )==0 )
#define END_TRY( identifier ) \
			TryCatchSignalHandler::Handler::processLevelSignal();\
		}
#define FINISH_TRY( identifier ) \
		if( tryCatchSignalFunctionState ){\
			delete[] tryCatchSignalFunctionState;\
			tryCatchSignalFunctionState = 0;\
		}\
		TryCatchSignalHandler::Handler::postTrySequence();\
	}while( false )

#endif // TRYCATCHSIGNALHANDLER_H
