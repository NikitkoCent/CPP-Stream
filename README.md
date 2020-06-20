# cpp-stream
C++17-compatible imitation of [Java 8 Stream API](https://docs.oracle.com/javase/8/docs/api/java/util/stream/Stream.html).
Elements are processed lazily and with minimum overhead.

## Example
```cpp
#include <stream/stream.h>      // stream::Stream
#include <stream/operations.h>  // stream::ops::*
#include <vector>               // std::vector
#include <string>               // std::string
#include <algorithm>            // std::transform
#include <iostream>             // std::cout

int main()
{
    namespace ops = stream::ops;

    const std::vector strings = {
        "dignity",
        "respect",
        "Fred's",
        "sun",
        "color",
        "friends",
        "premium",
        "run",
        "sector",
        "fried",
        "smile",
        "Fritos",
        "for",
        "global",
        "Fridays",
        "broadcast",
        "food"
    };

    // no copying or moving, string is stored via reference
    stream::Stream(strings)
    | ops::filter([](const char *str) { // select only strings started with 'F' or 'f'
        return (str[0] == 'f' || str[0] == 'F');
      }) // nothing will be evaluated at this point
    | ops::map([](std::string str) { // make every word is in uppercase
        (void)std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return std::toupper(ch); });
        return str;
      }) // still nothing will be evaluated
    | ops::print_to(std::cout); // there is terminal operation - causes evaluating everything

    return 0;
}
```
Output:
> FRED'S FRIENDS FRIED FRITOS FOR FRIDAYS FOOD

## Supported terminal operations
* `print_to(ostream)` - prints all elements of the stream to a given output stream `ostream`;
* `reduce(identityFn, accumulatorFn)` - reduces all elements of the stream to 1 similar to the following pseudo-code:
```cpp
stream::Stream s(...);
auto v = identityFn(s.getNext());
while (!s.isEnd())
    v = accumulatorFn(v, s.getNext());
return v;
```
* `reduce(accumulatorFn)` - reduces all elements of the stream to 1 similar to the following pseudo-code:
```cpp
stream::Stream s(...);
auto v = s.getNext();
while (!s.isEnd())
    v = accumulatorFn(v, s.getNext());
return v;
```
* `nth(n)` - returns `n`th element of the stream;
* `to_vector()` - moves all elements of the origin stream to a `std::vector`.

## Supported non terminal operations
* `skip(n)` - skips n elements of the stream;
* `get(n)` - takes only n first elements from the stream;
* `map(mapFn)` - creates a new stream of results of applying the given functor `mapFn` to every element of the given stream;
* `filter(predicateFn)` - leaves only elements to which applying `predicateFn` functor returns true;
* `group(n)` - creates a new stream of `std::vector`s with `n` elements of the origin stream.

## Requirements
* Using the library:
    * C++17-compatible compiler and STL
* For running tests, you need:
    * [git](https://git-scm.com/downloads)
    * [CMake](https://cmake.org/download/) >= v3.8.2
    * Additionally, if you want to measure code coverage, you need:
        * compiler,
        [gcov](https://en.wikipedia.org/wiki/Gcov)
        [, optional: [lcov](https://wiki.documentfoundation.org/Development/Lcov)]
        compatible with each other
        (for example, gcc 6.3.0, gcov 6.3.0 and lcov 1.13 are compatible)
        * [genhtml](https://linux.die.net/man/1/genhtml)
        * [CMake](https://cmake.org/download/) >= v3.13

## Build and run tests
See [Requirements](#requirements) chapter first.

Just use [CMake](https://cmake.org/download/) (run from project root):
```bash
# Build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Running tests
ctest -C Release -V
```

Note: This project uses [GoogleTest](https://github.com/google/googletest) for testing.

## Code coverage measurement
See [Requirements](#requirements) chapter first.

CMake scripts provides the next additional variables for setting up the measurement:
* `COLLECT_CODE_COVERAGE=<LCOV|OTHER|OFF>` - if set to `LCOV` then code coverage target `collect_coverage` will be created. `OTHER` assumes compiling/linking code for measuring via different than lcov instrument ([gcov](https://en.wikipedia.org/wiki/Gcov) for example). `OFF` does nothing and set by default.
* `GCOV_PATH` - allows to specify path to [gcov](https://en.wikipedia.org/wiki/Gcov) executable. `gcov` by default.

It's recommended to build the code in `Debug` mode for coverage measurement.

Example (run from project root):
```bash
# Build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_C_COMPILER=gcc-6.3.0 \
-DCMAKE_CXX_COMPILER=g++-6.3.0 \
-DCOLLECT_CODE_COVERAGE=LCOV \
-DGCOV_PATH=gcov-6.3.0 \
..

# Collecting the coverage:
cmake --build . --target collect_coverage

# Html reports will be generated. Use browser to see it:
<your_browser_name> ./collect_coverage/index.html &
```
