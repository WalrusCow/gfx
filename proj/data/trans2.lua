require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
bluegreen = gr.function_material("blue_green_squares", {0, 0, 0}, 25, 1)
clear = gr.material({1, 1, 1}, {0, 0, 0}, 15, 0, 1.09)
clear2 = gr.material({0.75, 0.00, 0.00}, {0.05, 0.05, 0.05}, 15, 0.33, 1.35)
clear3 = gr.material({1, 1, 1}, {0.05, 0.05, 0.05}, 15, 0.33, 1.35)

sphere = gr.sphere('sphere')
sphere:set_material(clear)
sphere:translate(0, -roomSize / 2 + 2, -roomSize * 4 + 4)
scene:add_child(sphere)

s1 = gr.cube('s1')
s1:set_material(clear2)
s1:translate(1, -roomSize / 2 + 3, -roomSize * 4 + 4)
s1:rotate('X',33)
s1:rotate('Y',23)
scene:add_child(s1)

t = gr.torus('t', 0.3)
t:set_material(clear3)
t:translate(-0.5, -roomSize / 2 + 2.6, -roomSize * 4 + 8)
t:rotate('X',33)
t:rotate('Y',23)
t:scale(0.3, 0.3, 0.3)
scene:add_child(t)


floor:set_material(grey)
back:set_material(bw)
roof:set_material(grey)
right:set_material(grey)
left:set_material(grey)
front:set_material(grey)

camera = {0, -roomSize / 2 + 2, roomSize * -4 + 10}

SIZE = 512
gr.render(scene, 'trans2.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
