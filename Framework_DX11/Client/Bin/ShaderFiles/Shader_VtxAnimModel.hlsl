#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.0f);
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

bool g_bIsRed = false, g_bOutBodyIsRed = false, g_bRedBlink = false;

/* ����ĵ�(�� �� ��ü�� ����ĵ�(x), ���� �׸��� �޽ÿ��� ������ �ִ� ����ĵ�(o) */
matrix g_BoneMatrices[512];

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;

    uint4 vBlendIndices : BLENDINDEX;
    float4 vBlendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    float fWeightW = 1.f - (In.vBlendWeights.x + In.vBlendWeights.y + In.vBlendWeights.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndices.x] * In.vBlendWeights.x +
		g_BoneMatrices[In.vBlendIndices.y] * In.vBlendWeights.y +
		g_BoneMatrices[In.vBlendIndices.z] * In.vBlendWeights.z +
		g_BoneMatrices[In.vBlendIndices.w] * fWeightW;
		
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
    
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
};

PS_OUT PS_MAIN_NONRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f > vMtrlDiffuse.a)
        discard;

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

    return Out;
}

PS_OUT PS_MAIN_CHANGE_RED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.3f >= vMtrlDiffuse.a)
        discard;

    if (g_bIsRed)
    {
        vMtrlDiffuse.r *= 2.3f;
        vMtrlDiffuse.g *= 0.6f;
        vMtrlDiffuse.b *= 0.6f;
    }
    else if (g_bRedBlink)
    {
        vMtrlDiffuse.r *= 1.5f;
    }
    else if (g_bOutBodyIsRed)
    {
        if (vMtrlDiffuse.b < 0.14f)   
            vMtrlDiffuse.r *= (1.6f - vMtrlDiffuse.b * 4.f);
    }
   
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    return Out;
}



technique11 DefaultTechnique
{
    pass AnimModel
    {
//3�� �� �־����
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
//���� ���ϰ� ������ ���Ϳ� �־��, rgba�� ���´�~ �ϳ�Ȥ�� ��� �������� ���Ķ�~
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONRED();
    }

    pass AnimModelRed
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHANGE_RED();
    }
}