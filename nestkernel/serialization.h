#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/export.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>

#define BOOST_IARCHIVE boost::archive::binary_iarchive
#define BOOST_OARCHIVE boost::archive::binary_oarchive
