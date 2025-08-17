
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
    float4 vNormal : NORMAL; /* 픽셀의 로컬 z축을 월드로 변환시킨거  */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT; /* 픽셀의 로컬 x축을 월드로 변환시킨거  */
    float3 vBinormal : BINORMAL; /* 픽셀의 로컬 y축을 월드로 변환시킨거  */
};


// 리턴값 VS_MAIN(float3	vPosition : POSITION, float2 vTexcoord : TEXCOORD)
/* IA : VertexBuffer(4) + IndexBuffer(6 -> 012023) */
/* SV_ : Shader Value : 내가 셰이더에서 처리해야할 모든 연산을 끝냈으니 앞으로는 니가 알아서 해야할 과정을 수행해라. */

/* VertexShader */
/* 1. 정점의 기본적인 변환(월드, 뷰, 투영)을 수행한다.: 로컬스페이스상의 정점정보를 투영행렬까지 곱해서 처리해주는 과정이 필요하다 . */
/* 2. 정점의 구성을 변경한다.  */
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

/* Triangle List로 그리고 있는 경우, 정점 세개가 정점 쉐이더를 통과할 때까지 모으는 과정을 거치낟. */
/* w나누기 연산을 통해서 투영에 해당하는 과정을 거치낟. 정점들은 투영스페이스(-1, 1 ~ 1, -1) 상에 존재하낟. */
/* 뷰포트 스페이스로 변환한다. 정점들은 윈도우 좌표공간(0, 0 ~ winsizex, winsizey)에 존재한다.  */
/* 래스라이즈를 수행하게된다. : 정점 정보를 보간하여 픽셀의 정보를 생성한다. */
/* 생성된 픽셀을 픽셀 쉐이더로 던져서 픽셀의 색을 결정한다. */

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL; /* 픽셀의 로컬 z축을 월드로 변환시킨거  */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT; /* 픽셀의 로컬 x축을 월드로 변환시킨거  */
    float3 vBinormal : BINORMAL; /* 픽셀의 로컬 y축을 월드로 변환시킨거  */
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPickedDepth : SV_TARGET3;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	// Out.vColor = g_Texture.Sample(어떤방식으로 가져올건데?, 어디에 색을 가져올건데?);
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);

	/* 픽셀의 로컬(탄젠트스페이스)에 정의되어있는 노말이니까 */
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;

	/* 픽셀의 로컬의 xyz축을 월드로 변환시키고 xyz를 행렬의 행에 집어넣으면 월드변환행렬이다. */
	/* In.vNormal 픽셀로컬의 z축을 월드로변환시킨거.. == 정점의 노멀을 선형보간해서 가져온거여.  */
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);

    vNormal = mul(vNormal, WorldMatrix);

	/* 알파테스트 더 이상 렌더스테이츠로 설정한다(x) */
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
	/* Pass : 셰이더 기능의 캡슐화 (여러기법을 모아놓은) */

	/* 빛연산 + 림라이트 + 그림자 + 메탈릭 */
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

