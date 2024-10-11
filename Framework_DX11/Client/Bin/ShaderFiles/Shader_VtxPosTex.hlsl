#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* �����ϳ��� � ��ȯ�� ���İ��� �Ǵ����� ���� ������ ���� ����. */
/* ���� ���̴��� ���� �ϳ��� �޾ƿ���. ������ ��ģ�� ������ ��ģ ������ �������ش�. */
/* �簢�����۸� �׸���. 6��ȣ��. */
// VS_MAIN(/*����*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DissolveTexture;
bool g_bBlink = false, m_bDissolve = false;
float g_fBrightness = 1.f;
float g_fAlpha = 1.f;
float g_fDissolve = 1.f;    //������� ���ٰ���
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
	/* ���� �ؾ��� ������ ��� ���ĳ��� ���̹Ƿ� ���� dx�� �߰������� �ؾ��� �̸��� �ض�. */
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
	
};

/* 1. ������ ��ȯ������ �����Ѵ�. */
/* 2. ������ ���� ������ �����Ѵ�. */
VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

	/* ������ ��ġ�� ���� �� ������ȯ�Ѵ�.*/		
	/* Ŭ���̾�Ʈ���� ���������� �����ϴ� TrnasformCoord�Լ��ʹ� �ٸ��� */
	/* mul�Լ��� ��쿡�� �����ϰ� ����� ���ϱ⸸ ������ �ϰ� w�����⿬����ü�� �������� �ʴ´�. */
    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

/* Triangle : ���� ������ �� vs_main�� ����Ҷ����� ��� */
/* ������ ��δ� ����Ǹ�. ���� ������ ����. */
/* ���ϵ� ������ w�� ������ xyzw�� ������. ���� */
/* ������ ��ġ�� ����Ʈ�� ��ȯ��. (��������ǥ�� ��ȯ�Ѵ�)*/
/* �����Ͷ����� : ���������� ��������Ͽ� �ȼ��� ���������. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};

/* 1. �ȼ��� �������� ������ �����Ѵ�. */
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
    
     //���� �׽�Ʈ
    if (0.2 >= Out.vColor.a) 
        discard;

    float4 Dissolve;
  
    if (m_bDissolve)
    {
        Dissolve = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
        
        if (g_fDissolve <= Dissolve.r)//������� ���ٰ���
            discard;
        if (g_fDissolve <= Dissolve.r + 0.15f)//������� ���ٰ���
            Out.vColor.rgb *= g_fOutLineColor.rgb;
    }
   
    //�� ����
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
        
        if (g_fDissolve <= Dissolve.r)//������� ���ٰ���
            discard;
    }
    
    //���� �׽�Ʈ
    if (0.2 >= Out.vColor.a) 
        discard;

    //�� ����
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
    
    // ��� ��ǥ ���
    float2 relativePos = uv - center;
    
    float angle = -atan2(relativePos.x, relativePos.y);
    
    ////angle�� 0���� 360���̷� ��ȯ
    //if (angle < 0.0f)
    //{
    //    angle += 2.0f * 3.14159f; // 360�ƿ� �ش��ϴ� 2�� �����ݴϴ�.
    //}
    
    ////���� ��ȯ�ؼ� �� (angle�� ����)
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
	/* ������ + ������Ʈ + ssao + ��ָ��� + pbr*/
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