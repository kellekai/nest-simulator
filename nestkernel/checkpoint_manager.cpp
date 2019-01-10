
#include "kernel_manager.h"
#include "checkpoint_manager.h"
#include "archiving_node.h"
#include "device_node.h"
#include "subnet.h"
#include "sibling_container.h"
#include "proxynode.h"
#include "node.h"


void nest::CheckpointManager::initialize()
{ 
  
  if( initialized_ ) {
    std::cout << "[WARNING] CheckpointManager is already initialized!" << std::endl;
    return;
  }

  initialized_ = true;
  
  // set checkpoint file name
  fn_ << "Ckpt" << id_ << "-Rank" << kernel().mpi_manager.get_rank() << ".fti";  
  
  // initialize serialization archives (input and output)
  oa_ = new BOOST_OARCHIVE(ss_);
  ia_ = new BOOST_IARCHIVE(ss_);

  // register basic types and casts
  //register_type<nest::Node>();
  //register_type<nest::Archiving_Node>();
  register_cast<nest::Archiving_Node, Node>();
  //register_type<nest::DeviceNode>();
  register_cast<nest::DeviceNode, Node>();
  //register_type<nest::Subnet>();
  register_cast<nest::Subnet, Node>();
  //register_type<nest::SiblingContainer>();
  register_cast<nest::SiblingContainer, Node>();
  //register_type<nest::proxynode>();
  register_cast<nest::proxynode, Node>();

}

void nest::CheckpointManager::write_checkpoint()
{
    fs_.open(fn_.str());
    fs_ << ss_.str();
    fs_.close();
    id_ ++;
    fn_ << "Ckpt" << id_ << "-Rank" << kernel().mpi_manager.get_rank() << ".fti";  
}

void nest::CheckpointManager::finalize()
{
}

void nest::CheckpointManager::set_status( const DictionaryDatum& )
{
}

void nest::CheckpointManager::get_status( DictionaryDatum& )
{
}

nest::CheckpointManager::CheckpointManager()
  : initialized_(false)
  , id_(0)
  , fn_("")
{
}

nest::CheckpointManager::~CheckpointManager() 
{
}
