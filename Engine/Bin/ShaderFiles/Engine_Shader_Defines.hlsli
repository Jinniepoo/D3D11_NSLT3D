
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

sampler ClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Wireframe
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = Less_Equal;
};

DepthStencilState DSS_Sky
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;

};

BlendState BS_EffectBlend
{
    BlendEnable[0] = true;
	// BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = one;
    BlendOp = Add;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;
	// BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
    SrcBlendAlpha = One;
    DestBlendAlpha = Zero;
    BlendOpAlpha = Add;
   // RenderTargetWriteMask[0] = Color_Write_Enable_All;
};

BlendState BS_LightBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;


};