#ifndef CNDP_COMMON_SYSTEM_NAMESPACE_H_
#define CNDP_COMMON_SYSTEM_NAMESPACE_H_

/* Standard System Library on alphabetical order */
#include <algorithm>
#include <assert.h>
#include <cassert>
#include <climits>	// for minimum and maximum primitive value
#include <cmath>	//for log
#include <cstdlib>	// for standard library (exit function, division and etc)
#include <cstring>	// for memcpy
#include <fstream>	// read write file
#include <iomanip>	// for set precision (rounding) operation
#include <iostream> // read file
#include <iterator>
#include <list>
#include <map>
#include <queue> // queue for bfs
#include <set>
#include <sstream>
#include <string>
#include <string> // for stoi function
#include <unordered_map>
#include <vector>

// Following for the system timer to sleep the thread and get the random number
#include <chrono>
#include <ctime>
#include <thread>

/*-----------------------------------------------------------------------*
 | invoke common std parameter rather then using the whole std namespace |
 | -- an alphabetical order												 |
 *-----------------------------------------------------------------------*/

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::min;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;

// to fixed the decimal precision
// using std::fixed;
// map function
// using std::map;
// to read and write the file
// using std::ofstream;
// stream template
// using std::ostream;
// pointer to the functional object
// using std::ptr_fun;
// to create the pair
// using std::pair;
// to create the queue
// using std::queue;
// to set up the decimal percison
// using std::setprecision;
// to swap the values between two variables
// using std::swap;

#endif /* CNDP_COMMON_SYSTEM_NAMESPACE_H_ */
