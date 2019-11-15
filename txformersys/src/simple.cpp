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
        size_t id;
        size_t value;
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
        size_t id;
        size_t value;
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
        return { data.id, data.value, data.s };
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
                    std::cout << "Consumer::Step: received:" << data.id << "\n";
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
        virtual bool validate( const auto & entry ) { return true; }
    };
    template <typename T> struct crtp {
        T & derived() { return static_cast< T & >( *this ); }
        const T & derived() const { return static_cast< const T & >( *this ); }
    };
    template <typename T> struct Validator : public crtp<T>, public BaseValidator {
        template <class... Ts> Validator(Ts... ts) : BaseValidator( ts... ) { }
        bool validate( const auto & entry ) override {
            auto result = this->derived().check( entry );
            std::cout << this->derived().name << " check:" << result << "\n";
            return result;
        }
    };
    struct IsEvenChecker : public Validator< IsEvenChecker > {
        using result_type = bool;
        template <class... Ts> IsEvenChecker(Ts... ts) : Validator< IsEvenChecker >( ts... ) { }
        result_type check( const auto & entry ) { return entry.id % 2 == 0; }
    };
    struct IsOddChecker : public Validator< IsOddChecker > {
        using result_type = bool;
        template <class... Ts> IsOddChecker(Ts... ts) : Validator< IsOddChecker >( ts... ) { }
        result_type check( const auto & entry ) { return entry.id % 2 == 1; }
    };
    struct IsPrimeChecker : public Validator< IsPrimeChecker > {
        using result_type = bool;
        template <class... Ts> IsPrimeChecker(Ts... ts) : Validator< IsPrimeChecker >( ts... ) { }
        result_type check( const auto & entry ) { return true; }
    };
}

namespace Transformer {

    template <typename K, typename V> class Cache {
        std::unordered_map<K, V> cache;
        public:
        bool conditionalAdd( const auto & newEntry ) {
            if( CriteriaReplace( newEntry ) ) {
                cache.insert( newEntry );
                return true;
            }
        }
        bool CriteriaReplace( const auto & newEntry ) {
            const auto kReplace{ true };
            const auto kDoNotReplace{ false };

            auto result = cache.find( newEntry.first )

            if( result == cache.end() ) return kReplace;
            else if( newEntry.tStart > result.tEnd ) return kReplace;
            else if( result.value >= newEntry.value ) return kReplace;
            return kDoNotReplace;
        }
    };
}

namespace Arranger {
    template <typename K, typename V> class Feed {
        struct State {
            kNotUsed,
            kInUse,
            kFutureUse,
        };
        struct Context {
            K           key;
            PlannedTime plannedTime;
        };
        const std::chrono::seconds kExecutionDuration{ 5 };
        using value_type = std::pair<K,Context>;
        std::map< K, V > table;
        public:
        using key_type = K;
        using mapped_type = V;
        bool inspect( const value_type & entry ) {
            auto key = entry.key;
            auto result = table.find( key );
            if( result == table.end() ) {
                using std::chrono;
                table.push_back( { key, { V, system_clock::now() } } );
                return true;
            } else {
                auto currentPlan = result.plannedTime;
                if( currentPlan + 
            }
            // new entry is new ; add it directly
            // new entry can/NOT replace old? whats the criteria
            // if Key is already in use
            
        }
    };
}

namespace Txformer {
    template <class Validators, class OPController>
    class Application : public System::StepperApplication {

        Validators validators;
        OPArranger op_arranger;

        public:
        template <class... Ts>
            Application( Ts... ts, const Validators & v, const OPController & opi )
            : System::StepperApplication( ts... )
            , validators( v )
            , op_arranger( opi ) {
            }

        bool step() {
            const auto & products = Producer::produce();
            for( const auto & p : products ) {
                op_arranger::mapped_type output; 
                entry.key( p.id );
                for( const auto & v : validators ) {
                    output.emplace_back( v->validate( p ) );
                }
                if( output.size() ) {
                    op_arranger.inspect( { p.id, output } );
                }
            }
            return true;
        }
    };
}

auto CreateOutputArranger() {
    using Enrichers = std::vector< bool >;
    Enrichers enrichers;

    using DerivedInfo = std::tuple<
        IsEvenChecker::result_type
        , IsOddChecker::result_type
        , IsPrimeChecker::result_type
        >;
    using ProductInfoMap = std::map< Producer::ProductId, DerivedInfo >;
    ProductInfoMap productInfoMap;
    return productInfoMap;
}

auto CreateValidators() {
    using namespace Validator;
    using PtrValidator = std::shared_ptr< BaseValidator >;
    using Validators = std::vector< PtrValidator >;
    Validators validators {
        std::make_shared< IsEvenChecker >( "IsEvenChecker" )
            , std::make_shared< IsOddChecker >( "IsOddChecker" )
            , std::make_shared< IsPrimeChecker >( "IsPrimeChecker" )
    };
    return validators;
}

int main()
{
    const auto & validators = CreateValidators();
    const auto & op_arranger = CreateOutputArranger();

    System::Runner< Producer::Application > producerRunner{ "Producer" };
    System::Runner< Txformer::Application > txformerRunner{ "Txformer", validators, op_arranger };
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

