#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
vector g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);

texture2D g_DiffuseTexture; /* 지형 픽셀이 빛을 받으면 반사해야할 재질정보를 담은것이다. */


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

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    
    //법선벡터를 월드 좌표로 옮겨 빛 벡터랑 비교할 수 있게 함
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

    //빛의 방향벡터를 역으로 만들어 픽셀의 법선벡터와 비교 -> 코싸인 그래프와 같은 모양이 나옴
	//음영은 0~1의 값이라 음수는 0으로 만든다.
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