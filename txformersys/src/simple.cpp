#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <utility>
#include <algorithm>
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
    using Container = std::deque<Consumer::Data>;
    Container consumerQ;
    bool consume( const Container & in ) {
        std::lock_guard<std::mutex> lock( cqMutex );
        std::copy( in.begin(), in.end(), std::back_inserter( consumerQ ) );
        return true;
    }
}

namespace Producer {
    struct Data {
        size_t i;
        std::string s;
    };
    std::mutex productMutex;
    using Container = std::vector<Producer::Data>;
    Container product;
    Container produce() {
        std::lock_guard<std::mutex> lock( productMutex );
        Container output = product;
        return output;
    }
}

namespace Txformer {
    Consumer::Data convert( const Producer::Data & data ) {
        return { data.i, data.s };
    }
    Consumer::Container convert( const Producer::Container & in ) {
        Consumer::Container out;
        std::transform( in.begin() , in.end() , std::back_inserter( out ), [](const Producer::Data & inData) {
                return convert( inData );
                });
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
                    consumerQ.pop_front();
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
            {
                std::lock_guard<std::mutex> lock( productMutex );
                product.push_back( {counter++, "ProducerData"} );
            }
            return true;
        }
    };
}

namespace Validator {
    struct BaseValidator {
        virtual bool validate( int i ) { return true; }
    };
    template <typename T> struct crtp {
        T & derived() { return static_cast< T & >( *this ); }
        const T & derived() const { return static_cast< const T & >( *this ); }
    };
    template <typename T> struct Validator : public crtp<T>, public BaseValidator {
        bool validate( int i ) override {
            std::cout << "befor check" << "\n";
            auto result = this->derived().check( i );
            std::cout << "after check" << "\n";
            return result;
        }
    };
    struct IsEvenChecker : public Validator< IsEvenChecker > {
        bool check( int i ) { return i % 2 == 0; }
    };
    struct IsOddChecker : public Validator< IsOddChecker > {
        bool check( int i ) { return i % 2 == 1; }
    };
    struct IsPrimeChecker : public Validator< IsPrimeChecker > {
        bool check( int i ) { return true; }
    };
}

namespace Txformer {
    using namespace Validator;
    class Application : public System::Application {

        using PtrValidator = std::shared_ptr< BaseValidator >;
        PtrValidator isEvenChecker;
        PtrValidator isOddChecker;
        PtrValidator isPrimeChecker;

        using Validators = std::vector< PtrValidator >;
        Validators validators {
            std::make_shared< IsEvenChecker >()
            , std::make_shared< IsOddChecker >()
            , std::make_shared< IsPrimeChecker >()
        };

        using Enrichers = std::vector< bool >;
        Enrichers enrichers;

        public:

        bool step() {
            const auto & products = Producer::produce();
            std::cout << "size-validators:" << validators.size() << "\n";
            for( const auto & v : validators ) {
                std::cout << "Going for validation" << "\n";
                auto result = v->validate( products[0].i );
                std::cout << "validation done" << "\n";
                if( result ) { return false; }
                std::cout << "before push_back" << "\n";
                enrichers.push_back( result );
                std::cout << "after push_back" << "\n";
            }
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

