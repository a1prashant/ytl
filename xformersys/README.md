# xformersys
Transformer system

It shall include:
- EcoSystem
   - Producer System // different types
   - Xformer System
   - Consumer System // different User types
  - DataSystem
   - Producer Data Format { different producer data format
   - Consumer Data Format
  - ControllerSystem
   - Producer Controller
   - Consumer Controller
  - ConfigurationSystem
   - Producer Configuration
   - Consumer Configuration
  - DataEnricher System
   - Input Data Validator
   - Data Enricher

- ProducerSystem
    - iData : Input data // Constant
- InputAdapter
    - iId-iData
- TranformationProcess
    - iId : Catelog // Input ID
    - ProducerProperties // priority
    - ProducerControllerConfig // enable/disable
    - UserProperties // Resource Info
    - PropertiesValidators // apply controller config (both Producer and User)
    - RuleEngine // apply business rules
    - Instance // instance object
    - OutputTimeline
- OutputAdapter
- UserSystem
    - oData : Output data // Constant


Testing:
3 processes running together
1 sends data
2 transforms
3 receives transformed data
