require('boxscene')

s2=gr.node('s2')
grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
yellow = gr.material({0.75, 0.75, 0}, {0, 0, 0}, 15, 1)

gourd = gr.mesh('gourd', readobj('gourd.obj'))
gourd:set_material(yellow)
gourd:translate(0, 0, 4)
gourd:rotate('Z', 90)
gourd:rotate('Y', 45)
s2:add_child(gourd)

roof:set_material(grey)

l=gr.light({0,0,10},{1,1,1},{1,0,0})
SIZE = 512
gr.render(s2, 'phong.png', SIZE, SIZE,
          {0, 0, 8}, {0, 0, -1}, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {l})


--SIZE=128
--gr.render(scene, 'phong.png', SIZE, SIZE,
--          camera, cameraDirection, {0, 1, 0}, 50,
--          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
