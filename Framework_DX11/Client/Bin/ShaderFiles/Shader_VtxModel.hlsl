#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_AlphaTexture;

bool g_bInteract = false;
bool g_bNormalize = false;

float g_fAlpha = 1.f;
float g_fBright = 1.f;
float g_MoveTexCoord = 0.f;
float4 g_fColor = { 1.f, 1.f, 1.f,1.f };
float4 g_fTextureScale= 1.f;

int g_FireType = 0;

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
    float4 vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN( /*����*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct VS_OUT_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;        //�ȼ� ����z��
    float2 vTexcoord : TEXCOORD0;   
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;      //�ȼ� ���� x��
    float3 vBinormal : BINORMAL;    //�ȼ��� ���� ���� y�� 
};

VS_OUT_NORMAL VS_MAIN_NORMAL(VS_IN In)
{
    VS_OUT_NORMAL Out = (VS_OUT_NORMAL) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix)).xyz;
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = normalize(cross(Out.vNormal, Out.vTangent));

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vGlow : SV_TARGET3;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vDiffuse.a)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    
    return Out;
}


PS_OUT PS_MAIN_NONSPECULAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vDiffuse.a)
        discard;

    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_OWLSTATUE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vDiffuse.a)
        discard;
    
    if (g_bInteract)
    {
        vDiffuse.rg *= 1.3f;
        vDiffuse.b *= 4.f;
    }
    
    
    Out.vDiffuse = vDiffuse;
     Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
     Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
        return Out;
}

PS_OUT PS_MAIN_AlphaChange(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vDiffuse.a)
        discard;
    
    vDiffuse.a = g_fAlpha;
    vDiffuse.rgb *= g_fBright;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_3DEFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vDiffuse.a)
        discard;
    
    vDiffuse.rgb = g_fColor.rgb;
    vDiffuse.a = g_fAlpha;
   // vDiffuse.rgb *= g_fBright;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_HALO(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
     // �ؽ�ó ��ǥ �̵� (��: (0.1, 0.1)��ŭ �̵�)
    float2 offset = float2(0.f, g_MoveTexCoord);
    float2 movedTexcoord = In.vTexcoord + offset; 
    
    vector vDiffuse;
    vector vAlpha;

    vDiffuse = g_DiffuseTexture.Sample(LinearSampler, movedTexcoord);
    vAlpha = g_AlphaTexture.Sample(LinearSampler, In.vTexcoord);
    
    vDiffuse.a = vAlpha.r;
    vDiffuse.a *= vAlpha.a;
   
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}


PS_OUT PS_MAIN_RING(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
     // �ؽ�ó ��ǥ �̵� (��: (0.1, 0.1)��ŭ �̵�)
    float2 offset = float2(g_MoveTexCoord, g_MoveTexCoord);
    float2 Texcoord = float2(In.vTexcoord.y, In.vTexcoord.x);
    float2 movedTexcoord = Texcoord + offset;
    
    vector vDiffuse;
    vector vAlpha;

    vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vAlpha = g_AlphaTexture.Sample(LinearSampler, movedTexcoord);
    
    vDiffuse.a = vAlpha.r;
    vDiffuse.a *= vAlpha.a;
   
    vDiffuse.rgb = g_fColor.rgb;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
     // �ؽ�ó ��ǥ �̵� (��: (0.1, 0.1)��ŭ �̵�)
    float2 offset = float2(0.f,g_MoveTexCoord);
    float2 MovedTexcoord = In.vTexcoord + offset; 
    
    float2 offset2 = float2(g_MoveTexCoord,0.f);
    float2 MovedTexcoord2 = In.vTexcoord + offset2;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, MovedTexcoord);
    vector vAlpha;

   // vAlpha = g_AlphaTexture.Sample(LinearSampler, DiffuseMovedTexcoord);
    
    vDiffuse.rgb = g_fColor.rgb;
    vDiffuse.a *= 1 - g_MoveTexCoord*3.f;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_CUBEHALF(PS_IN In) //�� �̵�, ����� �����ϰ� 
{
    PS_OUT Out = (PS_OUT) 0;
	
     // �ؽ�ó ��ǥ �̵� (��: (0.1, 0.1)��ŭ �̵�)
    float2 offset = float2(g_MoveTexCoord , 0.f);
    float2 movedTexcoord = In.vTexcoord + offset;
    
    vector vDiffuse;
    vector vAlpha;

    vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vAlpha = g_AlphaTexture.Sample(LinearSampler, movedTexcoord);
    
    vDiffuse.a = 1.f - vAlpha.a; //��Ϥ����� �����ϰ�
   
    vDiffuse.rgb = g_fColor.rgb;;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 1.f);
    
    return Out;
}

PS_OUT PS_MAIN_Slash(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 scaledTexCoord = In.vTexcoord * g_fTextureScale;
    
    vector vAlpha = g_AlphaTexture.Sample(LinearSampler, scaledTexCoord);
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (0.3f >= vDiffuse.a)
        discard;
    
    vDiffuse.a = vAlpha.a;
    vDiffuse.rgb = g_fColor.rgb;
    
    Out.vDiffuse = vDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //(���� Space�� Z��(W�����⸦ ��->2D�� ��ȯ), ����ȭ�� Z��, ������ ��)
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vGlow = vDiffuse;
    return Out;
}


struct PS_IN_NORMAL
{
    float4 vPosition : SV_POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

PS_OUT PS_MAIN_NORMAL_WORLD(PS_IN_NORMAL In)
{
    PS_OUT Out = (PS_OUT) 0;

    // ��ǻ�� �ؽ�ó ���ø�
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    // ������Ʈ �Ǵ� ���� ���� ������ ��ָ� ���ø�
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

    // [-1, 1] ������ ��ȯ (�̹� ������Ʈ/���� �����̹Ƿ� �߰� ��ȯ ���ʿ�)
   // float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    float3 vNormal;
  
    //z���� ������ش�
    if (g_bNormalize == false)
        vNormal.xy = vNormalDesc.xy ;
    else
        vNormal.xy = vNormalDesc.xy * 2.f - 1.f;
    
    vNormal.z = sqrt(1 - saturate(dot(vNormal.xy, vNormal.xy)));
    vNormal.xyz = mul(vNormal, WorldMatrix);
    
    // ���� �׽�Ʈ
    if (0.3f >= vDiffuse.a)
        discard;

    // ��ǻ�� ���
    Out.vDiffuse = vDiffuse;

    // ��� ���͸� �ٽ� [0, 1]�� ��ȯ�Ͽ� ���
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    // ���� �� ���
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vGlow = vDiffuse;
    return Out;
}

PS_OUT PS_MAIN_NORMAL(PS_IN_NORMAL In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord); //���û��� normal����(0~1 ���̷� �����Ǿ� �ִ�)
    
	/* ���û��� ��ȯ���� ���� �븻���͸� ���Ѵ�.(-1~1 ���̷� ). */
	/* ���ý����̽� => �����Ƿ��ý����̽�(x), ��ֺ��ʹ� �ϳ��� ���ý����̽��� ���������� �����ߴ�. */
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal);

    vNormal = normalize(mul(vNormal, WorldMatrix));

    if (0.3f >= vDiffuse.a)
        discard;

    Out.vDiffuse = vDiffuse;

	/* -1.f ~ 1.f -> 0.f ~ 1.f */
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f); //�ٽ� 0~1 ���̷� ����    
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
  
    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.w / 1000.f, 0.f, 0.f, 0.f);


    return Out;
}

technique11 DefaultTechnique
{
    pass Model //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_Wire //1
    {
        SetRasterizerState(RS_Wireframe);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_NonSpecular //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NONSPECULAR();
    }

    pass Model_OwlStatue //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_OWLSTATUE();
    }

    pass NormalMapping //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL();
    }

    pass WorldNormalMapping //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_NORMAL();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_NORMAL_WORLD();
    }

    pass Model_ChangeAlpha //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_AlphaChange();
    }


    pass Model_Halo//7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_HALO();
    }

    pass Model_Ring//8
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RING();
    }

    pass Model_CubeHalf //9
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_CUBEHALF();
    }

    pass Model_3D_EFFECT //10
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_3DEFFECT();
    } 

    pass Model_3D_Fire //11
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FIRE();
    }

    pass Model_3D_Slash //12
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Slash();
    }


    pass LightDepth //13
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

}