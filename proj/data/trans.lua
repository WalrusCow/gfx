require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)
mirror = gr.material({.1, .1, .1}, {0.6, 0.6, 0.6}, 15, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)

clear = gr.material({1, 1, 1}, {0, 0, 0}, 15, 0.05, 1.55)
notasclear = gr.material({1, 1, 1}, {0, 0, 0}, 15, 0.333, 1.55)
evenlessclear = gr.material({1, 1, 1}, {0, 0, 0}, 15, 0.666, 1.55)

s = gr.sphere('sphere')
s:set_material(clear)
s:translate(-3, -roomSize / 2 + 2, -10)
scene:add_child(s)

s2= gr.sphere('s2')
s2:set_material(notasclear)
s2:translate(0, -roomSize / 2 + 2, -10)
scene:add_child(s2)

s3 =gr.sphere('s2')
s3:set_material(evenlessclear)
s3:translate(3, -roomSize / 2 + 2, -10)
scene:add_child(s3)
--floor:set_material(mirror)
roof:set_material(grey)

SIZE = 512
gr.render(scene, 'trans.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
