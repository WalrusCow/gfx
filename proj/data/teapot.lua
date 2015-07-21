require('readobj')

scene = gr.node('scene')

blue = gr.material({0.25, 0.33, 0.88}, {0.5, 0.4, 0.8}, 25, 1)

teapot = gr.mesh('teapot', readobj('teapot.obj'))
scene:add_child(teapot)
teapot:translate(1, -1, 0)
teapot:set_material(blue)

l1 = gr.light({0, 0, 200}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, 'teapot.png', 128, 128,
    {0, 0, 200}, {0, 0, -1}, {0, 1, 0}, 50,
    {0.4, 0.4, 0.4}, {l1})
