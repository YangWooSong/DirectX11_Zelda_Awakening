#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;
texture2D g_Texture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;
texture2D g_NormalTexture;

texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f); //렌더타깃은 최대 8개 까지라 아끼기 위해 엠비언트 계산은 그냥 전역으로 처리
vector g_vMtrlSpecular = vector(0.6f, 0.6f, 0.6f, 1.f);
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;
texture2D g_BackTexture;
texture2D g_BloomTexture;
texture2D g_BlurXTexture;
texture2D g_BlurYTexture;

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
    if (vDepthDesc.w != 1.f)
         Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 80.f);
    else
        Out.vSpecular = 0;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_LIGHT_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vPosition = (vector) 0;

	/* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

    vector vLightDir = vPosition - g_vLightPos;

    float fAtt = saturate((g_fLightRange - length(vLightDir)) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
    vector vLook = vPosition - g_vCamPosition;

    if (vDepthDesc.w != 1.f)
        Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect) * -1.f, normalize(vLook)), 0.f), 50.f) * fAtt;
    else
        Out.vSpecular = 0;
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

    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;

	/* 1. 현재 그려내는 픽셀을 광원기준의 위치로 변환하기위해서 우선 월드로 역치환하여 월드위치를 구한다. */
    vector vPosition = (vector) 0;

	/* 투영공간상의 화면에 그려지는 픽셀의 위치를 구한다. */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / w */
    vPosition.x = In.vTexcoord.x * 2.f - 1.f;
    vPosition.y = In.vTexcoord.y * -2.f + 1.f;
    vPosition.z = vDepthDesc.x;
    vPosition.w = 1.f;

	/* 뷰스페이스 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
	/* 로컬위치 * 월드행렬 * 뷰행렬  */
    vPosition = vPosition * fViewZ;
    vPosition = mul(vPosition, g_ProjMatrixInv);

	/* 월드 상의 화면에 그려지는 픽셀의 위치를 구한다.*/
    vPosition = mul(vPosition, g_ViewMatrixInv);

	/* 2. 월드상의 픽셀 위치에다가 광원기준으로 만들어진 뷰행렬을 곱하여 광원기준의 스페이스로 변환한다. */
    vector vOldPos = mul(vPosition, g_LightViewMatrix);
    vOldPos = mul(vOldPos, g_LightProjMatrix);
	
    float fLightDepth = vOldPos.w;

    float2 vTexcoord;
    vTexcoord.x = (vOldPos.x / vOldPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vOldPos.y / vOldPos.w) * -0.5f + 0.5f;

    float fOldLightDepth = g_LightDepthTexture.Sample(LinearSampler, vTexcoord).r * 1000.f;

    if (fLightDepth - 0.1f > fOldLightDepth)
        Out.vColor = vector(Out.vColor.rgb * 0.6f, Out.vColor.a);


    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;


    Out.vColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(1.f / 1280.f * i, 0.f);
        Out.vColor += g_fWeights[i + 6] * g_BloomTexture.Sample(LinearSampler, vBlurUV);
		
    }

    Out.vColor /= 6.21f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vBlurUV = (float2) 0.f;

    for (int i = -6; i < 7; i++)
    {
        vBlurUV = In.vTexcoord + float2(0.f, 1.f / 720.f * i);
        Out.vColor += g_fWeights[i + 6] * g_BloomTexture.Sample(LinearSampler, vBlurUV);

    }

    Out.vColor /= 6.21f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBloom = g_BloomTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBlurY = g_BlurYTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = g_BackTexture.Sample(LinearSampler, In.vTexcoord) + vBloom + vBlurY ;

    return Out;
}


technique11 DefaultTechnique
{
//깊이 탐색 유무를 잘 생각하자
    pass Debug                          //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_DIRECTIONAL();
    }

    pass Light_Point //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHT_POINT();
    }

    pass Deferred //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass Final // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Blur_X //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass BlurFinal //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_FINAL();
    }
}