mat1 = gr.material({0.7, 0.45, 0.9}, {0.6, 0.6, 0.6}, 10)
mat2 = gr.material({0.2, 0.49, 0.2}, {0.9, 0.2, 0.1}, 15)
mat3 = gr.material({0.9, 0.78, 0.35}, {0.6, 0.6, 0.6}, 12)
mat4 = gr.material({0.7, 0.25, 0.2}, {0.9, 0.9, 0.9}, 22)
mat5 = gr.material({0.6, 0.1, 0.0}, {0.4, 0.4, 0.4}, 1)
mats = {mat1, mat2, mat3, mat4, mat5}

root = gr.node('root')

local makeFractalSphere
makeFractalSphere = function(maxDepth, scale, depth)
  local parentNode = gr.node('fractalparent' .. depth)
  local node = gr.sphere('fractal' .. depth)
  parentNode:add_child(node)

  node:set_material(mats[depth])
  node:scale(scale, scale, scale)

  if depth == maxDepth then
    return parentNode
  end

  -- Get sub fractals (5 of them)
  local s1 = makeFractalSphere(maxDepth, scale / 2.0, depth + 1)
  s1:translate(scale*3/2, 0, 0)
  s1:rotate('z', -90)
  local s2 = makeFractalSphere(maxDepth, scale / 2.0, depth + 1)
  s2:translate(-scale*3/2, 0, 0)
  s2:rotate('z', 90)
  local s3 = makeFractalSphere(maxDepth, scale / 2.0, depth + 1)
  s3:translate(0, scale*3/2, 0)
  local s4 = makeFractalSphere(maxDepth, scale / 2.0, depth + 1)
  s4:translate(0, 0, -scale*3/2)
  s4:rotate('x', -90)
  local s5 = makeFractalSphere(maxDepth, scale / 2.0, depth + 1)
  s5:translate(0, 0, scale*3/2)
  s5:rotate('x', 90)

  for k, v in pairs({s1, s2, s3, s4, s5}) do
    parentNode:add_child(v)
  end

  return parentNode
end

fractals = {
  {{12, 12, -5}, 14},
  {{-12, -12, -5}, 60},
  {{12, -12, -5}, 0},
  {{-12, 12, -5}, 78}
}

fractalSphere = makeFractalSphere(3, 4, 1)

for k,l in pairs(fractals) do
  parent = gr.node('hello')
  parent:add_child(fractalSphere)
  parent:translate(unpack(l[1]))
  parent:rotate('x', 25)
  parent:rotate('y', 25)
  parent:rotate('z', l[2])
  root:add_child(parent)
end

require('buckyball')

root:add_child(buckyball)
buckyball:set_material(mat4)
buckyball:scale(4, 2, 3)
buckyball:translate(0, 1, 0)

cube = gr.cube('randomcube')
cube:set_material(mat5)
cube:translate(0, -8, 0)
cube:scale(5, 5, 5)
cube:rotate('y', 15)
cube:rotate('x', 10)
root:add_child(cube)

l1 = gr.light({100, 0, 100}, {0.8, 0.8, 0.8}, {1, 0, 0})
l2 = gr.light({0, 0, 100}, {0.2, 0.2, 0.2}, {1, 0, 0})
lights = {
  l1,
  l2,
}

plane = gr.mesh('plane', {
  {-40, 0, -40},
  {40, 0, -40},
  {40, 0, 40},
  {-40, 0, 40},
}, {
  {3, 2, 1, 0}
})
planeMat = gr.material({1, 0.45, 0.0}, {0,0,0}, 1)
plane:rotate('x', 15)
plane:set_material(planeMat)
plane:translate(0, -25, 0)
root:add_child(plane)

SIZE = 128
gr.render(root, 'sample.png', SIZE, SIZE,
	  {0, 0, 40}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.4, 0.4, 0.4}, lights)
