
#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInv, g_ProjMatrixInv;
float4x4 g_LightViewMatrix, g_LightProjMatrix;

float g_fTexW = 1280.0f;
float g_fTexH = 720.0f;

static const float g_fWeight[13] =
{
    0.0044, 0.0175, 0.0540, 0.1295, 0.2420, 0.3521, 0.3989, 0.3521, 0.2420, 0.1295, 0.0540, 0.0175, 0.0044
	// 0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561

};

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
//vector g_vLightSpecular;

texture2D g_NormalTexture;

/* 빛연산이 필요한 모든 픽셀들의 디퓨즈 재질을 픽셀마다 다르게 쓰겠다. */
texture2D g_DiffuseTexture;

/* 빛연산이 필요한 모든 픽셀들의 앰비언트 재질을 모두다 한 값으로 퉁 칠께 */
//texture2D		g_AmbientTexture;
vector    g_vMtrlAmbient = vector(1.f, 1.f, 1.f, 1.f);
vector    g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);

texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_Texture;
texture2D g_LightDepthTexture;
vector    g_vCamPosition;

vector    g_vFogColor = vector(1.f, 0.f, 0.f, 1.f);
float     g_fFogRange = 10.0f;

/* Blur */
texture2D g_BlurTexture;
texture2D g_EffectTexture;
texture2D g_ResultTexture;

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

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
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
    float4 vColor : SV_TARGET0;
};

/* 픽셀의 색을 결정하낟. */
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    //float4 vSpecular : SV_TARGET1;
};

/* 쉐이드 타겟에 사각형 버퍼를 그리는 = 빛마다 빛의 명암 연산결과를 찍어준다. 48만번의 픽셀이 한 프레임당 빛의 갯수만큼 렌더링. */
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	/* 빛연산이 필요한 픽셀의 월드위치가 필요하다. */
	/* 월드위치를 얻어오는 방법! */
	/* 1. 빛연산이 필요한 객체들을 그릴 때, 하나의 렌더타겟을 더 추가하여 픽셀들의 월드위치를 저장받는다. 월드위치는 무한하기때문에 렌더타겟픽셀에 저장하기에 적합하진 않다. */
	/* 2. 현재위치에서 픽셀의 월드위치를 추론해보자. */

    vector vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w == 투영스페이스 상의 위치를 구하낟.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;



	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * (vDepthDesc.y * 3000.f);

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;

    float fDistance = length(vLightDir);

	/* 픽셀의 월드위치과 광원의 위치가 가까우면 1에 가깝게. 광원의 범위보다 멀어지면 0으로. */
    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;

    return Out;
}


/* 최종적으로 백버퍼에 결과장면을 찍어주기위한 쉐이더. 48만번의 픽셀이 한 프레임당 한번 렌더링. */
PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

    //vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade;


	/* 그림자연산을 한다.*/
	/* 현재 그려지는 픽셀이 광원기준으로 플레이어에게 가려져있는 픽셀이었냐를 판단하자. */
	/* 현재 화면에 그려지는 픽셀이 광원기준으로 얼마의 깊이를 가지고 있는지를 구하자.*/	
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 3000.f;

    vector vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w == 투영스페이스 상의 위치를 구하낟.*/
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos = vWorldPos * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);

    float fLightDepth = vWorldPos.w;

    vWorldPos /= vWorldPos.w;

    float2 vLightDepthTexcoord;

    vLightDepthTexcoord.x = vWorldPos.x * 0.5f + 0.5f;
    vLightDepthTexcoord.y = vWorldPos.y * -0.5f + 0.5f;

    vector vOldLightDepth = g_LightDepthTexture.Sample(LinearSampler, vLightDepthTexcoord);
    float fOldLightViewDepth = vOldLightDepth.y * 3000.f;

    vector shadowColor = vector(0.2f, 0.2f, 0.2f, 0.5f);
    
    if (fLightDepth - 0.1f > fOldLightViewDepth)
    {
        Out.vColor.rgb = lerp(Out.vColor.rgb, shadowColor.rgb, shadowColor.a);
    }

    return Out;
}

PS_OUT PS_MAIN_DEFERRED_FOG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);

    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);

    float fViewZ = vDepthDesc.y * 3000.f;

    float fFogPower = max((fViewZ - g_fFogRange) / 10.f, 0.f);
	/*fViewZ
	
	g_fFogRange;*/

    Out.vColor = vDiffuse * vShade;

    Out.vColor = Out.vColor * (1.f - fFogPower) + g_vFogColor * fFogPower;

    return Out;
}

PS_OUT PS_MAIN_RESULT(PS_IN In)
{

    PS_OUT Out = (PS_OUT) 0;

    vector vResult = g_ResultTexture.Sample(LinearSampler, In.vTexcoord);

    vector vBlur = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
	/*vector		vEffect = g_EffectTexture.Sample(LinearSampler, In.vTexcoord);*/

    Out.vColor = vResult + vBlur /*+ vEffect*/;

    return Out;


}

float4 Blur_X(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(1.f / 1280.0f * i, 0.f);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= 6.5f;

    return vOut;
}

float4 Blur_Y(float2 vTexCoord)
{
    float4 vOut = (float4) 0;

    float2 vUV = (float2) 0;

    for (int i = -6; i < 7; ++i)
    {
        vUV = vTexCoord + float2(0, 1.f / g_fTexH * i);
        vOut += g_fWeight[6 + i] * g_EffectTexture.Sample(ClampSampler, vUV);
    }

    vOut /= 6.5f;
    return vOut;
}

PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_X(In.vTexcoord);
	

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = Blur_Y(In.vTexcoord);
	

    return Out;
}

technique11 DefaultTechnique
{
    pass RenderTarget_Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

    pass Deferred_Fog
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED_FOG();
    }

    pass Blur_X
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

    pass Blur_Y
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

    pass Result
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Sky, 0);
        SetBlendState(BS_Default, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_RESULT();
    }

}

