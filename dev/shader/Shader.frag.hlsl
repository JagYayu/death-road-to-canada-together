uniform sampler2D _13;

static float2 _17;
static float4 _21;
static float4 _23;

struct SPIRV_Cross_Input
{
    float4 _23 : TEXCOORD0;
    float2 _17 : TEXCOORD1;
};

struct SPIRV_Cross_Output
{
    float4 _21 : COLOR0;
};

void frag_main()
{
    _21 = _23 * tex2D(_13, _17);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    _17 = stage_input._17;
    _23 = stage_input._23;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._21 = float4(_21);
    return stage_output;
}
