-- soft shadows file
require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
cyan = gr.material({0, 0.75, 0.75}, {0, 0, 0}, 15, 1)
red = gr.material({0.75, 0.00, 0.00}, {0, 0, 0}, 15, 0.33, 1.35)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)

--sphere = gr.sphere('sphere')
--sphere:set_material(yellow)
--sphere:translate(0, -roomSize / 2 + 2, -20)
--scene:add_child(sphere)

L = gr.node('L')

c1 = gr.cube('c1')
c1:set_material(yellow)
c1:translate(0, -roomSize / 2 + 2, -20)
c1:scale(0.4, 0.1, 3)
L:add_child(c1)

c2 = gr.cube('c2')
c2:set_material(yellow)
c2:translate(0, -roomSize / 2 + 2, -17)
c2:scale(1.5, 0.1, 0.4)
L:add_child(c2)

o = gr.torus('O', 0.3)
o:translate(0, -roomSize / 2 + 2, -17.5)
o:set_material(cyan)
o:rotate('X', 90)
o:scale(0.9, 0.9, 0.9)

L1 = gr.node('L1')
L1:add_child(L)
L1:translate(-2.8, 0, 0)

L2 = gr.node('L1')
L2:add_child(L)
L2:translate(1.3, 0, 0)


scene:add_child(L1)
scene:add_child(o)
scene:add_child(L2)

roof:set_material(grey)

topLight = gr.area_light(1, {0, TOP, BACK/2}, {1, 1, 1}, {1, 0, 0})

SIZE = 512
camera = {0, -roomSize / 4 + 5, -13}
gr.render(scene, 'soft.png', SIZE, SIZE,
          camera, {0, -3, -2}, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {topLight})
