/* Notes:
- std::jmp_buf can be a weird type (arrays or so), and cannot be used properly in template containers without using a pointer.
- The pointer to the std::jmp_buf variable is reported to be in the scope of the setjmp(). To avoid warnings, volatile is specified.
*/

#ifndef TRYCATCHSIGNALHANDLER_H
#define TRYCATCHSIGNALHANDLER_H

#include <exception>
#include <csignal>
#include <csetjmp>
#include <stack>

#ifdef _GLIBCXX_USE_NOEXCEPT
	#define SIGNALEXCEPTION_NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#else
	#define SIGNALEXCEPTION_NOEXCEPT /**/
#endif

class SignalException: public std::exception{
	// This class contains an exception translated from an exception signaled by the OS.
	public:
		SignalException(int signal);
		virtual const char* what() const SIGNALEXCEPTION_NOEXCEPT;
		int getSignalType() const SIGNALEXCEPTION_NOEXCEPT;
	private:
		int m_signal;
		static const char * const s_readableSIGTERM;
		static const char * const s_readableSIGSEGV;
		static const char * const s_readableSIGINT;
		static const char * const s_readableSIGILL;
		static const char * const s_readableSIGABRT;
		static const char * const s_readableSIGFPE;
		static const char * const s_readableSIG_INVALID;
};

class TryCatchSignalOverflow: public std::exception{
	// This class contains an exception telling that the stack of this library is overflowed.
	private:
		static const char * const s_stackOverflowMessage;
	public:
		virtual const char* what() const SIGNALEXCEPTION_NOEXCEPT;
};

class TryCatchSignalUnderflow: public std::exception{
	// This class contains an exception telling that the stack of this library is underflowed.
	private:
		static const char * const s_stackUnderflowMessage;
	public:
		virtual const char* what() const SIGNALEXCEPTION_NOEXCEPT;
};

class TryCatchSignalHandler{
	// This is a static class. It contains handling functions of this library.
	private:
		static volatile int s_signal;
		static std::stack<std::jmp_buf *> s_functionStates;
		static std::jmp_buf * getLastFunctionState(); // non-constant
	public:
		virtual ~TryCatchSignalHandler() = 0;
		static void processLevelSignal();
		static void preTryStatement();
		static void postTrySequence();
		static void signalHandler(int signal);
		static unsigned int getLastFunctionStatesCount();
		static const std::jmp_buf * getLastFunctionStateConst(); // constant
		static void pushNewFunctionState(std::jmp_buf * functionState);
};

// For initialization:
#define CATCH_SIGNAL( signalNum ) std::signal( signalNum, TryCatchSignalHandler::signalHandler )
#define CATCH_SIGNALS_CPU() \
	CATCH_SIGNAL( SIGSEGV );\
	CATCH_SIGNAL( SIGILL );\
	CATCH_SIGNAL( SIGFPE )
#define CATCH_SIGNALS_ALL() \
	CATCH_SIGNAL( SIGTERM );\
	CATCH_SIGNALS_CPU();\
	CATCH_SIGNAL( SIGINT );\
	CATCH_SIGNAL( SIGABRT )

// For each try sequence:
#define TRY( identifier ) \
	TryCatchSignalHandler::processLevelSignal();\
	TryCatchSignalHandler::preTryStatement();\
	std::jmp_buf * volatile tryCatchSignalFunctionState ## identifier = 0;\
	try{\
		tryCatchSignalFunctionState ## identifier = ( std::jmp_buf * )new( std::jmp_buf ); /* safe cast? */\
		TryCatchSignalHandler::pushNewFunctionState( tryCatchSignalFunctionState ## identifier );\
		if( setjmp( *tryCatchSignalFunctionState ## identifier )==0 )
#define END_TRY( identifier ) \
		TryCatchSignalHandler::processLevelSignal();\
	}
#define FINISH_TRY( identifier ) \
	if( tryCatchSignalFunctionState ## identifier ){\
		delete[] tryCatchSignalFunctionState ## identifier;\
		tryCatchSignalFunctionState ## identifier = 0;\
	}\
	TryCatchSignalHandler::postTrySequence()

#endif // TRYCATCHSIGNALHANDLER_H
