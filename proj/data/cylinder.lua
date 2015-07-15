-- A polygonal approximation to a cylinder
cyl = gr.mesh('cylinder', {
  {0.000000, 0.000000, 1.000000},
  {0.587785, 0.000000, 0.809017},
  {0.951057, 0.000000, 0.309017},
  {0.951057, 0.000000, -0.309017},
  {0.587785, 0.000000, -0.809017},
  {0.000000, 0.000000, -1.000000},
  {-0.587785, 0.000000, -0.809017},
  {-0.951057, 0.000000, -0.309017},
  {-0.951057, 0.000000, 0.309017},
  {-0.587785, 0.000000, 0.809017},
  {0.000000, 1.000000, 1.000000},
  {0.587785, 1.000000, 0.809017},
  {0.951057, 1.000000, 0.309017},
  {0.951057, 1.000000, -0.309017},
  {0.587785, 1.000000, -0.809017},
  {0.000000, 1.000000, -1.000000},
  {-0.587785, 1.000000, -0.809017},
  {-0.951057, 1.000000, -0.309017},
  {-0.951057, 1.000000, 0.309017},
  {-0.587785, 1.000000, 0.809017}
}, {
  {{0}, {1}, {11}, {10}},
  {{1}, {2}, {12}, {11}},
  {{2}, {3}, {13}, {12}},
  {{3}, {4}, {14}, {13}},
  {{4}, {5}, {15}, {14}},
  {{5}, {6}, {16}, {15}},
  {{6}, {7}, {17}, {16}},
  {{7}, {8}, {18}, {17}},
  {{8}, {9}, {19}, {18}},
  {{9}, {0}, {10}, {19}},
  {{9}, {8}, {7}, {6}, {5}, {4}, {3}, {2}, {1}, {0}},
  {{10}, {11}, {12}, {13}, {14}, {15}, {16}, {17}, {18}, {19}}
})
