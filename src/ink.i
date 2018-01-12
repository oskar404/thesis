#
# This is an example file of ink structure
#
# Comments starts always with a character '#'
#
#
# Pixel size tells the size of the pixels in matrix
#       pixel size is in micro meters
pixel_size    20.0
#
# Pixel size should be before image type specifications
#
#
# Splitting coefficient
splitting 0.5
#
#
# Deposition coefficient
deposition 0.5
#
#
# Absorption coefficient
Absorption 0.9
#
#
# Color tells the color of the ink
Ambient      0.2
380 0.0001
780 0.0001
#
#            scale  angle (max Pi/2)
Specular     0.8   0.05
380 0.5
780 0.5
#
#
Diffuse     0.2
380 0.0001
780 0.0001
#
#
#
#
#
# convolution matrix
#convolution 1 1
#1
convolution 9 9
0.000   0.000   0.006   0.006   0.006   0.006   0.006   0.000   0.000
0.000   0.006   0.006   0.013   0.013   0.013   0.006   0.006   0.000
0.006   0.006   0.013   0.019   0.019   0.019   0.013   0.006   0.006
0.006   0.013   0.019   0.032   0.039   0.032   0.019   0.013   0.006
0.006   0.013   0.019   0.039   0.065   0.039   0.019   0.013   0.006
0.006   0.013   0.019   0.032   0.039   0.032   0.019   0.013   0.006
0.006   0.006   0.013   0.019   0.019   0.019   0.013   0.006   0.006
0.000   0.006   0.006   0.013   0.013   0.013   0.006   0.006   0.000
0.000   0.000   0.006   0.006   0.006   0.006   0.006   0.000   0.000
#
#
# Location of picture printed with ink in millimeters
#
location 1.5 1.5
#
#
# Image Type Size
# Size of box in millimeters
# Size matrix is number of columns and rows
#image DOT 2.0
image BOX 2 2 1.0
#image MATRIX 11 11 1.0
#00000100000
#00001110000
#00011111000
#00111011100
#01110001110
#11100000111
#01110001110
#00111011100
#00011111000
#00001110000
#00000100000
#
#
#
#
#
#
