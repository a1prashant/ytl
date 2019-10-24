ytl
 - fieldmapper
   - declare CField< src_type, dest_type >
   - auto b = a;                 // should be CField type, and will be
   - auto b = a.value();         // should be src_type
   - auto b = a.output();        // should be preset dest_type
   - auto b = a.getValue();
   - auto b = a.getOutput();
   - auto b = a.get<T>();        // should be any other Type 'T'
   - ytl::convert(a);            // should be preset type
   - ytl::getValue(a);           //
   - ytl::getOutput(a);          //
   - ytl::get<B>(a);             // 
   - ytl::convert<B>(a);         // 
   - auto b = YTL::convert< CField< src_type, NEW_dest_type >( a );

