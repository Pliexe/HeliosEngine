#pragma once

#define BIND_ERROR(condition, error)    \
if (condition)                          \
{                                       \
    HL_MESSAGE(error);                  \
    return;                             \
}      