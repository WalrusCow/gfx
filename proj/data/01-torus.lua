require('boxscene')

grey = gr.material({0.2, 0.2, 0.2}, {0, 0, 0}, 0, 1)
white = gr.material({0.7, 0.7, 0.7}, {0, 0, 0}, 1, 1)
m1 = gr.material({0.35, 0.95, 0.44}, {0, 0, 0}, 15, 1)
m2 = gr.material({0.25, 0.33, 0.88}, {0,0,0}, 25, 1)
m3 = gr.material({1, 0, 1}, {0.01, 0.01, 0.01}, 25, 1)
m4 = gr.material({0.61424, 0.04136, 0.04136}, {0.05, 0.05, 0.05}, 12, 1)
m5 = gr.material({0.2, 0.2, 0.2}, {0.41, 0.41, 0.41}, 25, 1)

t1 = gr.torus('torus', 0.5)
t1:set_material(m1)
t1:translate(-1.3, -roomSize / 2 + 2, -8)
t1:rotate('X', 45)
t1:rotate('Y', 35)
t1:scale(0.75, 0.75, 0.75)
scene:add_child(t1)

t2 = gr.torus('torus', 1.2)
t2:set_material(m2)
t2:translate(0.2, -roomSize / 2 + 6, -16)
t2:rotate('X', 45)
t2:scale(0.3, 0.8, 1.1)
scene:add_child(t2)

t3 = gr.torus('torus', 0.1)
t3:set_material(m3)
t3:translate(0, -roomSize / 2 + 0.5, -10)
t3:rotate('X', -67)
scene:add_child(t3)

t4 = gr.torus('torus', 0.3)
t4:set_material(m5)
t4:translate(roomSize/2, -roomSize / 2, -15)
t4:scale(2.5, 2.5, 2.5)
scene:add_child(t4)

t5 = gr.torus('torus', 0.2)
t5:set_material(m4)
t5:translate(0, -roomSize / 2 + 1.5, -17)
t5:rotate('X', 10)
t5:rotate('Y', 7)
t5:scale(1, 1, 16)
scene:add_child(t5)


roof:set_material(grey)
floor:set_material(white)

SIZE = 512
gr.render(scene, '01-torus.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
