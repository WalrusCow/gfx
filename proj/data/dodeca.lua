dodeca = gr.mesh('dodecahedron', {
    {    1.,             1.,              1.},
    {    1.,             1.,              -1.},
    {    1.,             -1.,             1.},
    {    1.,             -1.,             -1.},
    {   -1.,             1.,              1.},
    {   -1.,             1.,              -1.},
    {   -1.,             -1.,             1.},
    {   -1.,             -1.,             -1.},
    {    0.618034,        1.618034,        0.},
    {   -0.618034,        1.618034,        0.},
    {    0.618034,       -1.618034,        0.},
    {   -0.618034,       -1.618034,        0.},
    {    1.618034,        0.,              0.618034},
    {    1.618034,        0.,             -0.618034},
    {   -1.618034,        0.,              0.618034},
    {   -1.618034,        0.,             -0.618034},
    {    0.,             0.618034,         1.618034},
    {    0.,             -0.618034,        1.618034},
    {    0.,             0.618034,        -1.618034},
    {    0.,             -0.618034,       -1.618034}
    }, {
    {   {1},       { 8},      {0},       {12},      {13},  },
    {   {4},       { 9},      {5},       {15},      {14},  },
    {   {2},       {10},      {3},       {13},      {12},  },
    {   {7},       {11},      {6},       {14},      {15},  },
    {   {2},       {12},      {0},       {16},      {17},  },
    {   {1},       {13},      {3},       {19},      {18},  },
    {   {4},       {14},      {6},       {17},      {16},  },
    {   {7},       {15},      {5},       {18},      {19},  },
    {   {4},       {16},      {0},       { 8},      { 9},  },
    {   {2},       {17},      {6},       {11},      {10},  },
    {   {1},       {18},      {5},       { 9},      { 8},  },
    {   {7},       {19},      {3},       {10},      {11},  }
})
