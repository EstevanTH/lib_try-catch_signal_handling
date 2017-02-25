#include "TryCatchSignalHandler.h"
#include <iostream>

#define MESSAGE_EXCEPTION( what, file, line ) std::cerr << "Program survived a " << what << " in " << file << ':' << line << std::endl

int main(/*int argc, char *argv[]*/){
	CATCH_SIGNALS_CPU();
	
	std::cout << "SIGSEGV = " << SIGSEGV << std::endl;
	std::cout << "SIGILL = " << SIGILL << std::endl;
	std::cout << "SIGFPE = " << SIGFPE << std::endl;
	
	// Let's do several illegal operations and let's see!
	TRY( 1 ){
		TRY( 2 ){
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int* ptr = 0;
			std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access #1
		}END_TRY( 2 )
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY( 2 );
		
		TRY( 4 ){
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int* ptr = 0;
			std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access #2
		}END_TRY( 4 )
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY( 4 );
		
		TRY( 3 ){
			// TryCatchSignalHandler::signalHandler() gets called, program continues without needing a jump (because simulated).
			std::raise( SIGILL ); // invalid instruction (simulated)
		}END_TRY( 3 )
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY( 3 );
		
		TRY( 5 ){
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			// This code is often ignored because of optimization.
			int a = 1;
			int b = 0;
			a = a/b; // invalid math operation
		}END_TRY( 5 )
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY( 5 );
		
		do{
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int a = 1;
			int b = 0;
			std::cout << "res = " << a/b << std::endl; // invalid math operation
		} while( false );
		
		std::cout << "This message is never shown." << std::endl;
	}END_TRY( 1 )
	catch( std::exception& e ){
		std::cerr << "Program survived a " << e.what() << " in " << __FILE__ << ":" << __LINE__ << std::endl;
	}
	FINISH_TRY( 1 );
	
	// Prove that we are still alive after all these CPU exceptions:
	std::cout << "Enter a character now: ";
	char userEntry;
	std::cin >> userEntry;
	
	return 0;
}
