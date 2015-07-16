require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25)

--require('mymesh')
--scene:add_child(mymesh)
--mymesh:translate(1, -1, 0)
--mymesh:set_material(blue)
s = gr.sphere('sphere')
scene:add_child(s)
s:set_material(blue)
s:translate(1, -1, 0)

s2 = gr.sphere('sphere2')
scene:add_child(s2)
s2:set_material(blue)
s2:translate(1, 1, 0)

l1 = gr.light({0, 0, 2}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'myscene.png', 128, 128,
    {0, 0, 4}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
