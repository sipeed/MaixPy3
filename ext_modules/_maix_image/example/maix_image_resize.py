from maix import image

# if padding = 0

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_240x240_240x240.jpg" % i)
t.resize(224, 224, padding=0).save("A_%d_240x240_224x224.jpg" % i)

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_240x240_240x240.jpg" % i)
t.resize(160, 120, padding=0).save("A_%d_240x240_160x120.jpg" % i)

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_240x240_240x240.jpg" % i)
t.resize(120, 160, padding=0).save("A_%d_240x240_120x160.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_224x224_224x224.jpg" % i)
t.resize(240, 240, padding=0).save("A_%d_224x224_240x240.jpg" % i)

i += 1
t = image.new(size=(320, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_320x240_320x240.jpg" % i)
t.resize(160, 120, padding=0).save("A_%d_320x240_160x120.jpg" % i)

i += 1
t = image.new(size=(160, 120), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_160x120_160x120.jpg" % i)
t.resize(320, 240, padding=0).save("A_%d_160x120_320x240.jpg" % i)

i += 1
t = image.new(size=(160, 120), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_160x120_160x120.jpg" % i)
t.resize(240, 240, padding=0).save("B_%d_160x120_240x240.jpg" % i)

i += 1
t = image.new(size=(320, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_320x240_320x240.jpg" % i)
t.resize(224, 224, padding=0).save("A_%d_320x240_224x224.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_224x224_224x224.jpg" % i)
t.resize(320, 240, padding=0).save("A_%d_224x224_320x240.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_224x224_224x224.jpg" % i)
t.resize(240, 320, padding=0).save("A_%d_224x224_240x320.jpg" % i)

i += 1
t = image.new(size=(240, 320), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_240x320_240x320.jpg" % i)
t.resize(120, 160, padding=0).save("A_%d_240x320_120x160.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_120x160_120x160.jpg" % i)
t.resize(240, 320, padding=0).save("A_%d_120x160_240x320.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_120x160_120x160.jpg" % i)
t.resize(320, 240, padding=0).save("A_%d_120x160_320x240.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_120x160_120x160.jpg" % i)
t.resize(240, 240, padding=0).save("A_%d_120x160_240x240.jpg" % i)

i += 1
t = image.new(size=(240, 320), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_240x320_240x320.jpg" % i)
t.resize(224, 224, padding=0).save("A_%d_240x320_224x224.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("A_%d_224x224_224x224.jpg" % i)
t.resize(240, 320, padding=0).save("A_%d_224x224_240x320.jpg" % i)

# default padding = 1

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_240x240_240x240.jpg" % i)
t.resize(224, 224, padding=1).save("B_%d_240x240_224x224.jpg" % i)

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_240x240_240x240.jpg" % i)
t.resize(160, 120, padding=1).save("B_%d_240x240_160x120.jpg" % i)

i = 1

t = image.new(size=(240, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_240x240_240x240.jpg" % i)
t.resize(120, 160, padding=1).save("B_%d_240x240_120x160.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_224x224_224x224.jpg" % i)
t.resize(240, 240, padding=1).save("B_%d_224x224_240x240.jpg" % i)

i += 1
t = image.new(size=(320, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_320x240_320x240.jpg" % i)
t.resize(160, 120, padding=1).save("B_%d_320x240_160x120.jpg" % i)

i += 1
t = image.new(size=(160, 120), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_160x120_160x120.jpg" % i)
t.resize(320, 240, padding=1).save("B_%d_160x120_320x240.jpg" % i)

i += 1
t = image.new(size=(160, 120), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_160x120_160x120.jpg" % i)
t.resize(240, 240, padding=1).save("B_%d_160x120_240x240.jpg" % i)

i += 1
t = image.new(size=(320, 240), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_320x240_320x240.jpg" % i)
t.resize(224, 224, padding=1).save("B_%d_320x240_224x224.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_224x224_224x224.jpg" % i)
t.resize(320, 240, padding=1).save("B_%d_224x224_320x240.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_224x224_224x224.jpg" % i)
t.resize(240, 320, padding=1).save("A_%d_224x224_240x320.jpg" % i)

i += 1
t = image.new(size=(240, 320), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_240x320_240x320.jpg" % i)
t.resize(120, 160, padding=1).save("B_%d_240x320_120x160.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_120x160_120x160.jpg" % i)
t.resize(240, 320, padding=1).save("B_%d_120x160_240x320.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_120x160_120x160.jpg" % i)
t.resize(320, 240, padding=1).save("B_%d_120x160_320x240.jpg" % i)

i += 1
t = image.new(size=(120, 160), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_120x160_120x160.jpg" % i)
t.resize(240, 240, padding=1).save("B_%d_120x160_240x240.jpg" % i)

i += 1
t = image.new(size=(240, 320), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_240x320_240x320.jpg" % i)
t.resize(224, 224, padding=1).save("B_%d_240x320_224x224.jpg" % i)

i += 1
t = image.new(size=(224, 224), color=(0xFF, 0xFF, 0xFF)).draw_circle(100, 100, 50, thickness=-1)
t.save("B_%d_224x224_224x224.jpg" % i)
t.resize(240, 320, padding=1).save("B_%d_224x224_240x320.jpg" % i)
