﻿Students:
	Vincent Fabioux
	Olivier Léobal

Week: 6

OBJ and MTL files can now be loaded. We can also define a material for the
whole object (color, diffuse, specular, etc).

Generating a picture using "dolphins.yaml" may take more than one hour due
to the complexity of the object mesh. This means that runme.bat may also
take more than one hour before completion.

For very long renders (such as dolphins.yaml), a progression indicator has been
added ; its step (in percentage of completion) can be specified using the
PrintProgression parameter.


Depth of field:
	We added Depth of Field support, but not through the method suggested.
	Instead, we use the sprite scattering method : we overlay a circular
	sprite depending on the depth representing the circle of confusion for
	each pixel, as explained in this article :
	http://www.adriancourreges.com/blog/2017/12/15/mgs-v-graphics-study/
	
	Unfortunately, we cannot seemingly cannot get our implementation to
	produce realistic images, although the implementation seems 
	fundamentally sound. It might be just that the focal length/aperture
	diameter aren't selected right, but I believe it might be the
	projection of absolute length circles into circles that is lacking
	

Parallelism :
	We added OpenMP support in a few parts of the program ( most notably
	scene.cpp:260).
	
	This unfortunately produces an erroneous (although cool-looking)
	output when combined with DoF
