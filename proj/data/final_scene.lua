require('final_box')

-- Name, KS, Phong, Alpha, refr
brown = gr.material({0.094, 0.05, 0.003}, {0.00, 0.00, 0.00}, 1000, 1, 1)
lensM = gr.material({0.2, 0.2, 0.2}, {0.02, 0.02, 0.02}, 1e10, .11, 1.85)
magM = gr.material({0, 0, 1}, {0, 0, 0}, 1, 1)
--gold = gr.material({0.75164, 0.60648, 0.22648}, {0.628281, 0.555802, 0.366065}, 51.2, 1)
gold = gr.material({0.75164, 0.60648, 0.22648}, {0,0,0}, 51.2, 1)
suzyM = gr.material({1, 0, 0}, {0, 0, 0}, 1, 1)
--chairM = gr.material({0.507, 0.507, 0.507}, {0.508, 0.508, 0.508}, 2, 1)
chairM = gr.material({0.507, 0.507, 0.507}, {0,0,0}, 2, 1)

antFoodM = gr.material({0, 0.75, 0.75}, {0,0,0},1,1)
antM = gr.material({0.02, 0.02, 0.02}, {0,0,0},1,1)

wood = gr.texture_material("wood.png", {0, 0, 0}, 1, 1)
bw = gr.function_material("bw_squares", {0, 0, 0}, 25, 1)
bluegreen = gr.function_material("blue_green_squares", {0, 0, 0}, 25, 1)

concrete = gr.texture_material("concrete.png", {0,0,0},1,1)
brick = gr.texture_material("brick.png", {0,0,0},1,1)
wood = gr.texture_material("wood3.png",{0,0,0},1,1)

floor:set_material(wood)
back:set_material(brick)
left:set_material(brick)

camera = {2, -3, -1}

--
-- Magnifying glass
--
mag = gr.node('mag')

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
-- Ant scene: Box + "food" + ant + magnifying glass
--
antScene = gr.node('antScene')
antScene:translate(-1.3, -roomSize / 2, -3)
antScene:rotate('Y', 45)

box = gr.cube('box')
antScene:add_child(box)
box:set_material(suzyM)
box:scale(0.5, 0.5, 0.5)

antFood = gr.torus('antFood', 1.5)
antScene:add_child(antFood)
antFood:set_material(antFoodM)
antFood:translate(0.25, 0.63, 0.25)
antFood:rotate('X', -32)
antFood:rotate('Y', 45)
antFood:scale(0.053, 0.053, 0.063)

ant = gr.mesh('ant', readobj('ant.obj'))
antScene:add_child(ant)
ant:set_material(antM)
ant:translate(0.55, 0.4, .3)
ant:rotate('X', 180)
ant:rotate('Z', 90)
ant:rotate('Y', -90)
ant:scale(0.03, 0.03, 0.03)

mag:translate(0.62, 0.67, 0.9)
mag:scale(0.45, 0.45, 0.45)
antScene:add_child(mag)

--scene:add_child(antScene)

--
-- A chair
--
chairScene = gr.node('chairScene')

armWidth = 0.3

-- Back pillar
pillarRad = 0.4
backPillar = gr.cylinder('backPillar')
backPillar:set_material(chairM)
backPillar:translate(0, 2.2, 0)
backPillar:rotate('X', 90)
backPillar:scale(pillarRad, pillarRad, 2.2)

-- Front pillar
frontPillar = gr.cylinder('frontPillar')
frontPillar:set_material(chairM)
frontPillar:translate(0, 4/3, 3 - pillarRad)
frontPillar:rotate('X', 90)
frontPillar:scale(pillarRad, pillarRad, 4/3)

-- Arm of chair
armSide = gr.cube('armSide')
armSide:set_material(chairM)
armSide:scale(armWidth, 2.2, 3 - pillarRad)

-- A side of the chair
chairArm = gr.node('chairArm')
chairArm:add_child(backPillar)
chairArm:add_child(frontPillar)
chairArm:add_child(armSide)

leftArmCentre = -1.5 + pillarRad/2
rightArmCentre = 1.5 - pillarRad/2

-- Left side of chair
leftChairArm = gr.node('leftChairArm')
leftChairArm:add_child(chairArm)
leftChairArm:translate(leftArmCentre, 0, 0)
chairScene:add_child(leftChairArm)

-- Right side of chair
rightChairArm = gr.node('rightChairArm')
rightChairArm:add_child(chairArm)
rightChairArm:translate(rightArmCentre, 0, 0)
chairScene:add_child(rightChairArm)

-- Back of chair
back = gr.cube('chairBack')
chairScene:add_child(back)
back:set_material(chairM)
back:translate(-1.5, 0, 0)
back:scale(3, 4, armWidth)

-- Base of chair
base = gr.cube('base')
chairScene:add_child(base)
base:translate(leftArmCentre, 0, 0)
base:set_material(chairM)
base:scale(3 - pillarRad, 1, 3 - 1.5*pillarRad)

-- Chair step
step = gr.cube('step')
chairScene:add_child(step)
step:translate(leftArmCentre, 0, 3 - pillarRad)
step:set_material(chairM)
step:scale(3 - pillarRad, 0.5, pillarRad/2)

-- Chair seat
seat = gr.cube('seat')
chairScene:add_child(seat)
seat:set_material(chairM)
seat:translate(-0.6, 1, 0)
seat:scale(1.2, 0.8, 1.8)

chairScene:translate(0, -roomSize / 2, -6)
chairScene:scale(0.45, 0.45, 0.45)
--scene:add_child(chairScene)

cameraDirection = {-2, -0.8, -2}

SIZE = 1024
gr.render(scene, 'final_scene.png', SIZE, SIZE,
          camera, cameraDirection, {0, 1, 0}, 50,
          {0.4, 0.4, 0.4}, {frontTopLeft, backTopRight})
