# CGProject

The Computer Graphics project

Current issues:




To add (mandatory):
-moving fish
-sea plants
-air bubbles (could be added as particles?)
-fog
-terrain


Current issues:
-The code is messy due to attempted implementation of water reflectivity that did not work, should probably remove these chunks of code (all of the framebuffers are related to this)
-Due to the above the code actually attempts rendering three times each frame
-Lots of random planets/asteroids are rendered as a placeholder from previous versions
-There is nothing preventing the submarine from going out of the water in any direction (it is effectively still a spaceship)
-It should be possible to add a simple underwater effect by rendering a semitransparent blue quad blocking the camera
