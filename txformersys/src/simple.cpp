#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <iterator>
#include <thread>
#include <chrono>
#include <queue>
#include <future>

namespace Consumer {
    struct Data {
        size_t i;
        std::string s;
    };
    std::mutex cqMutex;
    std::queue<Consumer::Data> consumerQ;
    bool consume( const std::vector<Consumer::Data> & in ) {
        std::lock_guard<std::mutex> lock( cqMutex );
        for( auto & i : in ) { consumerQ.push( i ); }
        return true;
    }
}

namespace Producer {
    struct Data {
        size_t i;
        std::string s;
    };
    std::mutex pqMutex;
    std::queue<Producer::Data> producerQ;
    std::vector<Producer::Data> produce() {
        std::vector<Producer::Data> output;
        std::lock_guard<std::mutex> lock( pqMutex );
        while( ! producerQ.empty() ) {
            output.push_back( producerQ.front() );
            producerQ.pop();
        }
        return output;
    }
}

namespace Txformer {
    Consumer::Data convert( const Producer::Data & data ) {
        return { data.i, data.s };
    }
    template <template <typename> class C>
    C<Consumer::Data> convert( const C<Producer::Data> & in ) {
        C<Consumer::Data> out;
        for( auto & i : in ) {
            out.push_back( convert( i ) );
        }
        return out;
    }
}

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
    class Stepper {
        public:
        virtual bool step() = 0;
    };
    class Application : public Stepper { };
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

namespace Consumer {
    class Application : public System::Application {
        public:
        bool step() override {
            {
                std::lock_guard<std::mutex> lock( cqMutex );
                if( ! consumerQ.empty() ) {
                    auto data = consumerQ.front( );
                    std::cout << "Consumer::Step: received:" << data.i << "\n";
                    consumerQ.pop();
                }
            }
            return true;
        }
    };
}

namespace Producer {
    class Application : public System::Application {
        std::queue<Data> queue;
        size_t counter{0};
        public:
        bool step() override {
            std::cout << "Producer::Step\n";
            producerQ.push( {counter++, "ProducerData"} );
            return true;
        }
    };
}

namespace Txformer {
    class Application : public System::Application {
        public:
        bool step() {
            return Consumer::consume( Txformer::convert( Producer::produce() ) );
        }
    };
}

int main()
{
    System::Runner< Producer::Application > producerRunner{};
    System::Runner< Txformer::Application > txformerRunner{};
    System::Runner< Consumer::Application > consumerRunner{};

    producerRunner.start();
    txformerRunner.start();
    consumerRunner.start();

    std::this_thread::sleep_for(std::chrono::seconds(5));
 
    consumerRunner.stop();
    txformerRunner.stop();
    producerRunner.stop();

    std::cout << "Exiting...\n";
}

