
#include "kernel_manager.h"
#include "checkpoint_manager.h"
#include "archiving_node.h"
#include "device_node.h"
#include "node.h"


void nest::CheckpointManager::initialize()
{ 
  // set checkpoint file name
  fn_ << "Ckpt-Rank" << kernel().mpi_manager.get_rank() << ".fti";  
  
  // initialize serialization archives (input and output)
  oa_ = new BOOST_OARCHIVE(ss_);
  ia_ = new BOOST_IARCHIVE(ss_);

  // register basic types and casts
  register_cast<Archiving_Node, Node>();
  register_cast<DeviceNode, Node>();

  initialized_ = true;
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
  , fn_("")
{
}

nest::CheckpointManager::~CheckpointManager() 
{
}
