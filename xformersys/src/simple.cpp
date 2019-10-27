#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <thread>
#include <chrono>
#include <queue>

struct SourceData {
    size_t i;
    std::string s;
};

size_t TOTAL_ITEMS = 10;

struct UserData {
    size_t i;
    std::string s;
};

std::queue<SourceData> gq;
std::mutex gqMutex;

namespace System {
    class Application {
        public:
        virtual bool step() = 0;
    };
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
    class Application : public System::Application {
        std::vector<SourceData> data;
        public:
        bool step() override {
            {
                std::lock_guard<std::mutex> lock( gqMutex );
                data.push_back( gq.front( ) );
                gq.pop();
            }
            std::cout << "User::Step: received:" << data.size() << "\n";
            if( data.size() == TOTAL_ITEMS ) return false;
            return true;
        }
    };
}

namespace Source {
    class Application : public System::Application {
        std::vector<SourceData> dataArray;
        size_t counter = 0;
        public:
        Application() {
            for( size_t i = 0 ; i < TOTAL_ITEMS ; i ++ ) {
                dataArray.push_back( { i, std::string("src.data.") + std::to_string(i) } );
            }
        }
        bool step() override {
            std::cout << "Source::Step\n";
            {
                std::lock_guard<std::mutex> lock( gqMutex );
                gq.push( dataArray[counter++] );
            }
            if( counter == TOTAL_ITEMS ) return false;
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
