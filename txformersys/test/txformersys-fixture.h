#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <thread>
#include <chrono>
#include <queue>

std::queue<int> gq;
std::mutex gqMutex;

namespace System {
    template <class APP> class Runner {
        std::unique_ptr<APP> app;

        std::thread theThread;

        std::mutex m;
        std::condition_variable cv;
        bool exit{ false };

        public:
        template <typename... ARGS> Runner( ARGS... args ) {
            app.reset( new APP( args... ) );
        }
        void start( ) {
            theThread = std::thread( &Runner<APP>::threadProc, this );
        }
        void threadProc( ) {
            std::cout << "inside threadProc\n";
            while( 1 ) {
                if( exit == true ) {
                    std::cout << "externally set exit\n";
                    break;
                }
                if( ! app->step( ) ) {
                    std::cout << "stepFunc failed\n";
                    break;
                }
                std::cout << "stepFunc succeeded\n";

                std::this_thread::sleep_for( std::chrono::seconds(1) );
            }
        }
        void join() {
            theThread.join();
            std::cout << "joined...\n";
        }
    };
}
namespace User {
    class Application {
        int counter{};
        public:
        bool step() {
            {
                std::lock_guard<std::mutex> lock( gqMutex );
                counter = gq.front( );
                gq.pop();
            }
            std::cout << "User::Step: received:" << counter << "\n";
            if( counter == 10 ) return false;
            return true;
        }
    };
}

namespace Source {
    class Application {
        struct Data {
            int i;
            string s;
        };
        std::vector<Data> dataArray;
        const size_t totalItem = 100;
        size_t counter = 0;
        public:
        Application() {
            for( size_t i = 0 ; i < totalItem ; i ++ ) {
                dataArray.push_back( { i, std::string("src.data.") + std::to_string(i) } );
            }
        }
        bool step() {
            std::cout << "Source::Step\n";
            {
                std::lock_guard<std::mutex> lock( gqMutex );
                gq.push( dataArray[counter++] );
            }
            if( counter == totalItem ) return false;
            return true;
        }
    };
}


