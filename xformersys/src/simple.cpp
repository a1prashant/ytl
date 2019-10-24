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
        int counter{};
        public:
        bool step() {
            std::cout << "Source::Step\n";
            {
                std::lock_guard<std::mutex> lock( gqMutex );
                gq.push( counter++ );
            }
            if( counter == 10 ) return false;
            return true;
        }
    };
}

int main()
{
    System::Runner< Source::Application >       runnerForSource;
    System::Runner< Source::Application >       runnerForXformer;
    System::Runner< User::Application >         runnerForUser;

    runnerForXformer.start();
    runnerForUser.start();
    runnerForSource.start();

    runnerForSource.join();
    runnerForUser.join();
    runnerForXformer.join();

    std::cout << "Exiting...\n";
}
/*
namespace Source {
    struct Data {
        int start;
        int value;
        int end;
    };
    class Application : IApplication {
        void operate() {
        }
    }
}

namespace Xformer {
}

namespace User {
    struct Data {
        int start;
        int value;
        int end;
    };
}

std::string data;
bool ready = false;
bool processed = false;
*/
