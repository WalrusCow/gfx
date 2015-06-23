-- test for hierarchical ray-tracers.
-- Thomas Pflaum 1996

gold = gr.material({0.9, 0.8, 0.4}, {0.8, 0.8, 0.4}, 25)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)
blue = gr.material({0.7, 0.6, 1}, {0.5, 0.4, 0.8}, 25)
scenenode = gr.material({0,0,1}, {0,0,0}, 25)
red = gr.material({1, 0.3, 0}, {0.1, 0.1, 0.1}, 25)

scene = gr.node('scene')
scene:translate(0, -2, -15)
scene:rotate('X', 23)

-- the arc
arc = gr.node('arc')
scene:add_child(arc)
arc:rotate('Y', 60)
arc:translate(0, 0.8, -10)
p1 = gr.cube('p1')
arc:add_child(p1)
p1:set_material(gold)
p1:translate(-2.4, 0, -0.4)
p1:scale(0.8, 4, 0.8)

p2 = gr.cube('p2')
arc:add_child(p2)
p2:set_material(gold)
p2:translate(1.6, 0, -0.4)
p2:scale(0.8, 4, 0.8)

s = gr.sphere('s')
arc:add_child(s)
s:set_material(gold)
s:translate(0, 4, 0)
s:scale(4, 0.6, 0.6)

-- the floor
plane = gr.mesh('plane', {
	{-1, 0, -1},
	{ 1, 0, -1},
	{1,  0, 1},
	{-1, 0, 1},
     }, {
	{3, 2, 1, 0}
     })
scene:add_child(plane)
plane:set_material(grass)
plane:scale(30, 30, 30)

-- sphere
--poly = gr.mesh('poly', {
--    {   1.,             1.,              1.},
--    {   1.,             1.,              -1.},
--    {   1.,             -1.,             1.},
--    {   1.,             -1.,             -1.},
--    {  -1.,             1.,              1.},
--    {  -1.,             1.,              -1.},
--    {  -1.,             -1.,             1.},
--    {  -1.,             -1.,             -1.},
--    {   0.618034,        1.618034,        0.},
--    {  -0.618034,        1.618034,        0.},
--    {   0.618034,       -1.618034,        0.},
--    {  -0.618034,       -1.618034,        0.},
--    {   1.618034,        0.,              0.618034},
--    {   1.618034,        0.,             -0.618034},
--    {  -1.618034,        0.,              0.618034},
--    {  -1.618034,        0.,             -0.618034},
--    {   0.,             0.618034,         1.618034},
--    {   0.,             -0.618034,        1.618034},
--    {   0.,             0.618034,        -1.618034},
--    {   0.,             -0.618034,       -1.618034}
--   }, {
--    {  1,       8,       0,       12,      13 },
--    {  4,       9,       5,       15,      14 },
--    {  2,       10,      3,       13,      12 },
--    {  7,       11,      6,       14,      15 },
--    {  2,       12,      0,       16,      17 },
--    {  1,       13,      3,       19,      18 },
--    {  4,       14,      6,       17,      16 },
--    {  7,       15,      5,       18,      19 },
--    {  4,       16,      0,        8,       9 },
--    {  2,       17,      6,       11,      10 },
--    {  1,       18,      5,        9,       8 },
--    {  7,       19,      3,       10,      11 }
-- })
--scene:add_child(poly)
--poly:translate(-2, 1.618034, 0)
--poly:set_material(blue)

-- The lights
l1 = gr.light({200,200,400}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 5, -20}, {0.4, 0.4, 0.8}, {1, 0, 0})
--l3 = gr.light({-20, 20, 0}, {0.6, 0.6, 0.6}, {1, 0, 0})

sk=gr.sphere('sk')
sk:set_material(blue)
arc:add_child(sk)

sc=gr.sphere('sc')
sc:set_material(scenenode)
scene:add_child(sc)


--b1 = gr.cube('b1')
--b1:translate(-4, 0, 0)
----b1:rotate('y', 25)
--b1:scale(5, 5, 5)

--b1:set_material(blue)
--arc:add_child(b1)

gr.render(scene, 'ez.png', 256, 256,
	  {0, 0, 0,}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, {l1, l2, l3})
