cbuffer _11_13 : register(b0)
{
    row_major float4x4 _13_m0 : packoffset(c0);
    row_major float4x4 _13_m1 : packoffset(c4);
    row_major float4x4 _13_m2 : packoffset(c8);
};

uniform float4 gl_HalfPixel;

static float4 gl_Position;
static float3 _36;
static float4 _46;
static float4 _48;
static float2 _52;
static float2 _54;

struct SPIRV_Cross_Input
{
    float3 _36 : TEXCOORD0;
    float4 _48 : TEXCOORD1;
    float2 _54 : TEXCOORD2;
};

struct SPIRV_Cross_Output
{
    float4 _46 : TEXCOORD0;
    float2 _52 : TEXCOORD1;
    float4 gl_Position : POSITION;
};

void vert_main()
{
    gl_Position = mul(float4(_36, 1.0f), mul(_13_m0, mul(_13_m1, _13_m2)));
    _46 = _48;
    _52 = _54;
    gl_Position.x = gl_Position.x - gl_HalfPixel.x * gl_Position.w;
    gl_Position.y = gl_Position.y + gl_HalfPixel.y * gl_Position.w;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    _36 = stage_input._36;
    _48 = stage_input._48;
    _54 = stage_input._54;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._46 = _46;
    stage_output._52 = _52;
    return stage_output;
}
