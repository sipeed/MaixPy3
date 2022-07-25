from maix import image

img = image.open("test_crop_affine.jpg")

print(img.size)

tmp = img.crop_affine([204, 205, 64, 202, 68, 154], 94, 24)

tmp.save('test_crop_affine.png')
