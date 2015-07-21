-- A simple scene with some miscellaneous geometry.
-- This file is very similar to nonhier.lua, but interposes
-- an additional transformation on the root node.  
-- The translation moves the scene, and the position of the camera
-- changes accordingly.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 1)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 1)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 1)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 1)

scene = gr.node( 'scene' )
scene:translate(0, 0, -800)

s1 = gr.sphere('s1', {0, 0, -400}, 100)
scene:add_child(s1)
s1:set_material(mat1)

s2 = gr.sphere('s2', {200, 50, -100}, 150)
scene:add_child(s2)
s2:set_material(mat1)

s3 = gr.sphere('s3', {0, -1200, -500}, 1000)
scene:add_child(s3)
s3:set_material(mat2)

b1 = gr.cube('b1', {-200, -125, 0}, 100)
scene:add_child(b1)
b1:set_material(mat4)

s4 = gr.sphere('s4', {-100, 25, -300}, 50)
scene:add_child(s4)
s4:set_material(mat3)

s5 = gr.sphere('s5', {0, 100, -250}, 25)
scene:add_child(s5)
s5:set_material(mat1)

-- A small stellated dodecahedron.

require('smstdodeca')

steldodec:set_material(mat3)
scene:add_child(steldodec)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'nonhier.png', 256, 256,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
