#include "TryCatchSignalHandler.h"

// #define TRYCATCHSIGNALHANDLER_DEBUG 1
#define NO_SIGNAL_VALUE 0

#ifdef TRYCATCHSIGNALHANDLER_DEBUG
	#include <iostream>
#endif


/// class SignalException

const char * const SignalException::s_readableSIGTERM = "SIGTERM";
const char * const SignalException::s_readableSIGSEGV = "SIGSEGV";
const char * const SignalException::s_readableSIGINT = "SIGINT";
const char * const SignalException::s_readableSIGILL = "SIGILL";
const char * const SignalException::s_readableSIGABRT = "SIGABRT";
const char * const SignalException::s_readableSIGFPE = "SIGFPE";
const char * const SignalException::s_readableSIG_INVALID = "SIG_INVALID";

SignalException::SignalException(int signal):
	m_signal( signal )
{
	#ifdef TRYCATCHSIGNALHANDLER_DEBUG
		std::cerr << "\tSignalException::SignalException() - Signal " << what() << " occurred" << std::endl;
	#endif
	CATCH_SIGNAL( signal ); // re-assign signal handle function (crash on 2nd time otherwise)
}

const char* SignalException::what() const SIGNALEXCEPTION_NOEXCEPT{
	switch( m_signal ){
		case SIGTERM: return s_readableSIGTERM;
		case SIGSEGV: return s_readableSIGSEGV;
		case SIGINT:  return s_readableSIGINT;
		case SIGILL:  return s_readableSIGILL;
		case SIGABRT: return s_readableSIGABRT;
		case SIGFPE:  return s_readableSIGFPE;
		default:      return s_readableSIG_INVALID;
	}
}

int SignalException::getSignalType() const SIGNALEXCEPTION_NOEXCEPT{
	return m_signal;
}


/// class TryCatchSignalOverflow

const char * const TryCatchSignalOverflow::s_stackOverflowMessage = "TryCatchSignalHandler stack overflow";

const char* TryCatchSignalOverflow::what() const SIGNALEXCEPTION_NOEXCEPT{
	return s_stackOverflowMessage;
}


/// class TryCatchSignalUnderflow

const char * const TryCatchSignalUnderflow::s_stackUnderflowMessage = "TryCatchSignalHandler stack underflow";

const char* TryCatchSignalUnderflow::what() const SIGNALEXCEPTION_NOEXCEPT{
	return s_stackUnderflowMessage;
}


/// class TryCatchSignalHandler

volatile int TryCatchSignalHandler::s_signal = NO_SIGNAL_VALUE;
std::stack<std::jmp_buf *> TryCatchSignalHandler::s_functionStates;

std::jmp_buf * TryCatchSignalHandler::getLastFunctionState(){
	if( s_functionStates.empty() ) return 0;
	else return s_functionStates.top();
}

void TryCatchSignalHandler::processLevelSignal(){
	if( !s_functionStates.empty() ){
		int signal = s_signal;
		s_signal = NO_SIGNAL_VALUE;
		if( signal!=NO_SIGNAL_VALUE ){
			throw SignalException( signal );
		}
	}
}

void TryCatchSignalHandler::preTryStatement(){
	unsigned int count = ( unsigned int )s_functionStates.size();
	if( count!=( unsigned int )( ~0 ) ){
		s_signal = NO_SIGNAL_VALUE; // reset in case of corrupted memory
	}
	else{
		throw TryCatchSignalOverflow();
	}
}

void TryCatchSignalHandler::postTrySequence(){
	if( !s_functionStates.empty() ){
		s_functionStates.pop();
	}
	else{
		throw TryCatchSignalUnderflow();
	}
}

void TryCatchSignalHandler::signalHandler(int signal){
	#ifdef TRYCATCHSIGNALHANDLER_DEBUG
		std::cerr << "\tTryCatchSignalHandler::signalHandler() - Signal " << signal << " occurred" << std::endl;
	#endif
	s_signal = signal;
	std::jmp_buf * lastFunctionState = ( std::jmp_buf * )getLastFunctionState();
	if( lastFunctionState ){
		/*std::signal( signal, SIG_IGN );*/ // ignore signal for now (useless)
		std::longjmp( *lastFunctionState, 1 );
	}
}

unsigned int TryCatchSignalHandler::getLastFunctionStatesCount(){
	return ( unsigned int )s_functionStates.size();
}

const std::jmp_buf * TryCatchSignalHandler::getLastFunctionStateConst(){
	std::jmp_buf * lastFunctionState = getLastFunctionState();
	return lastFunctionState;
}

void TryCatchSignalHandler::pushNewFunctionState(std::jmp_buf * functionState){
	s_functionStates.push( functionState );
}


#undef TRYCATCHSIGNALHANDLER_DEBUG
#undef NO_SIGNAL_VALUE
