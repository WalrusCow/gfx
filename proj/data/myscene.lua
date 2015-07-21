require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
monet = gr.texture_material("monet.png", {0, 0, 0}, 0, 1)

require('mymesh')
scene:add_child(mymesh)
mymesh:translate(-10, -10, -10)
mymesh:scale(20, 20, 20)
mymesh:set_material(bw)

s = gr.sphere('sphere')
scene:add_child(s)
s:set_material(monet)
s:translate(0, -0.5, 2)
s:rotate('X', 90)

l1 = gr.area_light(1, {0, -1.5, 6}, {0.8, 0.8, 0.8}, {1, 0, 0})

SIZE = 128

gr.render(scene, 'myscene.png', SIZE, SIZE,
    {0, 0, 6}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
