--
-- CS488 -- Introduction to Computer Graphics
--
-- a3mark.lua
--
-- A very simple scene creating a trivial puppet.  The TAs will be
-- using this scene as part of marking your assignment.  You'll
-- probably want to make sure you get reasonable results with it!

rootNode = gr.node('root')

-- Useful constants
noRotation = {0.0, 0.0, 0.0}
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 100)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
cyan = gr.material({0.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

function makeArm(name)
  armScale = {0.3, 1.3, 0.3}
  handScale = {0.35, 0.55, 0.35}

  shoulderRot = {-180, 0, 45}
  elbowRot = {-155, 0, 0}
  wristRot = {-85, 0, 90}

  return makeLimb(name .. 'Arm', blue,
                  shoulderRot, elbowRot, wristRot,
                  armScale, armScale, handScale)
end

function makeLeg(name)
  legScale = {0.3, 1.5, 0.3}
  footScale = {0.3, 0.3, 0.7}

  hipRot = {-90, 0, 45}
  kneeRot = {0, 0, 155}
  ankleRot = {-25, 0, 75}

  return makeLimb(name .. 'Leg', green,
                  hipRot, kneeRot, ankleRot,
                  legScale, legScale, footScale)
end

function makeLimb(name, colour,
                  startRot, midRot, endRot,
                  startScale, midScale, endScale)
  startJoint = gr.joint(name .. 'StartJoint', startRot, noRotation)
  startLimb = makeDrawableNode(name .. 'StartLimb', colour, startScale)
  startLimb:translate(0, -startScale[2], 0)
  startJoint:add_child(startLimb)

  midJoint = gr.joint(name .. 'MidJoint', midRot, noRotation)
  midJoint:translate(0, -startScale[2], 0)
  startLimb:add_child(midJoint)

  midLimb = makeDrawableNode(name .. 'MidLimb', colour, midScale)
  midLimb:translate(0, -midScale[2], 0);
  midJoint:add_child(midLimb)

  endJoint = gr.joint(name .. 'EndJoint', endRot, noRotation)
  endJoint:translate(0, -midScale[2], 0)
  midLimb:add_child(endJoint)

  endLimb = makeDrawableNode(name .. 'EndLimb', colour, endScale)
  endLimb:translate(0, -endScale[2], 0)
  endJoint:add_child(endLimb)

  return startJoint
end

function makeDrawableNode(name, material, scale)
  parent = gr.node(name .. 'Node')
  draw = gr.sphere(name .. 'Draw')
  parent:add_child(draw)
  draw:scale(unpack(scale))
  draw:set_material(material)

  return parent
end

torsoScale = {1.0, 3.0, 1.0}
torso = makeDrawableNode('Torso', cyan, torsoScale)
rootNode:add_child(torso)

shouldersScale = {1.6, 0.6, 0.6}
shoulders = makeDrawableNode('Shoulders', red, shouldersScale)
shoulders:translate(0, torsoScale[2] * 0.8, 0)
torso:add_child(shoulders)

leftArm = makeArm('Left')
rightArm = makeArm('Right')
leftArm:translate(-shouldersScale[1] * 0.9, 0, 0)
rightArm:translate(shouldersScale[1] * 0.9, 0, 0)
shoulders:add_child(leftArm)
shoulders:add_child(rightArm)

leftLeg = makeLeg('Left')
rightLeg = makeLeg('Right')
leftLeg:translate(-torsoScale[1] * 0.8, -torsoScale[2] * 0.70, 0)
rightLeg:translate(torsoScale[1] * 0.8, -torsoScale[2] * 0.70, 0)
torso:add_child(leftLeg)
torso:add_child(rightLeg)

neckRot = {0.0, 0.0, 45}
neckJoint = gr.joint('NeckJoint', neckRot, noRotation)
neckJoint:translate(0, shouldersScale[2], 0)
shoulders:add_child(neckJoint)

neckScale = {0.4, 0.6, 0.4}
neck = makeDrawableNode('Neck', blue, neckScale);
neckJoint:add_child(neck)

headTilt = {-45, 0, 75}
headRot = {-85, 0, 85}
headJoint = gr.joint('HeadJoint', headTilt, headRot)
headJoint:translate(0, neckScale[2], 0)
neck:add_child(headJoint)

headScale = {1, 1, 1}
head = makeDrawableNode('Head', green, headScale)
head:translate(0, headScale[2], 0)
headJoint:add_child(head)

noseScale = {0.2, 0.2, 0.2}
nose = makeDrawableNode('Nose', red, noseScale);
nose:translate(0, -0.2, headScale[3] - noseScale[2]/2)
head:add_child(nose)

rootNode:translate(0, 0, -20.0)

return rootNode
