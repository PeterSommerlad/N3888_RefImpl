#pragma once
#include "drawing.h"

class sample_draw
{
public:
    void operator()(::std::experimental::drawing::surface::context& ctxt, double elapsedTimeInMilliseconds);
};

