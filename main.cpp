#include "TryCatchSignalHandler2.h"
#include <iostream>

#define MESSAGE_EXCEPTION( what, file, line ) std::cerr << "Program survived a " << what << " in " << file << ':' << line << std::endl

int main(/*int argc, char *argv[]*/){
	TryCatchSignalHandler::catchSignalsCpu();
	/*for( unsigned int i = 0; i<=65535; ++i ){
		if( TryCatchSignalHandler::catchSignal( i ) ){
			std::cout << "Known signal: " << i << std::endl;
		}
	}*/
	
	std::cout << "SIGSEGV = " << SIGSEGV << std::endl;
	std::cout << "SIGILL = " << SIGILL << std::endl;
	std::cout << "SIGFPE = " << SIGFPE << std::endl;
	std::cout << std::endl;
	
	// Let's do several illegal operations and let's see!
	TRY{
		std::cout << "Attempting to dereference a null pointer..." << std::endl;
		TRY{
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int* ptr = 0;
			std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access #1
			std::cout << "Unexpected success!" << std::endl;
		}END_TRY()
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY();
		std::cout << std::endl;
		
		std::cout << "Attempting to dereference a null pointer again..." << std::endl;
		TRY{
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int* ptr = 0;
			std::cout << "*ptr = " << *ptr << std::endl; // invalid memory access #2
			std::cout << "Unexpected success!" << std::endl;
		}END_TRY()
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY();
		std::cout << std::endl;
		
		#if defined( __GNUC__ ) || defined( _MSC_VER )
			std::cout << "Attempting forbidden HLT instruction, press a key if frozen..." << std::endl;
			TRY{
				// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
				#ifdef __GNUC__
					__asm__( "HLT" ); // forbidden CPU instruction
				#endif
				#ifdef _MSC_VER
					__asm HLT; // forbidden CPU instruction
				#endif
				std::cout << "The HLT instruction actually worked!" << std::endl;
			}END_TRY()
			catch( std::exception& e ){
				MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
			}
			FINISH_TRY();
			std::cout << std::endl;
		#endif
		
		std::cout << "Simulating a SIGFPE signal..." << std::endl;
		TRY{
			// TryCatchSignalHandler::signalHandler() gets called, program continues without needing a jump (because simulated).
			std::raise( SIGFPE ); // invalid instruction (simulated)
			std::cout << "Unexpected success!" << std::endl;
		}END_TRY()
		catch( std::exception& e ){
			MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
		}
		FINISH_TRY();
		std::cout << std::endl;
		
		std::cout << "Attempting an integer division by 0..." << std::endl;
		do{
			// TryCatchSignalHandler::signalHandler() gets called if application not wrapped.
			int a = 1;
			int b = 0;
			std::cout << "res = " << a/b << std::endl; // invalid math operation
			std::cout << "Unexpected success!" << std::endl;
		} while( false );
		
		std::cout << "This message is never shown." << std::endl;
	}END_TRY()
	catch( std::exception& e ){
		MESSAGE_EXCEPTION( e.what(), __FILE__, __LINE__ );
	}
	FINISH_TRY();
	std::cout << std::endl;
	
	// Prove that we are still alive after all these CPU exceptions:
	std::cout << "Enter a character now: ";
	char userEntry;
	std::cin >> userEntry;
	
	return 0;
}
