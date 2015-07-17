require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25)

--require('mymesh')
--scene:add_child(mymesh)
--mymesh:translate(1, -1, 0)
--mymesh:set_material(blue)
s = gr.sphere('sphere')
scene:add_child(s)
s:set_material(bw)
s:scale(2, 2, 2)

l1 = gr.light({0, 0, 6}, {0.8, 0.8, 0.8}, {1, 0, 0})

SIZE = 128
gr.render(scene, 'myscene.png', SIZE, SIZE,
    {0, 0, 6}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
