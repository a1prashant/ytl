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
        std::string name{ "noname" };
        Stepper( std::string appname ) : name( appname ) {}
        virtual bool step() = 0;
    };
    using StepperApplication = Stepper;
    template <class APP> class Runner {
        std::unique_ptr<APP> app;

        std::thread theThread;

        ExitSignal exitSignal;

        public:
        template <typename... ARGS> Runner( ARGS... args ) : app( std::make_unique< APP >( args... ) ) {
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
                    std::cout << app->name << ": " << "stepFunc failed\n";
                    break;
                }
                std::cout << app->name << ": " << "stepFunc succeeded\n";

                std::this_thread::sleep_for( std::chrono::seconds(1) );
            }
        }
    };
}

namespace Consumer {
    class Application : public System::StepperApplication {
        public:
        template <class... Ts> Application(Ts... ts) : System::StepperApplication( ts... ) { }
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
    class Application : public System::StepperApplication {
        std::queue<Data> queue;
        size_t counter{0};
        public:
        template <class... Ts> Application(Ts... ts) : System::StepperApplication( ts... ) { }
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
        std::string name{};
        BaseValidator(std::string id) : name(id) {}
        virtual bool validate( int i ) { return true; }
    };
    template <typename T> struct crtp {
        T & derived() { return static_cast< T & >( *this ); }
        const T & derived() const { return static_cast< const T & >( *this ); }
    };
    template <typename T> struct Validator : public crtp<T>, public BaseValidator {
        template <class... Ts> Validator(Ts... ts) : BaseValidator( ts... ) { }
        bool validate( int i ) override {
            auto result = this->derived().check( i );
            std::cout << this->derived().name << " check:" << result << "\n";
            return result;
        }
    };
    struct IsEvenChecker : public Validator< IsEvenChecker > {
        template <class... Ts> IsEvenChecker(Ts... ts) : Validator< IsEvenChecker >( ts... ) { }
        bool check( int i ) { return i % 2 == 0; }
    };
    struct IsOddChecker : public Validator< IsOddChecker > {
        template <class... Ts> IsOddChecker(Ts... ts) : Validator< IsOddChecker >( ts... ) { }
        bool check( int i ) { return i % 2 == 1; }
    };
    struct IsPrimeChecker : public Validator< IsPrimeChecker > {
        template <class... Ts> IsPrimeChecker(Ts... ts) : Validator< IsPrimeChecker >( ts... ) { }
        bool check( int i ) { return true; }
    };
}

namespace Txformer {
    using namespace Validator;
    class Application : public System::StepperApplication {

        using PtrValidator = std::shared_ptr< BaseValidator >;
        PtrValidator isEvenChecker;
        PtrValidator isOddChecker;
        PtrValidator isPrimeChecker;

        using Validators = std::vector< PtrValidator >;
        Validators validators {
            std::make_shared< IsEvenChecker >( "IsEvenChecker" )
            , std::make_shared< IsOddChecker >( "IsOddChecker" )
            , std::make_shared< IsPrimeChecker >( "IsPrimeChecker" )
        };

        using Enrichers = std::vector< bool >;
        Enrichers enrichers;

        public:
        template <class... Ts> Application(Ts... ts) : System::StepperApplication( ts... ) { }

        bool step() {
            const auto & products = Producer::produce();
            std::cout << "products-size:" << products.size() << ", size-validators:" << validators.size() << "\n";
            for( const auto & p : products ) {
                for( const auto & v : validators ) {
                    auto result = v->validate( p.i );
                    enrichers.push_back( result );
                }
            }
            return Consumer::consume( Txformer::convert( Producer::produce() ) );
        }
    };
}

int main()
{
    System::Runner< Producer::Application > producerRunner{ "Producer" };
    System::Runner< Txformer::Application > txformerRunner{ "Txformer" };
    System::Runner< Consumer::Application > consumerRunner{ "Consumer" };

    producerRunner.start();
    txformerRunner.start();
    consumerRunner.start();

    std::this_thread::sleep_for(std::chrono::seconds(7));
 
    consumerRunner.stop();
    txformerRunner.stop();
    producerRunner.stop();

    std::cout << "Exiting...\n";
}

