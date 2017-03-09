#include "TryCatchSignalHandler.h"

// #define TRYCATCHSIGNALHANDLER_DEBUG 1
#define NO_SIGNAL_VALUE 0

#ifdef TRYCATCHSIGNALHANDLER_DEBUG
	#include <iostream>
#endif


/// class Exception

const char * const TryCatchSignalHandler::Exception::s_readableSIGTERM = "SIGTERM";
const char * const TryCatchSignalHandler::Exception::s_readableSIGSEGV = "SIGSEGV";
const char * const TryCatchSignalHandler::Exception::s_readableSIGINT = "SIGINT";
const char * const TryCatchSignalHandler::Exception::s_readableSIGILL = "SIGILL";
const char * const TryCatchSignalHandler::Exception::s_readableSIGABRT = "SIGABRT";
const char * const TryCatchSignalHandler::Exception::s_readableSIGFPE = "SIGFPE";
const char * const TryCatchSignalHandler::Exception::s_readableSIG_INVALID = "SIG_INVALID";

TryCatchSignalHandler::Exception::Exception(int signal):
	m_signal( signal )
{
	#ifdef TRYCATCHSIGNALHANDLER_DEBUG
		std::cerr << "\tTryCatchSignalHandler::Exception::Exception() - Signal " << what() << " occurred" << std::endl;
	#endif
	TryCatchSignalHandler::catchSignal( signal ); // re-assign signal handle function (crash on 2nd time otherwise)
}

const char* TryCatchSignalHandler::Exception::what() const SIGNALEXCEPTION_NOEXCEPT{
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

int TryCatchSignalHandler::Exception::getSignalType() const SIGNALEXCEPTION_NOEXCEPT{
	return m_signal;
}


/// class Overflow

const char * const TryCatchSignalHandler::Overflow::s_stackOverflowMessage = "TryCatchSignalHandler stack overflow";

const char* TryCatchSignalHandler::Overflow::what() const SIGNALEXCEPTION_NOEXCEPT{
	return s_stackOverflowMessage;
}


/// class Underflow

const char * const TryCatchSignalHandler::Underflow::s_stackUnderflowMessage = "TryCatchSignalHandler stack underflow";

const char* TryCatchSignalHandler::Underflow::what() const SIGNALEXCEPTION_NOEXCEPT{
	return s_stackUnderflowMessage;
}


/// class TryCatchSignalHandler

volatile int TryCatchSignalHandler::Handler::s_signal = NO_SIGNAL_VALUE;
std::stack<std::jmp_buf *> TryCatchSignalHandler::Handler::s_functionStates;

std::jmp_buf * TryCatchSignalHandler::Handler::getLastFunctionState(){
	if( s_functionStates.empty() ) return 0;
	else return s_functionStates.top();
}

void TryCatchSignalHandler::Handler::processLevelSignal(){
	if( !s_functionStates.empty() ){
		int signal = s_signal;
		s_signal = NO_SIGNAL_VALUE;
		if( signal!=NO_SIGNAL_VALUE ){
			throw TryCatchSignalHandler::Exception( signal );
		}
	}
}

void TryCatchSignalHandler::Handler::preTryStatement(){
	unsigned int count = ( unsigned int )s_functionStates.size();
	if( count!=( unsigned int )( ~0 ) ){
		s_signal = NO_SIGNAL_VALUE; // reset in case of corrupted memory
	}
	else{
		throw TryCatchSignalHandler::Overflow();
	}
}

void TryCatchSignalHandler::Handler::postTrySequence(){
	if( !s_functionStates.empty() ){
		s_functionStates.pop();
	}
	else{
		throw TryCatchSignalHandler::Underflow();
	}
}

void TryCatchSignalHandler::Handler::signalHandler(int signal){
	#ifdef TRYCATCHSIGNALHANDLER_DEBUG
		std::cerr << "\tTryCatchSignalHandler::Handler::signalHandler() - Signal " << signal << " occurred" << std::endl;
	#endif
	s_signal = signal;
	std::jmp_buf * lastFunctionState = ( std::jmp_buf * )getLastFunctionState();
	if( lastFunctionState ){
		/*std::signal( signal, SIG_IGN );*/ // ignore signal for now (useless)
		std::longjmp( *lastFunctionState, 1 );
	}
}

unsigned int TryCatchSignalHandler::Handler::getLastFunctionStatesCount(){
	return ( unsigned int )s_functionStates.size();
}

const std::jmp_buf * TryCatchSignalHandler::Handler::getLastFunctionStateConst(){
	std::jmp_buf * lastFunctionState = getLastFunctionState();
	return lastFunctionState;
}

void TryCatchSignalHandler::Handler::pushNewFunctionState(std::jmp_buf * functionState){
	s_functionStates.push( functionState );
}


/// out of any class

bool TryCatchSignalHandler::catchSignal(int signalNum){
	return std::signal( signalNum, TryCatchSignalHandler::Handler::signalHandler )!=SIG_ERR;
}

void TryCatchSignalHandler::catchSignalsCpu(){
	TryCatchSignalHandler::catchSignal( SIGSEGV );
	TryCatchSignalHandler::catchSignal( SIGILL );
	TryCatchSignalHandler::catchSignal( SIGFPE );
}

void TryCatchSignalHandler::catchSignalsAll(){
	TryCatchSignalHandler::catchSignal( SIGTERM );
	TryCatchSignalHandler::catchSignalsCpu();
	TryCatchSignalHandler::catchSignal( SIGINT );
	TryCatchSignalHandler::catchSignal( SIGABRT );
}


#undef TRYCATCHSIGNALHANDLER_DEBUG
#undef NO_SIGNAL_VALUE
