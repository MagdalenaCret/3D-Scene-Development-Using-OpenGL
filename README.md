{
 "project": {
   "title": "Mountain Residential Neighborhood 3D Scene",
   "subtitle": "OpenGL Graphics Processing Project",
   "author": {
     "name": "Maria-Magdalena Creț",
     "institution": "Technical University of Cluj-Napoca",
     "faculty": "Faculty of Automation and Computer Science",
     "academic_year": "2024-2025"
   }
 },
 "introduction": {
   "context": "🏗️ This project involves creating a 3D application using the OpenGL API to render a realistic scene containing multiple objects. The scene represents a residential neighborhood situated in a mountainous area, combining modern architecture with natural surroundings to create a sense of safety and comfort.",
   "scenario": "🌄 On a quiet spring morning, the residential neighborhood gradually awakens. Modern houses with contemporary architecture elegantly line the well-maintained pathways, each with its distinct personality. Building facades in tones of gray, beige, and white shine in the gentle sunlight. The buildings are imposing and modern, offering a sense of comfort. Young trees planted at equal distances along the main alley provide shade. Modern wooden benches, strategically placed at intersections, invite residents to relax. In the center of the neighborhood, a modern park with a playground awaits to come alive. The colorful swings and slides made of eco-friendly materials are still empty but will soon resonate with children's laughter. A red car is parked on the main street, waiting to embark on a journey."
 },
 "scene_modeling": {
   "creation_tool": "Blender",
   "process": "🏢 The scene was created following the description of the residential neighborhood in a mountainous zone. The scene consists of multiple objects imported as .obj files into Blender and fixed at specific aesthetic points. Each imported object underwent scaling, rotation, and translation operations. Texturing was also applied to each object.",
   "technical_details": "📐 Each object has multiple textures in .jpg or .png format. Since objects imported into OpenGL can only have one texture, the edit mode was used on each textured object with more than one texture. By pressing Ctrl+P, the object was divided into a number of objects equivalent to the number of textures, with one texture assigned to each resulting object. This allowed for correct export of the scene as .obj and proper import into OpenGL."
 },
 "implementation": {
   "development_environment": "Visual Studio with OpenGL API",
   "class_hierarchy": "📊 The project follows an object-oriented approach with classes for camera, models, shaders, and other components.",
   "camera_manipulation": {
     "initial_position": "glm::vec3(0.0f, 10.0f, 3.0f)",
     "look_at_point": "glm::vec3(0.0f, 0.0f, -20.0f)",
     "up_vector": "glm::vec3(0.0f, 1.0f, 0.0f)"
   },
   "transformations": {
     "scaling": "✂️ Modifies object dimensions by multiplying each coordinate by a scaling factor. Uniform scaling occurs when sx=sy=sz, while non-uniform scaling happens when these values differ.",
     "translation": "↔️ Moves objects from one position to another without changing shape or orientation using a translation matrix.",
     "rotation": "🔄 Rotates objects around a fixed point or axis using rotation matrices specific to each axis (x, y, or z)."
   },
   "scene_navigation": {
     "collision_detection": "⚠️ Implemented with constraints to prevent clipping through the ground and structures.",
     "mouse_control": "🖱️ Uses mouseCallback function to handle mouse movements and update camera direction.",
     "keyboard_control": "⌨️ Uses keyboardCallback to detect key events and initiate actions like movement, mode changes, and sound effects."
   }
 },
 "scene_features": {
   "visualization_modes": {
     "solid": "🧊 Objects displayed with filled faces, best for realistic viewing.",
     "wireframe": "📡 Objects displayed only by their edges, useful for examining geometric structure.",
     "polygonal": "🔷 Displays the polygons forming the object surface without lighting or smoothing effects."
   },
   "lighting_methods": {
     "ambient_light": "💡 Diffuse, omnipresent light that illuminates all surfaces uniformly.",
     "shadow_mapping": "👥 Technique for generating shadows using a shadow map that stores information about distances from the light source."
   },
   "special_effects": {
     "photorealism": "📸 Achieved through a combination of advanced lighting, texturing, and image processing techniques.",
     "fog": "🌫️ Adds opacity to the scene creating an illusion of depth, using quadratic exponential fog formula.",
     "skybox": "🌌 Two cubemaps with 6 different textures on each face, providing background images for day and night modes."
   },
   "sound_integration": "🔊 Sound effects positioned in the 3D world with their position affecting how they are heard by the user, implemented using Windows API's PlaySoundA function."
 },
 "animations": {
   "bird_animation": {
     "type": "Rotation around fixed point",
     "fixed_point_coordinates": "x=-6.1501, y=-1.9767, z=2.3055",
     "implementation": "🦅 Created by translating the object to the fixed point, performing a rotation with desired values, and translating back to the original coordinates."
   },
   "car_animation": {
     "type": "Progressive movement",
     "initial_position": "glm::vec3(0.13764f, -0.10738f, 36.677f)",
     "movement": "🚗 Implemented by incrementing the Z-coordinate gradually to create the effect of the car moving forward."
   },
   "scene_preview": "🎬 Camera rotation around a central point, allowing users to get a complete view of the scene distribution and details."
 },
 "controls": {
   "navigation": {
     "W": "Move camera forward",
     "A": "Move camera left",
     "S": "Move camera backward",
     "D": "Move camera right",
     "Q": "Rotate camera left",
     "E": "Rotate camera right"
   },
   "visualization": {
     "J": "Switch to Solid visualization mode",
     "K": "Switch to Wireframe visualization mode",
     "L": "Switch to Polygonal visualization mode"
   },
   "environment": {
     "N": "Enable day skybox",
     "M": "Enable night skybox",
     "F": "Activate fog effect",
     "G": "Deactivate fog effect"
   },
   "animations": {
     "H": "Start car animation",
     "Z": "Enable scene preview mode",
     "X": "Disable scene preview mode"
   },
   "audio": {
     "0": "Start special sound effect",
     "9": "Stop special sound effect"
   }
 },
 "screenshots": {
   "main_scene": "scene_main_view.png",
   "wireframe_mode": "scene_wireframe.png",
   "fog_effect": "scene_with_fog.png",
   "night_mode": "scene_night_skybox.png"
 },
 "conclusions": {
   "achievements": "🏆 Gained deep understanding of OpenGL graphics pipeline, geometric transformations, coordinate systems, shader programming, texture manipulation, and camera control techniques.",
   "future_improvements": [
     "🔄 Implementation of an animation system for all scene objects, allowing fluid movements and dynamic interactions.",
     "💡 Optimization of the lighting system by introducing spotlight sources to simulate more realistic lamp light.",
     "🌧️ Implementation of a particle system for atmospheric effects like rain and snow.",
     "👤 Development of a complex shadow system for more realistic rendering.",
     "⚡ Integration of a robust collision detection system for more realistic interactions between objects."
   ]
 },
 "references": [
   "📚 Graphics Processing - course and laboratory support from moodle.cs.utcluj.ro",
   "🧪 Laboratory 9 - used as template from moodle.cs.utcluj.ro",
   "🖼️ 3D models from free3d.com",
   "🎨 3D models from turbosquid.com/Search/3D-Models/free",
   "🛠️ Blender support from blender.org/support/",
   "📝 Tutorials from medium.com",
   "🎭 Textures from humus.name/index.php?page=Textures"
 ]
}
