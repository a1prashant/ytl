#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <thread>
#include <chrono>
#include <queue>
#include <future>

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
    class ExitSignal {
        std::promise<void> signal;
        std::future<void> result = std::move(signal.get_future());
        public:
        enum class State { Triggered, Timeout };
        void trigger() { signal.set_value(); }
        State exitOrWait( auto period ) {
            switch( result.wait_for( period ) ) {
                default: [[fallthrough]];
                case std::future_status::deferred: [[fallthrough]];
                case std::future_status::ready: return State::Triggered;
                case std::future_status::timeout: return State::Timeout;
            }
        }
    };
    class Application {
        public:
        virtual bool step() = 0;
    };
    template <class APP> class Runner {
        std::unique_ptr<APP> app;

        std::thread theThread;

        ExitSignal exitSignal;

        public:
        template <typename... ARGS> Runner( ARGS... args ) {
            app.reset( new APP( args... ) );
        }
        void start( ) {
            theThread = std::thread( &Runner<APP>::threadProc, this );
        }
        void stop( ) {
            exitSignal.trigger();
            theThread.join();
            std::cout << "joined...\n";
        }
        void threadProc( ) {
            std::cout << "inside threadProc\n";
            auto waitTime = std::chrono::milliseconds(1);
            while ( exitSignal.exitOrWait( waitTime ) == ExitSignal::State::Timeout ) {
                if( ! app->step( ) ) {
                    std::cout << "stepFunc failed\n";
                    break;
                }
                std::cout << "stepFunc succeeded\n";

                std::this_thread::sleep_for( std::chrono::seconds(1) );
            }
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

    std::this_thread::sleep_for(std::chrono::seconds(5));
 
    runnerForSource.stop();
    runnerForUser.stop();
    runnerForXformer.stop();

    std::cout << "Exiting...\n";
}

