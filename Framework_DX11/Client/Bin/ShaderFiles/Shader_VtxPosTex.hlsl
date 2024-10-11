#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* 정점하나당 어떤 변환을 거쳐가면 되는지를 직접 연산을 통해 구현. */
/* 정점 쉐이더는 정점 하나를 받아오고. 연산을 거친후 연산을 마친 정점을 리턴해준다. */
/* 사각형버퍼를 그린다. 6번호출. */
// VS_MAIN(/*정점*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DissolveTexture;
bool g_bBlink = false, m_bDissolve = false;
float g_fBrightness = 1.f;
float g_fAlpha = 1.f;
float g_fDissolve = 1.f;    //흰색부터 없앨거임
float g_fAngle = 0.f;
float4 g_fColor = { 1.f, 1.f, 1.f, 1.f };
float4 g_fOutLineColor = { 1.f, 1.f, 1.f, 1.f };

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
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

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

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
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;
    
    if (g_bBlink == true)
    {
        Out.vColor.rg = g_fBrightness;
        Out.vColor.b = 0.6f + g_fBrightness;
    }

    return Out;
}

PS_OUT PS_MAIN_LAWALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;
    if (Out.vColor.r < 0.3f)
        Out.vColor.a *= 0.8f;
    return Out;
}

PS_OUT PS_MAIN_CHANGEALPHA(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;

    Out.vColor.a *= g_fAlpha;
    Out.vColor.rgb = g_fColor;
    
    return Out;
}

PS_OUT PS_MAIN_CAMPUSUI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;

    Out.vColor.a *= g_fAlpha;
    
    return Out;
}

PS_OUT PS_MAIN_FillColor(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;

    Out.vColor.r = g_fColor.x;
    Out.vColor.g = g_fColor.y;
    Out.vColor.b = g_fColor.z;
    Out.vColor.a *= g_fColor.w;

    
    return Out;
}

PS_OUT PS_MAIN_BLACK_TO_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.2 >= Out.vColor.a)
        discard;

    if (Out.vColor.a < 0.8 || Out.vColor.r < 0.3f)
    {
        Out.vColor.r = g_fColor.x;
        Out.vColor.g = g_fColor.y;
        Out.vColor.b = g_fColor.z;
    }

    Out.vColor.a *= max(0, g_fColor.w);
    
    return Out;
}

PS_OUT PS_MAIN_SMOKE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
     //알파 테스트
    if (0.2 >= Out.vColor.a) 
        discard;

    float4 Dissolve;
  
    if (m_bDissolve)
    {
        Dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
        
        if (g_fDissolve <= Dissolve.r)//흰색부터 없앨거임
            discard;
        if (g_fDissolve <= Dissolve.r + 0.15f)//흰색부터 없앨거임
            Out.vColor.rgb *= g_fOutLineColor.rgb;
    }
   
    //색 변경
        if (Out.vColor.r > 0.3f)
        {
            Out.vColor.r *= g_fColor.x;
            Out.vColor.g *= g_fColor.y;
            Out.vColor.b *= g_fColor.z;
        }

    
   // Out.vColor.a *= max(0, g_fColor.w);
    
        return Out;
    }

PS_OUT PS_MAIN_WHITE_TO_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float4 Dissolve;
  
    if (m_bDissolve)
    {
        Dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
        
        if (g_fDissolve <= Dissolve.r)//흰색부터 없앨거임
            discard;
    }
    
    //알파 테스트
    if (0.2 >= Out.vColor.a) 
        discard;

    //색 변경
    if (Out.vColor.r > 0.3f)
    {
        Out.vColor.r *= g_fColor.x;
        Out.vColor.g *= g_fColor.y;
        Out.vColor.b *= g_fColor.z;
    }

    
   // Out.vColor.a *= max(0, g_fColor.w);
    
    return Out;
}

PS_OUT PS_MAIN_HP(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a <= 0.2f)
        discard;
    
    float2 uv = In.vTexcoord;
    float2 center = (0.5f,0.5f);
    
    // 상대 좌표 계산
    float2 relativePos = uv - center;
    
    float angle = -atan2(relativePos.x, relativePos.y);
    
    ////angle을 0에서 360사이로 변환
    //if (angle < 0.0f)
    //{
    //    angle += 2.0f * 3.14159f; // 360°에 해당하는 2π를 더해줍니다.
    //}
    
    ////라디안 변환해서 비교 (angle은 라디안)
    //if (angle < g_fAngle * (3.14159f / 180.f))
    //    Out.vColor.rgb *= 1.f;
    //else
    //    Out.vColor.rgb *= 3.f;
    
    float ResultAngle = (angle + 3.14159f) / (3.14159f * 2);
    
    if (ResultAngle < g_fAngle / 360.f)
        Out.vColor.rgb *= 1.f;
    else
        Out.vColor.rgb *= 3.f;
    

    
    return Out;
}

technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
    pass UI //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass UI_WIRE //1
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Background //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LowAlpha //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LAWALPHA();
    }

    pass ChangeAlpha //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CHANGEALPHA();
    } 

    pass HP //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HP();
    }

    pass FillColor //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FillColor();
    }

   pass BlackToColor //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLACK_TO_COLOR();
    }
      pass WhiteToColor //8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WHITE_TO_COLOR();
    }

    pass Smoke //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SMOKE();
    }
    pass CampusUI //10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CAMPUSUI();
    }

	
}