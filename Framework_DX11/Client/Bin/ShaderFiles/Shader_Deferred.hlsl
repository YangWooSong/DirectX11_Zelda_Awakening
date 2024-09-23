#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
texture2D g_Texture;

vector g_vLightDir;
vector g_vLightDiffuse;
vector g_vLightAmbient;
texture2D g_NormalTexture;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f); //렌더타깃은 최대 8개 까지라 아끼기 위해 엠비언트 계산은 그냥 전역으로 처리
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;

vector g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
};

PS_OUT_LIGHT PS_MAIN_LIGHT_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f; //1000f는 카메라의 far --> z가 0에서 far가 됨 (w나누기 하기 전의 z를 구하기 위해)
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord); // 선형 보간
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    //빛은 Shade 렌더 타겟에 적용
    //더하다 보면 1이 넘을 때가 있어서 saturate사용
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    
    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));

    vector vPosition = (vector) 0;
    
    /* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다.  */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;           //(Texcoord 0,0은 투영 space의 -1,1)
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;          //(Texcoord 0,0은 투영 space의 -1,1)
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv); //역행렬은 받아오는게 좋음

	/* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLook = vPosition - g_vCamPosition;

    //pow는 제곱
    Out.vSpecular = pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f);

    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDiffuse.a == 0.f)  //알파값으로 모델이 그려지지 않은 부분 잘라내기
        discard;
    
    //배경을 마젠타로 초기화 한 경우 모델이 그려지지 않은 부분 빼는 법
	/*if (vDiffuse.r == 1.f && 
		vDiffuse.g == 0.f &&
		vDiffuse.b == 1.f)
		discard;
    */
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor = vDiffuse * vShade + vSpecular;

    return Out;
}

technique11 DefaultTechnique
{
//깊이 탐색 유무를 잘 생각하자
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }
}