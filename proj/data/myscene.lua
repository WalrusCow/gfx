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
s:translate(2, 2, 0)
s:scale(0.4, 0.4, 0.4)

s2 = gr.sphere('sphere2')
scene:add_child(s2)
s2:set_material(blue)
s2:translate(-2, 2, 0)
s2:scale(0.4, 0.4, 0.4)

s3 = gr.sphere('sphere3')
scene:add_child(s3)
s3:set_material(blue)
s3:translate(2, -2, 0)
s3:scale(0.4, 0.4, 0.4)

s4 = gr.sphere('sphere4')
scene:add_child(s4)
s4:set_material(blue)
s4:translate(-2, -2, 0)
s4:scale(0.4, 0.4, 0.4)

l1 = gr.light({0, 0, 2}, {0.8, 0.8, 0.8}, {1, 0, 0})

SIZE = 128
gr.render(scene, 'myscene.png', SIZE, SIZE,
    {0, 0, 6}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
