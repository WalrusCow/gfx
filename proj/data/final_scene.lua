require('final_box')

-- Name, KS, Phong, Alpha, refr
brown = gr.material({0.094, 0.05, 0.003}, {0.00, 0.00, 0.00}, 1000, 1, 1)
lensM = gr.material({0.2, 0.2, 0.2}, {0.02, 0.02, 0.02}, 1e10, .11, 1.45)
magM = gr.material({0, 0, 1}, {0, 0, 0}, 1, 1)
--gold = gr.material({0.75164, 0.60648, 0.22648}, {0.628281, 0.555802, 0.366065}, 51.2, 1)
gold = gr.material({0.75164, 0.60648, 0.22648}, {0,0,0}, 51.2, 1)
suzyM = gr.material({1, 0, 0}, {0, 0, 0}, 1, 1)

antFoodM = gr.material({0, 0.75, 0.75}, {0,0,0},1,1)
antM = gr.material({0.02, 0.02, 0.02}, {0,0,0},1,1)

wood = gr.texture_material("wood.png", {0, 0, 0}, 1, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
bluegreen = gr.function_material("blue_green_squares", {0, 0, 0}, 25, 1)

floor:set_material(brown)
back:set_material(bw)
left:set_material(bluegreen)

camera = {2, -3, -1}

--
-- Magnifying glass
--
mag = gr.node('mag')
--scene:add_child(mag)
--mag:translate(unpack(camera))
--mag:translate(-1.6, -.5, -2)
--mag:rotate('Y', 45)
--mag:rotate('X', -5)

lens = gr.sphere('lens')
mag:add_child(lens)
lens:set_material(lensM)
lens:scale(0.62, 0.62, 0.04)

edge = gr.torus('edge', 0.06)
mag:add_child(edge)
edge:set_material(magM)
edge:scale(0.6, 0.6, 0.3)

handle = gr.cylinder('handle')
mag:add_child(handle)
handle:set_material(magM)
handle:translate(0, -2.08, 0)
handle:rotate('X', 90)
handle:scale(0.05, 0.05, 1.5)

--
-- Monkey head on wall
--
suzy = gr.mesh('suzy', readobj('suzy.obj'))
--scene:add_child(suzy)
suzy:set_material(gold)
suzy:translate(LEFT, BOTTOM/2, BACK)
suzy:translate(-0.7, -0.4, 3.5)
suzy:rotate('Y', 90)
suzy:rotate('X', 35)
suzy:scale(0.6, 0.6, 0.6)

--
-- Box for ant
--

antScene = gr.node('antScene')
antScene:translate(-1.3, -roomSize / 2, -3)
antScene:rotate('Y', 45)

box = gr.cube('box')
antScene:add_child(box)
box:set_material(suzyM)
box:scale(0.5, 0.5, 0.5)

antFood = gr.torus('antFood', 1.5)
--antScene:add_child(antFood)
antFood:set_material(antFoodM)
antFood:translate(0.25, 0.63, 0.25)
antFood:rotate('X', -32)
antFood:rotate('Y', 45)
antFood:scale(0.053, 0.053, 0.063)

ant = gr.mesh('ant', readobj('ant.obj'))
--antScene:add_child(ant)
ant:set_material(antM)
ant:translate(0.55, 0.4, .3)
ant:rotate('X', 180)
ant:rotate('Z', 90)
ant:rotate('Y', -90)
ant:scale(0.03, 0.03, 0.03)

antScene:add_child(mag)
scene:add_child(antScene)

--
-- Ant
--
--ant = gr.mesh('ant', readobj('ant.obj'))
--ant:set_material(suzyM)
--ant:rotate('Z', 180)
--ant:scale(0.06, 0.06, 0.06)
--
--a1 = gr.node('a1')
--a1:add_child(ant)
--a1:translate(LEFT - 0.8, BOTTOM - 1, BACK - 0.8)
--a1:translate(unpack(camera))
--a1:translate(-1.9, -.5, -2.3)

--a2 = gr.node('a1')
--a2:add_child(ant)
--a2:translate(unpack(camera))
--a2:translate(-1.9, BOTTOM, -2.3)


--scene:add_child(a1)
--scene:add_child(a2)

cameraDirection = {-2, -0.8, -2}

SIZE = 64
gr.render(scene, 'final_scene.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
