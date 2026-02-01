texconv correct.png --premultiplied-alpha -m 1 -f BC3_UNORM
texconv cat.png --premultiplied-alpha -m 1 -f BC3_UNORM
texconv sunset.jpg --premultiplied-alpha -m 1 -f BC3_UNORM
texconv shipanimated.png --premultiplied-alpha -m 1 -f BC3_UNORM
texconv starfield.png --premultiplied-alpha -m 1 -f BC3_UNORM
texconv rocks.jpg --premultiplied-alpha -m 1 -f BC3_UNORM
# The tangent-space normal map used here was generated from a height map using texconv's --normal-map feature.
# texconv rocks_NM_height.dds --normal-map l --normal-map-amplitude 4
texconv earth.bmp --premultiplied-alpha -m 1 -f BC3_UNORM