#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

texture2D g_DiffuseTexture; /* ���� �ȼ��� ���� ������ �ݻ��ؾ��� ���������� �������̴�. */


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    
    //�������͸� ���� ��ǥ�� �Ű� �� ���Ͷ� ���� �� �ְ� ��
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

    //���� ���⺤�͸� ������ ����� �ȼ��� �������Ϳ� �� -> �ڽ��� �׷����� ���� ����� ����
	//������ 0~1�� ���̶� ������ 0���� �����.
    float fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);
	
    Out.vColor = g_vLightDiffuse * g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord * 30.f) * fShade;

    return Out;
}



technique11 DefaultTechnique
{
    pass Terrain
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}