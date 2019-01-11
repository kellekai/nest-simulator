#ifndef CHECKPOINT_MANAGER_H
#define CHECKPOINT_MANAGER_H

// c++ includes
#include <cstring>

// Includes from libnestutil:
#include "manager_interface.h"

// Includes from nestkernel
#include "kernel_manager.h"
#include "node.h"

// Includes from BOOST for serialization
#include "serialization.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

#include <fti.h>
#define FTI_CONFIG_FILE "config.fti"

// Set archive type for serialization buffer
#define BOOST_IARCHIVE boost::archive::binary_iarchive
#define BOOST_OARCHIVE boost::archive::binary_oarchive

namespace nest {

  class CheckpointManager : public ManagerInterface
  {
    public:
      CheckpointManager();
      virtual ~CheckpointManager();

      virtual void initialize();
      virtual void finalize();

      virtual void set_status( const DictionaryDatum& );
      virtual void get_status( DictionaryDatum& );

      virtual void write_checkpoint();
      virtual void finalize_fti();

      template< typename communicatorT >
      void init_fti( communicatorT & comm )
      {
        char tstr[FTI_BUFS];
        std::strncpy( tstr, config_file.c_str(), FTI_BUFS );
        FTI_Init( tstr, comm );
      }

      template< typename T >
      void expose_data( T & data )
      { 
        *oa_ << data;
      }

      template< typename Class >
      void register_type()
      {
        if(!initialized_) {
          std::cout << "CheckpointManager::register_type :: Serialization Manager is not initialized!" << std::endl;
          return;
        }
        //std::cout << "TRYREGISTER " << typeid(Class).name() << std::endl;
        oa_->register_type< Class >();
        ia_->register_type< Class >();
      }
      template< typename Derived, typename Base >
      void register_cast() 
      {
        boost::serialization::void_cast_register< Derived, Base >();
      }
      
    private:
      
      int id_;

      BOOST_OARCHIVE *oa_;
      BOOST_IARCHIVE *ia_;

      bool initialized_;

      std::stringstream fn_;
      std::stringstream ss_;

      std::string config_file;
      std::ofstream fs_;

  };

}

#endif // CHECKPOINT_MANAGER_H
