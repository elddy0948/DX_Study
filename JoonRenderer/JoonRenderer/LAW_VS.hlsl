cbuffer cbPerObject : register(b0)
{
    float4x4 world;
};

cbuffer cbPass : register(b1)
{
    float4x4 view;
    float4x4 invView;
    float4x4 proj;
    float4x4 invProj;
    float4x4 viewProj;
    float4x4 invViewProj;
    
    float3 eyePos;
    float2 renderTargetSize;
    float2 invRenderTargetSize;

    float nearZ;
    float farZ;
    float totalTime;
    float deltaTime;
};

struct VPos
{
    float3 PosL : POSITION;
};

struct VColor
{
    float4 Color : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_Position;
    float4 Color : COLOR;
};

VertexOut main(VPos vpos_in, VColor vcolor_in)
{
    VertexOut vout;
    
    float4 PosW = mul(float4(vpos_in.PosL, 1.0f), world);
    vout.PosH = mul(PosW, viewProj);
    vout.Color = vcolor_in.Color;
    return vout;
}