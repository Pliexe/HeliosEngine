#include <iostream>

#define CRITICAL_TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[CRITICAL] [FAIL] " #expr << std::endl; \
            std::cout << "Critical test failed! Now existing..." << std::endl; \
            std::exit(1); \
        } else { \
            std::cout << "[CRITICAL] [PASS] " #expr << std::endl; \
        } \
    } while(0)

#define MINOR_TEST(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "[MINOR] [FAIL] " #expr << std::endl; \
        } else { \
            std::cout << "[MINOR] [PASS] " #expr << std::endl; \
        } \
    } while(0)