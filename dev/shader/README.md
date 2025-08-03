The default shader language we use is glsl.

* HLSL shader model: SM 6.0

Current Workflow:
1. Write glsl shaders, e.g. Shader.frag
2. Use glslc to convert glsl format shaders to spirv format, e.g. Shader.frag -> Shader.frag.spirv
   to spv: glslc <glsl_shader_path> -o <spv_shader_path>
3. Use spirv-cross to convert spirv format shaders to all the other formats, e.g. Shader.frag.spirv -> Shader.frag.glsl
   to hlsl: spirv-cross <spv_shader_path> --hlsl --shader-model 60 --output <hlsl_shader_path>
