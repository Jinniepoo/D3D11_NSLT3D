#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;

float3 g_CameraPlayerPosition;
float g_EffectDistance;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    
    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL; /* 월드 스페이스 노멀 */
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vLifeTime : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);
    
    matrix TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
    float4 worldPosition = mul(float4(In.vPosition, 1.0f), TransformMatrix);
    Out.vPosition = mul(worldPosition, matWVP);
    
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.f), g_WorldMatrix)).xyz;
    Out.vBinormal = cross(Out.vNormal.xyz, Out.vTangent);
    
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    Out.vLifeTime = In.vLifeTime;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vLifeTime : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    float4 vPickedDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord).xyz * 2.f - 1.f;
    float3x3 WorldMatrix = float3x3(In.vTangent, In.vBinormal, In.vNormal.xyz);
    float3 vNormal = mul(vNormalDesc, WorldMatrix);
    
    if (Out.vDiffuse.a <= 0.1f)
        discard;
    
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    Out.vPickedDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 3000.f, 0.f, 0.f);
    
    float3 worldPosition = mul(float4(In.vPosition), g_WorldMatrix).xyz;
    float distanceToCam = distance(worldPosition, g_CameraPlayerPosition);
    
    if (distanceToCam < g_EffectDistance)
    {
        float alphaFactor = saturate(distanceToCam / g_EffectDistance);
        Out.vDiffuse.a *= alphaFactor;
    }

    return Out;
}

technique11 DefaultTechnique
{
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
}
