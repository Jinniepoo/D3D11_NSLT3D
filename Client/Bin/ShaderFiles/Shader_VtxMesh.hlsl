
#include "Engine_Shader_Defines.hlsli"

float4x4    g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D   g_DiffuseTexture;
texture2D   g_NormalTexture;

float3      g_CameraPlayerPosition;
float       g_EffectDistance;
bool        g_bIsNear;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL; /* �ȼ��� ���� z���� ����� ��ȯ��Ų��  */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT; /* �ȼ��� ���� x���� ����� ��ȯ��Ų��  */
    float3 vBinormal : BINORMAL; /* �ȼ��� ���� y���� ����� ��ȯ��Ų��  */
};


// ���ϰ� VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : ���� ���̴����� ó���ؾ��� ��� ������ �������� �����δ� �ϰ� �˾Ƽ� �ؾ��� ������ �����ض�. */

/* VertexShader */
/* 1. ������ �⺻���� ��ȯ(����, ��, ����)�� �����Ѵ�.: ���ý����̽����� ���������� ������ı��� ���ؼ� ó�����ִ� ������ �ʿ��ϴ� . */
/* 2. ������ ������ �����Ѵ�.  */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = cross(Out.vNormal.xyz, Out.vTangent);

    return Out;
}

/* Triangle List�� �׸��� �ִ� ���, ���� ������ ���� ���̴��� ����� ������ ������ ������ ��ġ��. */
/* w������ ������ ���ؼ� ������ �ش��ϴ� ������ ��ġ��. �������� ���������̽�(-1, 1 ~ 1, -1) �� �����ϳ�. */
/* ����Ʈ �����̽��� ��ȯ�Ѵ�. �������� ������ ��ǥ����(0, 0 ~ winsizex, winsizey)�� �����Ѵ�.  */
/* ��������� �����ϰԵȴ�. : ���� ������ �����Ͽ� �ȼ��� ������ �����Ѵ�. */
/* ������ �ȼ��� �ȼ� ���̴��� ������ �ȼ��� ���� �����Ѵ�. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL; /* �ȼ��� ���� z���� ����� ��ȯ��Ų��  */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT; /* �ȼ��� ���� x���� ����� ��ȯ��Ų��  */
    float3 vBinormal : BINORMAL; /* �ȼ��� ���� y���� ����� ��ȯ��Ų��  */
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPickedDepth : SV_TARGET3;
};

/* �ȼ��� ���� �����ϳ�. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(�������� �����ðǵ�?, ��� ���� �����ðǵ�?);
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

	/* �ȼ��� ����(ź��Ʈ�����̽�)�� ���ǵǾ��ִ� �븻�̴ϱ� */
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	/* �ȼ��� ������ xyz���� ����� ��ȯ��Ű�� xyz�� ����� �࿡ ��������� ���庯ȯ����̴�. */
	/* In.vNormal �ȼ������� z���� ����κ�ȯ��Ų��.. == ������ ����� ���������ؼ� �����°ſ�.  */
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

	/* �����׽�Ʈ �� �̻� �������������� �����Ѵ�(x) */
    if (Out.vDiffuse.a <= 0.1f)
        discard;

    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    
    if (true == g_bIsNear)
    {
        Out.vDiffuse.a *= 0.3f;
    }

    return Out;
}

struct VS_OUT_LIGHTDEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0; 
};

VS_OUT_LIGHTDEPTH VS_MAIN_LIGHTDEPTH(VS_IN In)
{
    VS_OUT_LIGHTDEPTH Out = (VS_OUT_LIGHTDEPTH) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vProjPos = Out.vPosition;
  
    return Out;
}

struct PS_IN_LIGHTDEPTH
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_LIGHTDEPTH
{
    float4 vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN_LIGHTDEPTH In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);

    return Out;
}

technique11 DefaultTechnique
{
	/* Pass : ���̴� ����� ĸ��ȭ (��������� ��Ƴ���) */

	/* ������ + ������Ʈ + �׸��� + ��Ż�� */
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_LIGHTDEPTH();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

}

