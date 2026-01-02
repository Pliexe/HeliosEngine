#include "Helios/Math/VectorImpl.h"
#include <iostream>

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"   

#define ORANGE  "\033[38;5;214m"
#define YELLOW  "\033[38;5;226m"
#define WHITE   "\033[38;5;15m"
#define BLUE    "\033[38;5;15m"
#define CYAN    "\033[36m"

#define BG_RED  "\033[48;5;196m"
#define BG_GREEN "\033[48;5;46m"
#define BG_CYAN "\033[48;5;15m"

#define EPSILON 1e-6

#define TITLE(x) \
    std::cout << "\n\t\033[1;33m" << x << "\033[0m\n" << std::endl;

#define CRITICAL_TEST_APPROX(a, b) \
    if (!((a - b).sqr_magnitude() < EPSILON * EPSILON)) { \
        auto xa = (a); \
        auto xb = (b); \
        std::cerr << ORANGE "[CRITICAL] " RED "[FAIL] " RESET "[" BG_RED << __LINE__ << RESET << "] " << #a << RED " ≈ " RESET << #b << RESET \
        << " (" RED << std::to_string(xa) << RESET " ≈ " GREEN << std::to_string(xb) << RESET ")" << RESET << std::endl; \
        std::cout << "Critical test failed! Now exiting..." << std::endl; \
        std::exit(1); \
    } else { \
        std::cout << ORANGE "[CRITICAL] " GREEN "[PASS] " RESET << #a << GREEN " ≈ " RESET << #b << RESET << std::endl; \
    }

#define CRITICAL_TEST_EQUALS(a, b) \
    if (!(a == b)) { \
        auto xa = (a); \
        auto xb = (b); \
        std::cerr << ORANGE "[CRITICAL] " RED "[FAIL] " RESET "[" BG_RED << __LINE__ << RESET << "] " << #a << RED " == " RESET << #b << RESET \
        << " (" RED << std::to_string(xa) << RESET " == " GREEN << std::to_string(xb) << RESET ")" << RESET << std::endl; \
        std::cout << "Critical test failed! Now exiting..." << std::endl; \
        std::exit(1); \
    } else { \
        std::cout << ORANGE "[CRITICAL] " GREEN "[PASS] " RESET << #a << GREEN " == " RESET << #b << RESET << std::endl; \
    }

#define CRITICAL_TEST_NOT_EQUALS(a, b) \
    if (!(a != b)) { \
        auto xa = (a); \
        auto xb = (b); \
        std::cerr << ORANGE "[CRITICAL] " RED "[FAIL] " RESET "[" BG_RED << __LINE__ << RESET << "] " << #a << RED " != " RESET << #b << RESET \
        << " (" RED << std::to_string(xa) << RESET " != " GREEN << std::to_string(xb) << RESET ")" << RESET << std::endl; \
        std::cout << "Critical test failed! Now exiting..." << std::endl; \
        std::exit(1); \
    } else { \
        std::cout << ORANGE "[CRITICAL] " GREEN "[PASS] " RESET << #a << GREEN " == " RESET << #b << RESET << std::endl; \
    }

#define CRITICAL_TEST(expr) \
    if (!(expr)) { \
        std::cerr << ORANGE "[CRITICAL] " RED "[FAIL] " RESET "[" BG_RED << __LINE__ << RESET << "] " #expr << RESET << std::endl; \
        std::cout << "Critical test failed! Now exiting..." << std::endl; \
        std::exit(1); \
    } else { \
        std::cout << ORANGE "[CRITICAL] " GREEN "[PASS] " RESET << #expr << RESET << std::endl; \
    }

#define MINOR_TEST(expr) \
    if (!(expr)) { \
        std::cerr << YELLOW "[MINOR] " RED "[FAIL] " RESET "[" BG_RED << __LINE__ << RESET << "] " #expr << RESET << std::endl; \
    } else { \
        std::cout << YELLOW "[MINOR] " GREEN "[PASS] " RESET << #expr << RESET << std::endl; \
    }