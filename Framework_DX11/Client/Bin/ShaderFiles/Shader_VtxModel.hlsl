
/* float2 float3 float4 == vector */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

texture2D g_DiffuseTexture;

RasterizerState rsWireframe
{
    FillMode = WireFrame;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = WRAP;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN( /*Á¤Á¡*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
	
    Out.vColor = g_vLightDiffuse * g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * fShade;

    return Out;
}



technique11 DefaultTechnique
{
    pass Model
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_Wire
    {
        SetRasterizerState(rsWireframe);
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}